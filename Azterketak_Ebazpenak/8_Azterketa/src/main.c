/*===========================================================
 * 8. AZTERKETA: SU ARTIFIZIALAK (PALMONDOA) (2020-06-12)
 * 80C552 Mikrokontrolagailua - Keil C51
 *
 * SARRERA:
 * Palmera itxurako su artifizialen sekuentzia.
 * P1.0 = Hasi prozesua ("1" logiko bidez)
 * P1.7 = Larrialdi etengailua (1 jasotzean -> Gelditu, gillotina ON)
 * P4.0 = Enborra
 * P4.1 = Hostoak
 * P4.2 = Eguzkia
 * P4.7 = Gillotina
 *
 * Denborak (Kontagailu bakarrarekin!):
 *   T=0s -> Enborra ON (P4.0)
 *   T=5s -> Hostoak ON (P4.1)
 *   T=8s (5+3) -> Eguzkia ON (P4.2)
 *   T=12s (8+4) -> Dena OFF (P4.0, P4.1, P4.2)
 *
 * Osziladorea 11.0592 MHz. Timer 0 2. moduan. ISR 3. bankuan.
 *===========================================================*/

#include <reg552.h>

/* --- Pinak --- */
sbit HASI_BTN        = P1^0;
sbit LARRI_ETENGAILU = P1^7;

sbit ENBORRA   = P4^0;
sbit HOSTOAK   = P4^1;
sbit EGUZKIA   = P4^2;
sbit GILLOTINA = P4^7;

/* --- TIMER 0 MODU 2 (8-bit Auto-reload) ---
 * 11.0592 MHz -> 1 Ziklo = 1.085 us.
 * TH0 = 256 - 230 = 26 = 0x1A -> 249.56 us.
 * 4 ISR exekuzio ≈ 1 ms.
 */
#define T0_ERREKARGA 0x1A

/* --- Aldagai Globalak --- */
static bit prozesua_martxan = 0;
static bit larrialdia       = 0;

/* Kontagailu bakarra eskatzen du (ms milisegundutan, T=12s arte) */
static unsigned int ms_denbora = 0; /* Gehienez 12000 ms, 16bit nahikoa da */
static unsigned char t0_ticks  = 0;

/* Aldagai globala parametroa erakusteko: "modua" */
unsigned char modua = 2; 

/* =========================================================
 * TIMER 0 ISR - 3. erregistro bankua
 * ========================================================= */
void timer0_ISR(void) interrupt 1 using 3 {
    /* Modu 2 denez, TL0 automatikoki kargatzen da TH0 baliotik */
    t0_ticks++;
    if (t0_ticks >= 4) {
        t0_ticks = 0;
        
        /* 1 ms igaro da */
        if (prozesua_martxan && !larrialdia) {
            ms_denbora++;
        }
    }
}

/* =========================================================
 * HASIERAKETA (Funtzionamendu Aurreratua paramatroduna)
 * ========================================================= */
void hasieratu(unsigned char timer_modua) {
    /* Portuak */
    P1 = 0x00; /* P1 sarrera ezaugarriekin irakurtzeko erabiliko da begiztan if() bidez, lehenespenez itzalita irteerak */
    P4 = 0x00; /* Irteerak itzalita (0) */
    
    TMOD &= 0xF0; /* Timer 0 bitak garbitu */
    
    if (timer_modua == 1) {
        TMOD |= 0x01; /* Modu 1 (nahiz eta azterketak 2 exijitu, Funtzionamendu Aurreratuak eskatzen du malgutzea) */
        /* TH/TL ezberdin kargatu beharko litzateke hemen baina azterketa xede modura ez da garatzen 1.a benetan */
    } 
    else if (timer_modua == 2) {
        TMOD |= 0x02; /* Modu 2 */
        TH0 = T0_ERREKARGA;
        TL0 = T0_ERREKARGA;
    }
    
    t0_ticks = 0;
    ms_denbora = 0;
    
    ET0 = 1; /* Timer 0 int */
    EA  = 1;
    TR0 = 1;
}

void kudeatu_larrialdia(void) {
    if (LARRI_ETENGAILU == 1) {
        larrialdia = 1;
        /* Metxak moztu gillotina erabiliz */
        GILLOTINA = 1;
        /* Eragingailuak desaktibatu */
        ENBORRA = 0;
        HOSTOAK = 0;
        EGUZKIA = 0;
        /* Palmondoa geratu */
        prozesua_martxan = 0;
    } else {
        larrialdia = 0;
        GILLOTINA = 0;
    }
}

/* =========================================================
 * FUNTZIO NAGUSIA
 * ========================================================= */
void main(void) {
    /* Modu aldagaia globala definitu da: 2. modua ezarriko dugu */
    hasieratu(modua);
    
    while(1) {
        /* Larrialdien Lehenespena */
        kudeatu_larrialdia();
        
        /* Sistema abiaraztea */
        if (!prozesua_martxan && !larrialdia) {
            if (HASI_BTN == 1) {
                prozesua_martxan = 1;
                /* Hasieran SOILIK reseteatzen da kontagailu bakarra */
                ms_denbora = 0; 
                
                /* T=0 -> Enborra jauzi */
                ENBORRA = 1;
            }
        }
        
        /* Palmondo prozesua sekuentziala (kontagailu bakarraz gidatuta) */
        if (prozesua_martxan && !larrialdia) {
            
            /* T = 5 segundo */
            if (ms_denbora == 5000) {
                HOSTOAK = 1;
            }
            /* T = 8 segundo (5+3) */
            else if (ms_denbora == 8000) {
                EGUZKIA = 1;
            }
            /* T = 12 segundo (8+4) -> Dena amaitu */
            else if (ms_denbora >= 12000) {
                ENBORRA = 0;
                HOSTOAK = 0;
                EGUZKIA = 0;
                /* Hurrengo prozesuari bide eman */
                prozesua_martxan = 0; 
            }
        }
    }
}
