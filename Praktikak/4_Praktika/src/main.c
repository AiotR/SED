/*===========================================================
 * main.c — K.Z. Motor Baten Posizio-Kontrola
 * 4. Praktika: Sistema Elektroniko Digitalak
 * 80C552 Mikrokontrolagailua - Keil C51
 *
 * SARRERA:
 * K.Z. (Korronte Zuzeneko) motor baten ardatzaren posizioa
 * kontrolatzeko aurreratze-konpentsadore digitala inplementatzen da.
 *
 * Diferentzia-ekuazioa (Kontrol legea):
 *   uk = A0*u(k-1) + A1*e(k) + A2*e(k-1)
 *
 * Non:
 *   A0 = (2 - a*h) / (2 + a*h)
 *   A1 = Kc * (2 + b*h) / (2 + a*h)
 *   A2 = Kc * (b*h - 2) / (2 + a*h)
 *
 * Parametroak:
 *   h  = 0.022  s   (laginketa-periodoa)
 *   a  = 20     s^-1
 *   b  = 16.949152542372865  s^-1
 *   Kc = -9.311534963543551  V/rad
 *===========================================================*/

#include <reg552.h>
#include "bihurketak.h"
#include "karakterizazioa.h"

/* ===========================================================
 * KONTROL PARAMETROAK (ezagunak, kalkulatuak)
 * =========================================================== */
#define H_LAGINKATZE  0.022f                    /* Laginketa-periodoa [s] */
#define A_PARAM       20.0f                     /* [s^-1] */
#define B_PARAM       16.949152542372865f       /* [s^-1] */
#define KC_PARAM      (-9.311534963543551f)     /* [V/rad] */

/* Aurrez kalkulatutako koefizienteak (konpilazioan kalkulatu, eraginkortasuna hobetzeko) */
#define DENOM         (2.0f + A_PARAM * H_LAGINKATZE)   /* 2 + a*h = 2.44 */
#define A0_KOEF       ((2.0f - A_PARAM * H_LAGINKATZE) / DENOM)  /* ~0.639 */
#define A1_KOEF       (KC_PARAM * (2.0f + B_PARAM * H_LAGINKATZE) / DENOM)
#define A2_KOEF       (KC_PARAM * (B_PARAM * H_LAGINKATZE - 2.0f) / DENOM)

/* u[k] mugen balioak: -8 V ... +8 V */
#define U_MAX  8.0f
#define U_MIN (-8.0f)

/* Timer 0 konfiguraketa: 22ms-ko etendura @ 12MHz
 * Timer 1 modua: 16 biteko tenporizadore.
 * Zikloak: 22ms * (12MHz / 12) = 22000 ziklo
 * Hasierapen balioa: 65536 - 22000 = 43536 = 0xAA10 */
#define TIMER0_TH  0xAA
#define TIMER0_TL  0x10

/* ===========================================================
 * ALDAGAI GLOBALAK
 * Bakarra global moduan deklaratzen dira (ISR eta main
 * artean partekatzen direlako bakarrik).
 * =========================================================== */
static bit  kontrolatu_flag = 0; /* Timer ISR-ak 1 jartzen du */

/* Aurreko pausoko balioak (diferentzia-ekuazioak beharrezkoak) */
static float uk_prev = 0.0f;    /* u(k-1): aurreko kontrol seinalea */
static float ek_prev = 0.0f;    /* e(k-1): aurreko errorea */

/* Erreferentzia profila inplementatzeko taula (2. bertsioa)
 * Balio bakoitza radianarretan, 22ms-ko pausoetan
 * Taula Flash memorian gordetzeko 'code' gako-hitza erabili */
float code erreferentzia_taula[] = {
    0.0f,  0.0f,  0.0f,  0.0f,  0.0f,   /* 0.0 rad (0-110ms) */
    1.57f, 1.57f, 1.57f, 1.57f, 1.57f,  /* pi/2 rad (110-220ms) */
    1.57f, 1.57f, 1.57f, 1.57f, 1.57f,  /* pi/2 mantendu */
    3.14f, 3.14f, 3.14f, 3.14f, 3.14f,  /* pi rad */
    3.14f, 3.14f, 3.14f, 3.14f, 3.14f,  /* pi mantendu */
    1.57f, 1.57f, 1.57f, 1.57f, 1.57f,  /* pi/2 itzuli */
    0.0f,  0.0f,  0.0f,  0.0f,  0.0f    /* 0.0 rad itzuli */
};

#define TAULA_LUZERA 35  /* erreferentzia_taula[] elementu kopurua */

/* ===========================================================
 * HASIERAKETAKO FUNTZIOA
 * Timer0 eta Etendura Sistema konfiguratzen ditu.
 * =========================================================== */
void hasieratu(void) {
    /* ----- Timer 0 konfigurazioa: Modu 1 (16 bit) ----- */
    TMOD &= 0xF0;   /* Timer0 biteak garbitu (goi nibbleak Timer1-a dira) */
    TMOD |= 0x01;   /* Timer0 Modu 1: 16-bit tenporizadore */

    /* 22ms-ko overflow: TH0:TL0 = 0xAA10 */
    TH0 = TIMER0_TH;
    TL0 = TIMER0_TL;

    /* ----- Etendura Sistema gaitu ----- */
    ET0 = 1;  /* Timer 0 etendura gaitu */
    EA  = 1;  /* Etendura orokorrak gaitu */

    /* ----- Timer 0 abiarazi ----- */
    TR0 = 1;

    /* ----- Aldagai-hasierapenak ----- */
    uk_prev = 0.0f;
    ek_prev = 0.0f;
}

/* ===========================================================
 * TIMER 0 ETENDURA FUNTZIOA (ISR)
 * 22ms-ro deitzen da automatikoki.
 *
 * 'interrupt 1' = Timer 0 overflow
 * 'using 1'     = 1. erregistro bankua erabili (ataza eskatu duena)
 * =========================================================== */
void timer0_ISR(void) interrupt 1 using 1 {
    /* Tenporizadorea berrekarri hurrengo 22ms-rako */
    TH0 = TIMER0_TH;
    TL0 = TIMER0_TL;

    /* Kontrol-bandera 1 jarri: main() begiztak kontrolatu */
    kontrolatu_flag = 1;
}

/* ===========================================================
 * KONTROL ALGORITHM FUNTZIOA
 * Diferentzia-ekuazioa:  uk = A0*u(k-1) + A1*ek + A2*e(k-1)
 * =========================================================== */
float kalkulatu_uk(float theta_ref, float theta_k) {
    float ek, uk;

    /* Errorea kalkulatu */
    ek = theta_ref - theta_k;

    /* Kontrol legea aplikatu */
    uk = A0_KOEF * uk_prev + A1_KOEF * ek + A2_KOEF * ek_prev;

    /* Saturazio mugak aplikatu: -8V .. +8V */
    if (uk >  U_MAX) uk =  U_MAX;
    if (uk <  U_MIN) uk =  U_MIN;

    /* Aurreko balioak eguneratu hurrengo iteraziorako */
    ek_prev = ek;
    uk_prev = uk;

    return uk;
}

/* ===========================================================
 * MAIN
 * =========================================================== */
void main(void) {
    unsigned int ADkodea;
    float theta_k;
    float uk;
    unsigned char taula_indizea = 0;

    /* Sistema hasieratu */
    hasieratu();

    /* Begizta nagusia */
    while (1) {
        /* Timer ISR flag zain egon */
        if (kontrolatu_flag == 1) {
            kontrolatu_flag = 0; /* Bandera garbitu */

            /* ------ 1. URRATSA: Posizioa irakurri (ADC 0. kanala) ------ */
            ADkodea = ADBihurketa(0);

            /* ------ 2. URRATSA: ADKodea → θ[rad] bihurtu ------ */
            theta_k = ADKodeaThetara(ADkodea);

            /* ------ 3. URRATSA: Erreferentzia lortu ------
             * 1. Bertsioa: Erreferentzia konstantea (pi/4 rad = 45 gradu) */
            /* float theta_ref = 0.785f; */  /* Aktibatzeko komentario kendu */

            /* 2. Bertsioa: Erreferentzia aldakorra (taula) */
            uk = kalkulatu_uk(erreferentzia_taula[taula_indizea], theta_k);

            /* Taula indizea eguneratu (bueltaka) */
            taula_indizea++;
            if (taula_indizea >= TAULA_LUZERA) {
                taula_indizea = 0;
            }

            /* ------ 4. URRATSA: Kontrol seinalea D/A bihurgailura bidali ------ */
            DABihurketa(uk);
        }
    }
}
