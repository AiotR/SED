/*===========================================================
 * 5. AZTERKETA: GIE2 EIEKZIO SISTEMA (2024-06-10)
 * 80C552 Mikrokontrolagailua - Keil C51
 *
 * SARRERA:
 * Lockheed Martin hegazkin ultra-azkarraren simulagailua
 * eserlekua eta garlinga askatzeko.
 *
 * P1.0 = Garlinga askatu
 * P1.1 = Eserleku paraxuta
 * P1.2 = Maskara askatu
 * P1.3 = Alarma
 * P1.5 = Pilotu paraxuta
 * P1.6 = Eiekzioa aktibatu
 * P1.7 = Uhalak estutu
 * P4.0 = Eraztuna (Tiratzeko botoia, "1" aktibatzen da)
 * P4.1 = Arnas sentsorea
 * P4.7 = Berrabiarazi botoia ("1" aktibatzen da)
 * P5.2 = Arrisku maila ADC bidez (0 to 100)
 *===========================================================*/

#include <reg552.h>

/* --- Pinak --- */
sbit GARLINGA_ASKATU  = P1^0;
sbit ESER_PARAXUTA    = P1^1;
sbit MASKARA_ASKATU   = P1^2;
sbit ALARMA           = P1^3;
sbit PILOTU_PARAXUTA  = P1^5;
sbit EIEKZIOA_AKTIB   = P1^6;
sbit UHALAK_ESTUTU    = P1^7;

sbit ERAZTUNA         = P4^0;
sbit ARNAS_SENTSOREA  = P4^1;
sbit BERRABIARAZI_BTN = P4^7;

/* --- Aldagai Globalak --- */
static bit gie2_martxan = 0;
static bit arnas_fasea  = 0;
static unsigned int timer_ticks = 0;     /* 50ms-ko etendurak kontatzeko */
static unsigned int segundu_kont  = 0;   /* GIE2 sekuentziaren segunduak */
static bit segundu_flag = 0;             /* ISR-tik main-era segundu 1 pasa dela adierazteko */

static unsigned int alarma_ticks = 0;    /* Alarma 10s kontagailua */
static bit alarma_martxan = 0;

/* Timer 0 Modu 1 (16 bit), 12 MHz -> 1us/ziklo
 * 50ms = 50.000 ziklo = 0xC350.
 * 65536 - 50000 = 15536 = 0x3CB0
 */
#define T0_TH 0x3C
#define T0_TL 0xB0

/* =========================================================
 * TIMER 0 ISR - ~50ms
 * 3. ERREGISTRO BANKUA ERABILIZ (using 3)
 * ========================================================= */
void timer0_ISR(void) interrupt 1 using 3 {
    TH0 = T0_TH;
    TL0 = T0_TL;
    
    timer_ticks++;
    if (timer_ticks >= 20) { /* 1 segundo = 20 * 50ms */
        timer_ticks = 0;
        
        if (gie2_martxan) {
            segundu_kont++;
            segundu_flag = 1;
        }
        
        if (alarma_martxan) {
            alarma_ticks++;
        }
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
 * HASIERAKETA
 * ========================================================= */
void hasieratu(void) {
    /* Portuak (Sarrerak = 1) */
    P1 = 0x00; /* P1 irteera moduan nagusiki */
    P4 |= 0x83; /* P4.0, P4.1 eta P4.7 sarrera gisa erabiliko dira */
    
    /* Timer 0 Modu 1 */
    TMOD &= 0xF0;
    TMOD |= 0x01;
    TH0 = T0_TH;
    TL0 = T0_TL;
    
    ET0 = 1;
    EA  = 1;
    TR0 = 1;
    
    gie2_martxan = 0;
    arnas_fasea = 0;
    alarma_martxan = 0;
}

void garbitu_irteerak(void) {
    P1 = 0x00;
}

/* =========================================================
 * FUNTZIO NAGUSIA
 * ========================================================= */
void main(void) {
    unsigned int adc_balioa;
    float arrisku_maila;

    hasieratu();
    
    while(1) {
        
        /* 1) Sentsore analogikoa: Erorketa askearen arrisku maila P5.2 */
        adc_balioa = ADBihurketa(2);
        arrisku_maila = (100.0f * (float)adc_balioa) / 1023.0f;
        
        /* 2) Sistema Berrabiarazi Botoia (P4.7) */
        if (BERRABIARAZI_BTN == 1) {
            garbitu_irteerak();
            gie2_martxan = 0;
            arnas_fasea = 0;
            alarma_martxan = 0;
            segundu_kont = 0;
            alarma_ticks = 0;
            continue;
        }

        /* 3) GIE2 SISTEMAREN AKTIBAZIOA */
        /* Pilotuak eraztunetik tiratzen badu EDO arriskua > 80 bada */
        if (!gie2_martxan) {
            if (ERAZTUNA == 1 || arrisku_maila > 80.0f) {
                /* GIE2 Martxan */
                gie2_martxan = 1;
                alarma_martxan = 0; /* Alarma itzali badaude */
                ALARMA = 0;
                
                UHALAK_ESTUTU = 1;
                GARLINGA_ASKATU = 1;
                
                segundu_kont = 0;
                timer_ticks = 0;
                segundu_flag = 0;
                arnas_fasea = 0;
            } 
            /* Bestela, arriskua 50-80 tartean badago */
            else if (arrisku_maila >= 50.0f && arrisku_maila <= 80.0f) {
                if (!alarma_martxan) {
                    alarma_martxan = 1;
                    ALARMA = 1;
                    alarma_ticks = 0;
                    timer_ticks = 0;
                } else {
                    /* Alarma barruan: 10 segunduen kontrola */
                    if (alarma_ticks >= 10) {
                        alarma_martxan = 0;
                        ALARMA = 0;
                        
                        /* 10s ondoren berriro begiratu, oraindik handia bada GIE2 piztu */
                        if (arrisku_maila >= 50.0f) {
                            gie2_martxan = 1;
                            UHALAK_ESTUTU = 1;
                            GARLINGA_ASKATU = 1;
                            segundu_kont = 0;
                        }
                    }
                }
            }
            /* Arriskua paretzen bada (<50), alarma itzali */
            else if (alarma_martxan && arrisku_maila < 50.0f) {
                alarma_martxan = 0;
                ALARMA = 0;
                alarma_ticks = 0;
            }
        }
        
        /* 4) GIE2 SISTEMAREN SEKUENZIA */
        if (gie2_martxan && segundu_flag) {
            segundu_flag = 0;
            
            /* Garlingaren kontra talka ez egiteko, 5 segundora eiekzioa */
            if (segundu_kont == 5) {
                EIEKZIOA_AKTIB = 1;
            }
            /* Eiekziotik 3 segundora (5+3=8), eserleku paraxuta */
            if (segundu_kont == 8) {
                ESER_PARAXUTA = 1;
                arnas_fasea = 1; /* Arnas sentsorearen irakurketarako prestatuta */
            }
        }
        
        /* 5) ERLIBERAZIO FINALA (ARNAS SENTSOREA) */
        if (arnas_fasea && ARNAS_SENTSOREA == 1) {
            PILOTU_PARAXUTA = 1;
            MASKARA_ASKATU = 1;
            UHALAK_ESTUTU = 0; /* Uhalak lasaitu pilotua askatzeko */
            
            arnas_fasea = 0; /* Behin bakarrik egiteko */
        }
    }
}
