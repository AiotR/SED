/*===========================================================
 * 2. AZTERKETA: BATAPLAN ARGIZKO ERROTULUA (2016-06-27)
 * 80C552 Mikrokontrolagailua - Keil C51
 *
 * SARRERA:
 * "BATAPLAN" hitza argizko errotulu batean konfigurazioak
 * erakusteko programa bat da, 3. praktikan egindako
 * "ZORIONAK" errotuluaren antzekoa baina timer etendura bidez.
 *
 * P1 ataka (Letrak):
 *   P1.7 = B    P1.3 = P
 *   P1.6 = A    P1.2 = L
 *   P1.5 = T    P1.1 = A
 *   P1.4 = A    P1.0 = N
 *
 * P4 ataka (Kontrola):
 *   P4.0 = Martxa (1) / Gelditu (0)
 *   P4.1 = Noranzkoa (1 = Goitik behera / 0 = Behetik gora)
 *   P4.2 = Abiadura (1 = 1s / 0 = 0.5s)
 *===========================================================*/

#include <reg552.h>

/* --- Pin definizioak --- */
#define L_MARTXA    (P4 & 0x01)     /* P4.0 bit azterketa */
#define L_NORANZKOA (P4 & 0x02)     /* P4.1 bit azterketa */
#define L_ABIADURA  (P4 & 0x04)     /* P4.2 bit azterketa */

/* --- Baldintzen Parametroak (Osziladorea 12MHz = 1us/ziklo) --- 
 * Timer0 = Modu 1 (16 bit). 
 * 50ms-ko etendurak egingo ditugu denbora luzeak (1s, 0.5s) errazago kontatzeko.
 * 50ms = 50.000 us -> 65536 - 50000 = 15536 = 0x3CB0 */
#define TIMER0_TH  0x3C
#define TIMER0_TL  0xB0

/* --- Aldagai Globalak (ahalik eta gutxien) --- */
static unsigned char volatile timer_tick = 0; /* 50ms-ko tick-ak kontatzeko */
static bit eguneratu_behar_da = 0;   /* Letrak eguneratzeko bandera */

/* --- Errotuluaren Konfigurazio Taula (18 egoera) ---
 * Memorian ('code' ROM) bakarrik gordetzeko. 
 * 'B A T A P L A N'
 *  7 6 5 4 3 2 1 0  (bitak)
 */
unsigned char code bataplan[18] = {
    0x80, /* 00: B------- */
    0x40, /* 01: -A------ */
    0x20, /* 02: --T----- */
    0x10, /* 03: ---A---- */
    0x08, /* 04: ----P--- */
    0x04, /* 05: -----L-- */
    0x02, /* 06: ------A- */
    0x01, /* 07: -------N */
    0xAA, /* 08: B-T-P-A- (1010 1010) */
    0x55, /* 09: -A-A-L-N (0101 0101) */
    0x80, /* 10: B------- */
    0xC0, /* 11: BA------ */
    0xE0, /* 12: BAT----- */
    0xF0, /* 13: BATA---- */
    0xF8, /* 14: BATAP--- */
    0xFC, /* 15: BATAPL-- */
    0xFE, /* 16: BATAPLA- */
    0xFF  /* 17: BATAPLAN */
};
#define NUM_EGOERA 18

/* =========================================================
 * TIMER 0 ETENDURA (Funtzio 1)
 * 50ms-ro deitzen da. Abiadura (P4.2) zein den ikusita,
 * behar den denbora pasatu den neurtuko du.
 * ========================================================= */
void timer0_ISR(void) interrupt 1 using 1 {
    /* Timer-a berrarmatu */
    TH0 = TIMER0_TH;
    TL0 = TIMER0_TL;
    
    /* P4.0 = '0' bada (Geldik), ez kontatu ezer, mantendu a 0 */
    if (!L_MARTXA) {
        timer_tick = 0;
        P1 = 0x00; /* Errotulua itzali exijentzian *ez* bada argi jartzen badu ere hobe */
        return;
    }

    timer_tick++;
    
    /* Abiadura neurtu:
     * P4.2 = 1 -> 1 segundo (20 tick * 50ms)
     * P4.2 = 0 -> 0.5 segundo (10 tick * 50ms) */
    if (L_ABIADURA) {
        if (timer_tick >= 20) {
            timer_tick = 0;
            eguneratu_behar_da = 1;
        }
    } else {
        if (timer_tick >= 10) {
            timer_tick = 0;
            eguneratu_behar_da = 1;
        }
    }
}

/* =========================================================
 * HASIERA (Hasierapenak)
 * ========================================================= */
void hasieratu(void) {
    /* Portuak */
    P1 = 0x00;
    P4 = 0x00;
    
    /* Timer 0 Modu 1 (16 bit) */
    TMOD &= 0xF0;
    TMOD |= 0x01;
    
    TH0 = TIMER0_TH;
    TL0 = TIMER0_TL;
    
    /* Etendurak gaitu */
    ET0 = 1; /* Timer 0 */
    EA  = 1; /* Orokorra */
    
    TR0 = 1; /* Timer 0 martxan hasi! */
}

/* =========================================================
 * FUNTZIO NAGUSIA
 * ========================================================= */
void main(void) {
    signed char indizea = 0; /* signed erabili atzerako zenbatzean <0 detektatzeko */
    
    hasieratu();
    
    while(1) {
        if (eguneratu_behar_da && L_MARTXA) {
            eguneratu_behar_da = 0;
            
            /* P4.1 noranzkoa ebaluatu */
            if (L_NORANZKOA) {
                /* 1 = Goitik behera (0 -> 17) */
                indizea++;
                if (indizea >= NUM_EGOERA) {
                    indizea = 0;
                }
            } else {
                /* 0 = Behetik gora (17 -> 0) */
                indizea--;
                if (indizea < 0) {
                    indizea = NUM_EGOERA - 1;
                }
            }
            
            /* P1 atakari taulako balio berria bota */
            P1 = bataplan[indizea];
        }
        
        /* Martxa kentzean asikera posiziora ezarri nahi badugu */
        if (!L_MARTXA) {
            indizea = 0;
        }
    }
}
