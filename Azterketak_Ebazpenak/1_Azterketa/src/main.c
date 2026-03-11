/*===========================================================
 * 1. AZTERKETA: NAHASKETA SISTEMA (2022-06-13)
 * 80C552 Mikrokontrolagailua - Keil C51
 *
 * SARRERA:
 * Ura eta produktu baten arteko nahasketa kudeatzen duen sistema.
 * 
 * HW KONFIGURAZIOA:
 * - P3.0: START pultsagailua (1 logikoa sakatzean)
 * - P3.2: STOP pultsagailua (0 logikoa sakatzean - INT0)
 * - P4.0: V1 Balbula (Ura)
 * - P4.1: V2 Balbula (Produktua)
 * - P4.2: KONPORTA
 * - P4.7: LED (Martxan)
 * - P5.5: Pisu sentsorea (Analogikoa - AD5 kanala)
 *===========================================================*/

#include <reg552.h>

/* --- Bit Definizioak --- */
sbit START_BTN = P3^0;
sbit STOP_BTN  = P3^2; /* INT0 sarrera ere bada */

sbit V1_URA     = P4^0;
sbit V2_PROD    = P4^1;
sbit KONPORTA   = P4^2;
sbit LED_RUN    = P4^7;

/* --- Egoera Makina --- */
typedef enum {
    GELDI,
    URA_ISURI,
    PROD_ISURI,
    KONPORTA_IREKI
} Egoera;

/* --- Aldagai Globalak --- */
static Egoera egoera_nagusia = GELDI;
static unsigned int ms_kontadorea = 0;
static bit timer_flag = 0;

/* --- Funtzio Prototipoak --- */
void hasieratu(void);
unsigned int ADBihurketa(unsigned char kanala);
float lortu_gramoak(unsigned int kodea);

/* =========================================================
 * TIMER 1 ISR - 22ms-ro
 * Denbora zenbatzeko erabiliko dugu.
 * 12MHz-ko osziladorea -> 1 ziklo = 1us.
 * 22ms = 22.000us -> 65536 - 22000 = 43536 (0xAA10)
 * ========================================================= */
void timer1_ISR(void) interrupt 3 {
    TH1 = 0xAA;
    TL1 = 0x10;
    timer_flag = 1; /* Begizta nagusiari abisatu */
}

/* =========================================================
 * KANPO ETENDURA 0 (INT0) - STOP Pultsagailua
 * P3.2 pinean 0 (falling edge) detektatzean exekutatzen da.
 * ========================================================= */
void stop_ISR(void) interrupt 0 {
    /* Berehalako geldialdia */
    V1_URA = 0;
    V2_PROD = 0;
    KONPORTA = 0;
    LED_RUN = 0;
    
    egoera_nagusia = GELDI;
    ms_kontadorea = 0;
}

/* =========================================================
 * HASIERAKETA FUNTZIOA
 * ========================================================= */
void hasieratu(void) {
    /* Portuen hasierako egoera */
    P1 = 0x00;
    P4 = 0x00;
    
    /* Timer 1: Mode 1 (16-bit) */
    TMOD &= 0x0F; /* Timer1 zatia garbitu */
    TMOD |= 0x10; /* Timer1 Mode 1 */
    
    TH1 = 0xAA;
    TL1 = 0x10;
    
    /* Etendurak gaitu */
    ET1 = 1; /* Timer 1 interrupt */
    EX0 = 1; /* External interrupt 0 (STOP) */
    IT0 = 1; /* INT0 Falling edge (0 logikoa sakatzean) */
    
    EA = 1;  /* Interrupt orokorrak */
    TR1 = 1; /* Timer 1 martxan */
    
    egoera_nagusia = GELDI;
}

/* =========================================================
 * AD BIHURKETA
 * P5.5 sentsorea irakurtzeko (5. kanala)
 * ========================================================= */
unsigned int ADBihurketa(unsigned char kanala) {
    if (kanala > 7) kanala = 0;
    
    ADCON = (unsigned char)(0x08 | kanala); /* ADCS = 1 */
    while (!(ADCON & 0x10)); /* ADCI zain */
    
    return ((unsigned int)ADCH << 2) | (ADCON & 0x03);
}

/* =========================================================
 * GRAMOEN KALKULUA
 * Gramoak = (10 / 1023) * kodea
 * ========================================================= */
float lortu_gramoak(unsigned int kodea) {
    return (10.0f / 1023.0f) * (float)kodea;
}

/* =========================================================
 * MAIN
 * ========================================================= */
void main(void) {
    unsigned int ad_kodea;
    float pisu_gramoak;

    hasieratu();

    while(1) {
        if (timer_flag) {
            timer_flag = 0;
            
            /* Egoera makinaren logika */
            switch (egoera_nagusia) {
                
                case GELDI:
                    if (START_BTN == 1) {
                        LED_RUN = 1;
                        V1_URA = 1;
                        ms_kontadorea = 0;
                        egoera_nagusia = URA_ISURI;
                    }
                    break;
                
                case URA_ISURI:
                    ms_kontadorea += 22; /* 22ms gehitzen dugu */
                    if (ms_kontadorea >= 3000) { /* 3 segundo */
                        V1_URA = 0;
                        V2_PROD = 1;
                        egoera_nagusia = PROD_ISURI;
                    }
                    break;
                
                case PROD_ISURI:
                    ad_kodea = ADBihurketa(5); /* P5.5 sentsorea */
                    pisu_gramoak = lortu_gramoak(ad_kodea);
                    
                    if (pisu_gramoak >= 10.0f) {
                        V2_PROD = 0;
                        KONPORTA = 1;
                        ms_kontadorea = 0;
                        egoera_nagusia = KONPORTA_IREKI;
                    }
                    break;
                
                case KONPORTA_IREKI:
                    ms_kontadorea += 22;
                    if (ms_kontadorea >= 5000) { /* 5 segundo */
                        KONPORTA = 0;
                        LED_RUN = 0;
                        egoera_nagusia = GELDI;
                    }
                    break;
                
                default:
                    egoera_nagusia = GELDI;
                    break;
            }
        }
    }
}
