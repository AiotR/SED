/*===========================================================
 * 6. AZTERKETA: INDUKZIO-PLAKA KONTROL ON/OFF (Dena-Ezer Ez)
 * 80C552 Mikrokontrolagailua - Keil C51
 *
 * SARRERA:
 * Indukzio-plaka baten tenperatura kontrolatzeko algoritmoa.
 * "Dena edo ezerez" (ON-OFF) kontrol-legea da.
 *
 * Pindegia:
 *  P4.0 = Martxa / Geldi (1 = Martxan, 0 = Ahalik eta lasterren gelditu)
 *  P5.0 = Tenperatura Erreferentzia (ADC 0 kanala - Analogikoa)
 *  P5.1 = Tenperatura Sentsorea  (ADC 1 kanala - Analogikoa)
 *  P3.2, P3.3, P3.4, P3.5 = Kontrol Seinalea (Aktibatu 1, Desaktibatu 0)
 *
 * Osziladorea: DISEN-552koa (11.0592 MHz).
 * Timer 0: 1 Moduan (16 bit) erabili behar da.
 *===========================================================*/

#include <reg552.h>

/* --- Pinak --- */
sbit MARTXA_BTN = P4^0;

/* Kontrol Seinaleko pinak (P3) */
sbit KS_1 = P3^2;
sbit KS_2 = P3^3;
sbit KS_3 = P3^4;
sbit KS_4 = P3^5;

/* --- Tenporizadorea --- */
/* 11.0592 MHz osziladorearekin ziklo makina iraupena = 1.085 us.
 * 50ms = 50.000 / 1.085 ≈ 46082 ziklo.
 * 65536 - 46082 = 19454 = 0x4BFE -> Gerturatzen: 0x4C00
 */
#define T0_TH 0x4C
#define T0_TL 0x00

/* --- Aldagai Globalak ---
 * Globalak ezinbesteko kasuetan bakarrik erabili (etendura+main):
 * "Kontrol seinalearen eguneratzea timer-aren etendura funtzioaren barruan egin behar da".
 * Hori dela eta, kalkuluak edo eguneraketak berak INT zerbitzuan egiten dira kasu honetan!
 * Horrela ISRak berak irakurtzen du ADC, funtzio deia bidez.
 */
static unsigned char timer_tick = 0;

/* --- Funtzio Prototipoak --- */
unsigned int ADBihurketa(unsigned char kanala);
void hasieratu(void);


/* =========================================================
 * TIMER 0 ETENDURA (ISR)
 * Seinalea automatikoki 10 segunduro eguneratzeko!
 * ========================================================= */
void timer0_ISR(void) interrupt 1 {
    float t_erref_balioa;
    float t_neurtua_balioa;
    float errorea;

    TH0 = T0_TH;
    TL0 = T0_TL;
    
    /* 10 segundo bete al dira? 
     * 1 seg = 20 ticks (50ms). 10 seg = 200 ticks. */
    timer_tick++;
    if (timer_tick >= 200) {
        timer_tick = 0;
        
        /* Martxan gaude? */
        if (MARTXA_BTN == 1) {
            /* 1) Tenperaturaren Errorea Lortu
             *    formula: e_t = t_erreferentzia - t_neurtua
             *    T_n = (97.5 / 1023) * kode_AD + 2.5
             */
             t_erref_balioa   = (97.5f / 1023.0f) * (float)ADBihurketa(0) + 2.5f;
             t_neurtua_balioa = (97.5f / 1023.0f) * (float)ADBihurketa(1) + 2.5f;
             
             errorea = t_erref_balioa - t_neurtua_balioa;
             
             /* 2) Kontrol On-Off logika 
              * P3_2, P3_3, P3_4, P3_5 soilik ukitu, 
              * gainerakoak bere horretan mantendu P3 atakaren baitan */
             if (errorea > 0.0f) {
                 /* Positiboa => Pin-ak aktibatu (1) */
                 P3 |= 0x3C;  /* b0011 1100 => P3.5, P3.4, P3.3, P3.2 '1' bilakatu (OR) */
             } else {
                 /* Nulua edo negatiboa => Pin-ak desaktibatu (0) */
                 P3 &= ~0x3C; /* Bere alderantzizkoarekin AND eginez '0' jartzen dira */
             }
        }
    }
}

/* =========================================================
 * A/D BIHURKETA
 * ========================================================= */
unsigned int ADBihurketa(unsigned char kanala) {
    ADCON = (0x08 | (kanala & 0x07)); /* 0x08 ADCS abiagailua da */
    while (!(ADCON & 0x10));          /* ADCI bandera */
    return ((unsigned int)ADCH << 2) | (ADCON & 0x03);
}

/* =========================================================
 * HASIERAKETA
 * ========================================================= */
void hasieratu(void) {
    /* P4 Sarrera bezala irakurtzeko */
    P4 |= 0x01; 
    
    /* P3.2-P3.5 kontrol moduan erabiliko direnez hasierako balioa baxua */
    P3 &= ~0x3C;
    
    /* Timer 0, Modu 1 */
    TMOD &= 0xF0;
    TMOD |= 0x01;
    TH0 = T0_TH;
    TL0 = T0_TL;
    
    ET0 = 1;
    EA  = 1;
    
    timer_tick = 0;
    TR0 = 1; /* Piztu timerra */
}

/* =========================================================
 * FUNTZIO NAGUSIA
 * ========================================================= */
void main(void) {
    hasieratu();
    
    while(1) {
        /* "0 logikoa sartzean, sistema ahalik eta lasterren geldituko da." */
        if (MARTXA_BTN == 0) {
            /* Geldi ezazu indukzio plaka berehala */
            P3 &= ~0x3C; 
            /* Kontagailua reset egin, berriro ON jartzerakoan 10s berriak hasi arte ez kontrolatzeko,
             * edota hasierako neurtzea errazteko. 
             * Enuntziatuak ez du adierazten zer gertatzen den berehala martxan jartzean.
             * Seguruena timer_tick zeroratzea da. */
            timer_tick = 0;
        }
        
        /* Oharra: Gainerako kode guztia automatikoki eta zorrozki 10 segunduro
         * ISR (timer_0_interrupt) funtzioan egiten da, exijentziaren arabera. */
    }
}
