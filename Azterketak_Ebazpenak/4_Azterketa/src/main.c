/*===========================================================
 * 4. AZTERKETA: UR-MAILA ETA TENPERATURA KONTROLA (2019-05-31)
 * 80C552 Mikrokontrolagailua - Keil C51
 *
 * SARRERA:
 * Depositu nabigusiaren ur-maila eta tenperatura kontrolatzea.
 * 
 * HW SARRERAK:
 *  - P1.0: Martxa etengailua (1 logikoa = martxan)
 *  - P1.1 (S1): Sentsore digitala (1 = ur-maila S1etik gora)
 *  - P1.2 (S2): Sentsore digitala (1 = ur-maila S2tik gora)
 *  - P1.7: Tº kontrola etengailua (1 = kontrola martxan)
 *  - P3.2: Mantentze-lanak etengailua (OFF=1, ON=0 -> INT0 falling edge)
 *  - P5.5: ADC (Uraren tenperaturaren sentsorea analagikoa, 5 kanala)
 *          tenp = (130.0 / 1023) * kodeDigitala - 30.0
 * 
 * HW IRTEERAK (P4):
 *  - P4.0 (LED_M): Sistema martxan dagoen adierazlea (1 = piztu)
 *  - P4.1: Hustutze balbula (0 = itxi, 1 = ireki)
 *  - P4.2: Ur-ponpa (0 = geratu, 1 = martxan)
 *  - P4.6: Tº-aren kontrol seinalea (1 = berotu ura)
 *  - P4.7 (LED_T): Tenperatura kontrolaren adierazlea (1 = piztu)
 *===========================================================*/

#include <reg552.h>

/* --- Pinak --- */
sbit MARTXA_BTN    = P1^0;
sbit S1_SENTSORE   = P1^1;
sbit S2_SENTSORE   = P1^2;
sbit T_KONTROL_BTN = P1^7;

sbit LED_M         = P4^0;
sbit HUSTUTZE_BALB = P4^1;
sbit UR_PONPA      = P4^2;
sbit T_SEINALEA    = P4^6;
sbit LED_T         = P4^7;

/* Mantentze lanak -> P3.2, baina INT0 bidez kudeatuko da (IT0 = 1).
 * "Hautatzailea ON posizioan jartzean ("0" logikoa) sartu mantentze lanetan, 
 * ireteera guztiak desaktibatuta OFF posiziora bueltatu arte."
 */
sbit MANTEN_BTN = P3^2; 

/* --- TIMER 0 (1 Modua, 16 bit) --- 
 * Osziladorea = 11.0592 MHz.
 * Ziklo 1 = 12 / 11.0592MHz = 1.085 us.
 * 50ms = 50.000 us.
 * Tikak = 50000 / 1.085 = 46082 ziklo.
 * 65536 - 46082 = 19454 = 0x4BFE (-ish).
 * Zehazki: 0x4C00 erabiliko dugu gerturapen eroso baterako (~50ms). */
#define T0_TH 0x4C
#define T0_TL 0x00

/* --- Aldagai Globalak (ahalik eta gutxien) --- */
static bit betetze_prozesuan = 0;   /* Depositua betetzen ari garen ala ez */
static bit t_kontrola_martxan = 0;  /* Tenperatura kontrola exekutatzen ari denean */

/* Kontagailuak Timer 0-tik (50ms x 20 = 1 segundo) */
static unsigned int ticks_50ms = 0;
static signed char betetze_segunduak = 0; 
static signed char temp_segunduak = 0;

/* Mantenimendu etendurarentzat bandera bat */
static bit mantentzen = 0; 

/* =========================================================
 * TIMER 0 ISR - ~50ms
 * ========================================================= */
void timer0_ISR(void) interrupt 1 using 1 {
    TH0 = T0_TH;
    TL0 = T0_TL;
    
    ticks_50ms++;
    if (ticks_50ms >= 20) { /* 1 segundo pasa da */
        ticks_50ms = 0;
        
        if (betetze_prozesuan) {
            betetze_segunduak++;
        }
        
        if (t_kontrola_martxan) {
            temp_segunduak++;
        }
    }
}

/* =========================================================
 * KANPO ETENDURA 0 (INT0) - Mantentze Lanak
 * ========================================================= */
void manten_ISR(void) interrupt 0 {
    /* '0' detektatu da (falling edge). Mantentze lanetan sartu gara */
    mantentzen = 1;
    
    /* Irteera guzti-guztiak desaktibatu */
    P4 = 0x00;
    
    /* "Hautatzailea OFF posiziora bueltatu arte" ("1" logikorako itxaron) 
     * Etenaldian (ISR) bertan blokeatzea onartzen da, 
     * sistemak ez baitu ezer egin behar bitartean. */
    while (MANTEN_BTN == 0) {
        /* Itxaron hemen hautagailua askatu/OFF jarri arte */
    }
    
    /* Mantentze modutik irtenda. Sistema prest hasieratik berriro hasteko */
    mantentzen = 0;
    betetze_prozesuan = 0;
    t_kontrola_martxan = 0;
    ticks_50ms = 0;
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
    /* Portuak */
    P1 = 0xFF; /* Sarrerak bezala */
    P3 |= 0x04; /* P3.2 sarrera */
    P4 = 0x00; /* Irteerak itzalita */
    
    /* Timer 0 Modu 1 */
    TMOD &= 0xF0;
    TMOD |= 0x01;
    TH0 = T0_TH;
    TL0 = T0_TL;
    
    /* Etendurak: INT0 = Falling edge */
    IT0 = 1;
    
    ET0 = 1; /* Timer 0 etendura */
    EX0 = 1; /* INT0 etendura */
    EA  = 1; /* Globala */
    
    TR0 = 1; /* Hasi tenporizadorea */
    
    mantentzen = 0;
    betetze_prozesuan = 0;
    t_kontrola_martxan = 0;
}

/* =========================================================
 * FUNTZIO NAGUSIA
 * ========================================================= */
void main(void) {
    unsigned int adc_balioa;
    float tenperatura;
    
    hasieratu();
    
    while(1) {
        if (mantentzen) continue; /* ISRak blokeatzen badu ez da hona sekula iritsiko baina badaezpada */

        /* --- 1. MARTXA SISTEMA --- */
        if (!MARTXA_BTN) {
            /* Martxa kentzean edozein unean, prest hasieratik hasteko */
            P4 = 0x00;
            betetze_prozesuan = 0;
            t_kontrola_martxan = 0;
            continue; /* Ezer gehiago ez egin */
        }
        
        LED_M = 1; /* Martxan gaude */
        
        /* --- 2. GUTXIENEKO UR-MAILAREN KUDEAKETA --- */
        if (S1_SENTSORE == 0 && !betetze_prozesuan) {
            /* S1 0ra jartzen denean, betetze prozesua errepikatu:
               - Hustutze balbula itxi
               - Ur ponpa piztu 10s edo S2==1 arte */
            betetze_prozesuan = 1;
            HUSTUTZE_BALB = 0;
            UR_PONPA = 1;
            betetze_segunduak = 0;
            ticks_50ms = 0; 
            
            /* T_kontrola bertan behera utzi betetze prozesuan */
            t_kontrola_martxan = 0;
            LED_T = 0;
            T_SEINALEA = 0;
        }
        
        if (betetze_prozesuan) {
            /* Betetzen ari da. Bi baldintzak ebaluatu */
            if (betetze_segunduak >= 10 || S2_SENTSORE == 1) {
                /* Depositua bete da */
                UR_PONPA = 0;
                /* Hustutze balbula ireki ur eskariari aurre egiteko */
                HUSTUTZE_BALB = 1; 
                betetze_prozesuan = 0; /* Prozesua amaitu */
                
                /* Betetze prozesua amaitutakoan, 
                   eta T_KONTROL_BTN aurretik aktibatuta bazegoen,
                   jarraian tenperatura kontrola lanean hasiko da */
            }
        }
        
        /* --- 3. URAREN TENPERATURA KONTROLA --- */
        /* Ezin da T kontrolatu betetze prozesuan */
        if (T_KONTROL_BTN == 1 && !betetze_prozesuan) {
            /* T kontrola aktibatu */
            t_kontrola_martxan = 1;
            LED_T = 1;
            
            /* 10 segunduro tenperatura neurtu behar da */
            if (temp_segunduak >= 10 || temp_segunduak == 0) { /* HASIERATZEKO BEREHALA NEURTU */
                adc_balioa = ADBihurketa(5); /* 5. kanala */
                tenperatura = (130.0f / 1023.0f) * adc_balioa - 30.0f;
                
                if (tenperatura <= 30.0f) {
                    T_SEINALEA = 1; /* Ura berotu */
                } else {
                    T_SEINALEA = 0; /* Ez berotu */
                }
                
                temp_segunduak = 1; /* 0tik gora pasa sarrera baldintza ez betetzeko etengabe */
            }
        } else {
            /* Tenperatura kontrola deskonektatuta badago edo betetzen ari garelako */
            t_kontrola_martxan = 0;
            LED_T = 0;
            T_SEINALEA = 0;
            temp_segunduak = 0;
        }
    }
}
