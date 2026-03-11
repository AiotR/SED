/*===========================================================
 * 3. AZTERKETA: HOTELAREN SARRAILA NFC (2025-06-10)
 * 80C552 Mikrokontrolagailua - Keil C51
 *
 * SARRERA:
 * Landa-hotel batean NFC sarrailaren simulazioa.
 * Gidariak:
 *  - P1.6: NFC Irakurri (Goi-saihets) -> Polling bidezkoa
 *          (ezin da kanpo-etendura 1 eduki, 'falling edge' direlako)
 *  - P1.2, P1.1, P1.0: ID-a
 *  - P1.7: Reset (Goi mailan aktiba)
 *  - P5.1: ADC (Su-sentsorea, sugar maila = 10*k/1023)
 *  - P3.3: INT1 (Su detektatu digitala -> Falling Edge)
 *  - P3.4: Aspertsoreak
 *  - P4: Kontrol irteerak (Aire, Intso., Audio, Errotulu, Gorria, Berdea, Argiak, Atea).
 *===========================================================*/

#include <reg552.h>

/* --- Pin definizioak bektorean --- */
sbit IRAKURRI_AGINDUA = P1^6;
sbit RESET_BTN        = P1^7;

/* Irteerak P4 */
sbit P4_AIRE_GIROTUA    = P4^0;
sbit P4_INTSONORIZAZIOA = P4^1;
sbit P4_AUDIOBISUALAK   = P4^2;
sbit P4_ERROTULUA       = P4^3;
sbit P4_LED_GORRIA      = P4^4;
sbit P4_LED_BERDEA      = P4^5;
sbit P4_ARGIAK          = P4^6;
sbit P4_ATEA            = P4^7;

/* Sutealarma */
sbit P3_ASPERTSOREAK  = P3^4;

/* --- Egoerak eta Aldagaiak --- */
static bit sute_egoera = 0;
static bit onartu_flag = 0;
static bit ukatu_flag = 0;

/* Denborizazioko aldagaiak */
static unsigned int timer_tick = 0;     /* 5ms-ro gehituko da */
static unsigned int ukatu_keinu_ms = 0; /* LED gorria keinuka */

/* --- T0 Timer: Modu 2 (8-bit Auto-Reload) ---
 * 12 MHz = 1us ziklo.
 * 250 tick = 250us irauten du etendura bakoitzak.
 * Nahi duguna = 5ms lortzeko aldagaia kontatu behar da.
 * Baina etendura Timer0-k 250us egiten badu -> 5ms dira 20 aldiz!
 * Reload balioa = 256 - 250 = 6 = 0x06
 */
#define T0_RELOAD 0x06

/* =========================================================
 * TIMER 0 ISR - 250us etendurak
 * using 1 (Azterketak eskatzen du)
 * ========================================================= */
void timer0_ISR(void) interrupt 1 using 1 {
    timer_tick++;
    
    /* Ukatu zikloa aktibo badago (LED gorria keinuka 3.5seg, 0.5s frekuentzia)
     * 500ms = 2000 ticks of 250us */
    if (ukatu_flag) {
        ukatu_keinu_ms++;
        if (ukatu_keinu_ms >= 14000) { /* 3.5s -> 14000 ticks */
            ukatu_flag = 0;
            P4_LED_GORRIA = 0;
        } else if ((ukatu_keinu_ms % 2000) == 0) {
            P4_LED_GORRIA = ~P4_LED_GORRIA; /* 500ms toggle */
        }
    }
}

/* =========================================================
 * KANPO ETENDURA 1 (Sute digitala) - P3.3
 * ========================================================= */
void sute_ISR(void) interrupt 2 {
    /* Lehentasuna emanda hasieraketan (PX1 = 1) */
    sute_egoera = 1;
    
    /* Irteera guztiak itzali (atea izan ezik) */
    P4 = 0x80; /* Atea (P4.7) desblokeatuta 1, besteak 0 */
}

/* =========================================================
 * A/D BIHURKETA
 * ========================================================= */
unsigned int ADBihurketa(unsigned char kanala) {
    ADCON = (0x08 | (kanala & 0x07));
    while (!(ADCON & 0x10));
    return ((unsigned int)ADCH << 2) | (ADCON & 0x03);
}

void sentsore_sutea(void) {
    if (sute_egoera) {
        float sugar_maila = (10.0f * (float)ADBihurketa(1)) / 1023.0f;
        if (sugar_maila >= 2.0f) {
            /* 1 segunduko keinua: 4000 ticks (250us) */
            if ((timer_tick % 4000) == 0) {
                P3_ASPERTSOREAK = ~P3_ASPERTSOREAK;
            }
        } else {
            P3_ASPERTSOREAK = 0;
        }
    }
}

/* =========================================================
 * HASIERAKETA
 * ========================================================= */
void hasieratu(void) {
    /* Portuak: P1.x sarrerak dira = 1 bezala idatzi */
    P1 = 0xFF; /* Pull-up on sarrera irakurri aurretik */
    P4 = 0x00;
    P3_ASPERTSOREAK = 0;
    
    /* T0 Mode 2 */
    TMOD &= 0xF0;
    TMOD |= 0x02;
    TH0 = T0_RELOAD;
    TL0 = T0_RELOAD;
    
    /* Etendurak: INT1 Falling edge eta Lehentasuna */
    IT1 = 1; /* P3.3 falling edge */
    PX1 = 1; /* INT1 Lehentasuna maximoa (1) azalpenek eskatuta */
    
    ET0 = 1; /* T0 */
    EX1 = 1; /* INT1 */
    EA  = 1;
    
    TR0 = 1; /* Hasi tenporizagailua */
}

/* =========================================================
 * JATORRIZKO EGOERARA RESET (P1.7)
 * ========================================================= */
void garbitu_sistema(void) {
    P4 = 0x00;
    P3_ASPERTSOREAK = 0;
    sute_egoera = 0;
    onartu_flag = 0;
    ukatu_flag = 0;
}

/* =========================================================
 * BEGIZTA NAGUSIA
 * ========================================================= */
void main(void) {
    unsigned char id_kode = 0;
    unsigned char irakurri_prev = 0;
    
    hasieratu();
    
    while(1) {
        
        /* 1) Hardware bidezko Reset */
        if (RESET_BTN) {
            garbitu_sistema();
        }
        
        /* 2) Sute egoera kudeatu */
        if (sute_egoera) {
            /* Funtzionamendu normala ekidin, sute kudeaketa bakarrik */
            sentsore_sutea();
            continue; /* Berriro hasi begizta */
        }
        
        /* 3) Funtzionamendu normala: Polling bidez irakurriAgindua (P1.6) */
        if (IRAKURRI_AGINDUA && !irakurri_prev && !onartu_flag && !ukatu_flag) {
            /* Goi saihets bat detektatu da! ID-a kudeatu: */
            id_kode = (P1 & 0x07); /* P1.2, P1.1, P1.0 */
            
            /* Kointzidentzia aztertu: 1, 3, 5, edo 7 */
            if (id_kode == 1 || id_kode == 3 || id_kode == 5 || id_kode == 7) {
                /* Onartua */
                P4_LED_BERDEA = 1;
                P4_ATEA = 1;
                P4_ARGIAK = 1;
                
                if (id_kode >= 1) P4_AIRE_GIROTUA = 1;
                if (id_kode >= 3) P4_INTSONORIZAZIOA = 1;
                if (id_kode >= 5) P4_AUDIOBISUALAK = 1;
                if (id_kode == 7) P4_ERROTULUA = 1;
                
                onartu_flag = 1;
                timer_tick = 0; /* Kontagailua berritu */
            } else {
                /* Ukatua */
                P4_LED_GORRIA = 1;
                ukatu_flag = 1;
                ukatu_keinu_ms = 0;
            }
        }
        irakurri_prev = IRAKURRI_AGINDUA;
        
        /* 4) Onartu egoera (5 segundoz piztuta egon behar da) */
        if (onartu_flag) {
            /* 5 seg = 5000ms = 20000 ticks of 250us */
            if (timer_tick >= 20000) {
                onartu_flag = 0;
                /* P4.5 itzali baina logikak gainerakoak aktibo mantendu behar dizkio gela erabiltzeko?
                 * "LED berde bat piztuko da 5 segundoz"
                 */
                P4_LED_BERDEA = 0;
            }
        }
    }
}
