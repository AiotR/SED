/*===========================================================
 * 7. AZTERKETA: SEMAforoa eta IZOTZA DETEKTATZEA (2019-07-01)
 * 80C552 Mikrokontrolagailua - Keil C51
 *
 * SARRERA:
 * Autoen ibilbidea gobernatzen duen semaforoa (Berde/Laranja/Gorri)
 * eta izotzaren ohartarazpen panela ADC bidez neurtua.
 * 
 * Pinak:
 *   P1.0 = Kolore berdea (1 logikoa = piztu)
 *   P1.1 = Kolore laranja
 *   P1.2 = Kolore gorria
 *   P1.7 = Izotz panela ("Kontuz, izotza!")
 *   P4.0 = Oinezkoen pultsagailua (1 logiko = zapalduta)
 *   P5.3 = Tenperatura sentsorea (ADC 3. kanala)
 *
 * Denborak eta arauak:
 *   Berdea = 10s, Laranja = 2s, Gorria = 7s.
 *   Tenperatura = (70.0/1023)*k - 20.0
 *   Timer 1 Modu 2an (8-bit auto-reload) zenbatzeko, 11.0592 MHz.
 *===========================================================*/

#include <reg552.h>

/* --- Pinak --- */
sbit BERDEA       = P1^0;
sbit LARANJA      = P1^1;
sbit GORRIA       = P1^2;
sbit IZOTZ_PANELA = P1^7;

sbit OINEZKO_BTN  = P4^0; 

/* --- TIMER 1 MODU 2 (8-bit Auto-reload) ---
 * Osziladorea: 11.0592 MHz -> 1 Ziklo = 1.085 us.
 * 8-bit tenporizadore batentzat 255 baino ezin da kontaketa maximoa izan.
 * 230 ziklo x 1.085 us = 249.56 us ≈ 0.25 ms.
 * Beraz, errekarga -> TH1 = (256 - 230) = 26 = 0x1A.
 * Horrela ISR-an 4 aldiz sartzean ≈ 1 ms pasako da.
 */
#define T1_ERREKARGA 0x1A

/* --- Aldagai Globalak (ahalik eta gutxien) --- */
static unsigned char egoera_sema = 0; /* 0=Berdea, 1=Laranja, 2=Gorria */
static unsigned int ms_semaforokoa = 0; /* Denbora kontrola egoera bakoitzean (0-10000 ms) */
static unsigned int ms_tenperatura = 0; /* Tenperatura ADC polling egiteko kontagailua */

static unsigned char t1_ticks = 0;
static bit b_oinezko = 0; /* Botoia sakatzeko atzerapena saihesteko bandera (sakaketa isolatzeko) */

/* =========================================================
 * TIMER 1 ISR - 249.5 us koroa
 * ========================================================= */
void timer1_ISR(void) interrupt 3 {
    /* Modu 2an dagoenez, TH1 balioa TL1-en automatikoki kargatzen da,
     * ez da berridatzi behar ISR barruan eszepziok! */
     
    t1_ticks++;
    if (t1_ticks >= 4) {
        t1_ticks = 0;
        
        /* 1 Milisegundo pasata */
        ms_semaforokoa++;
        ms_tenperatura++;
    }
}

/* =========================================================
 * A/D BIHURKETA
 * ========================================================= */
unsigned int ADBihurketa(unsigned char kanala) {
    if (kanala > 7) kanala = 0;
    ADCON = (0x08 | kanala);
    while (!(ADCON & 0x10));
    return ((unsigned int)ADCH << 2) | (ADCON & 0x03);
}

/* =========================================================
 * SEMAforoko IRTEERAK EGUNERATU
 * ========================================================= */
void eguneratuSemaforoa(unsigned char egoera_berria) {
    BERDEA = 0;
    LARANJA = 0;
    GORRIA = 0;
    
    if (egoera_berria == 0) BERDEA = 1;
    else if (egoera_berria == 1) LARANJA = 1;
    else if (egoera_berria == 2) GORRIA = 1;
    
    egoera_sema = egoera_berria;
    ms_semaforokoa = 0; /* Egoera aldatzean, kontagailua hutsetik (denbora berrabiarazi) */
}

/* =========================================================
 * HASIERAKETA
 * ========================================================= */
void hasieratu(void) {
    /* Portuak */
    P1 = 0x00; /* P1.0, 1.1, 1.2, 1.7 Irteera bezala (0 hasieran) */
    P4 |= 0x01; /* P4.0 Sarrera gisa */
    
    /* Timer 1, Modu 2 (8-bit auto-reload)
     * TMOD reg-> Timer 1 zatia MSB nibblea da. b0010 = 0x20
     */
    TMOD &= 0x0F;
    TMOD |= 0x20;
    
    /* TH1 kargatu, TL1 ere kargatu lehen itzulirako prestatzeko */
    TH1 = T1_ERREKARGA;
    TL1 = T1_ERREKARGA;
    
    t1_ticks = 0;
    ms_semaforokoa = 0;
    /* Tenperatura hasieran irakurri dadin zehaztu balio altua emanez */
    ms_tenperatura = 10000; 
    
    ET1 = 1; /* Timer 1 Int on */
    EA  = 1;
    TR1 = 1; /* Timer 1 on */
    
    eguneratuSemaforoa(0); /* Hasieran Berdea */
}

/* =========================================================
 * FUNTZIO NAGUSIA
 * ========================================================= */
void main(void) {
    float tenperatura;
    unsigned int konbertsioa;
    
    hasieratu();
    
    while(1) {
        
        /* --- OINEZKOEN BOTOIA KUDEAKETA --- */
        if (OINEZKO_BTN == 1) {
            if (!b_oinezko) {
                b_oinezko = 1; /* Sakatu berritan bakarrik ekintza bat hartu */
                
                if (egoera_sema == 0) {
                    /* Berdean badago -> Laranja kolorera pasatu berehala */
                    eguneratuSemaforoa(1);
                }
                else if (egoera_sema == 2) {
                    /* Gorrian badago -> Beste 7 segundoz egon (denbora berrabiarazita) */
                    eguneratuSemaforoa(2);
                }
            }
        } else {
            b_oinezko = 0; /* Askatu egin da */
        }
        
        /* --- SEMAforoko DENBORA KUDEAKETA --- */
        if (egoera_sema == 0) {
            /* Berdea: 10 segundo */
            if (ms_semaforokoa >= 10000) {
                eguneratuSemaforoa(1); /* Pasatu Laranjara */
            }
        }
        else if (egoera_sema == 1) {
            /* Laranja: 2 segundo */
            if (ms_semaforokoa >= 2000) {
                eguneratuSemaforoa(2); /* Pasatu Gorrira */
            }
        }
        else if (egoera_sema == 2) {
            /* Gorria: 7 segundo */
            if (ms_semaforokoa >= 7000) {
                eguneratuSemaforoa(0); /* Pasatu Berdera */
            }
        }
        
        /* --- TENPERATURAREN KUDEAKETA (10s behin irakurri) --- */
        if (ms_tenperatura >= 10000) {
            ms_tenperatura = 0;
            
            konbertsioa = ADBihurketa(3); /* P5.3 -> 3. kanala */
            tenperatura = (70.0f / 1023.0f) * (float)konbertsioa - 20.0f;
            
            if (tenperatura < 0.0f) {
                IZOTZ_PANELA = 1; /* Kontuz izotza piztu */
            } else {
                IZOTZ_PANELA = 0; /* Itzali */
            }
        }
    }
}
