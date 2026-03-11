/*===========================================================
 * bihurketak.c — A/D eta D/A bihurketa funtzioen inplementazioa
 * 4. Praktika: K.Z. motor baten posizio-kontrola
 * 80C552 Mikrokontrolagailua - Keil C51
 *===========================================================*/

#include "bihurketak.h"

/* =========================================================
 * 1. ATAZA: ADBihurketa — ADC irakurketa
 *
 * 80C552 ADC prozesua:
 *   1. ADCON erregistroan kanala hautatu (bit 2:0)
 *   2. ADCS bita 1 jarri bihurketa abiarazteko (bit 3)
 *   3. ADCI banderaren zain egon (bit 4) → 1 denean bete da
 *   4. Emaitza irakurri: ADCH[7:0] = bit 9..2
 *                        ADCON[1:0] = bit 1..0 (2 LSB)
 * ========================================================= */
unsigned int ADBihurketa(unsigned char kanala) {
    unsigned int emaitza;

    /* Baldintza: kanala 0-7 tartean egon behar da */
    if (kanala > 7) kanala = 0;

    /* 1. Urratsa: Kanal hautaketa + bihurketa hastea
     *    ADCON = 0b00001xxx  (xxx = kanal zenbakia)
     *    Bit 3 (ADCS) = 1 → bihurketa abiarazi */
    ADCON = (unsigned char)(0x08 | kanala);

    /* 2. Urratsa: ADCI banderaren zain (bit 4 = 1 bihurtu arte)
     *    Bihurketa ~40 ziklo irauten du @ 12MHz */
    while (!(ADCON & 0x10)) {
        /* Itxaroten... ADCI=0 bitartean begiztan jarraituko du */
    }

    /* 3. Urratsa: Emaitza irakurri
     *    ADCH = [B9 B8 B7 B6 B5 B4 B3 B2]   (bit garantzitsuenak)
     *    ADCON[1:0] = [B1 B0]                 (2 bit gutxigarrantzitsuenak)
     *
     *    10 biteko balioa osatzeko:
     *    emaitza = (ADCH << 2) | (ADCON & 0x03) */
    emaitza = ((unsigned int)ADCH << 2) | (ADCON & 0x03);

    return emaitza; /* 0..1023 tartean */
}

/* =========================================================
 * 3. ATAZA: DABihurketa — D/A bihurgailura idatzi
 *
 * Portaera lineala:
 *   uk = -10V → kode = 0    (0x000)
 *   uk = +10V → kode = 4095 (0xFFF)
 *   kode = (uk + 10) / 20 * 4095
 *
 * 12 biteko banaketa:
 *   P3[5:2] ← B11..B8   (4 MSB)  — P3 gainerakoak UKITU GABE
 *   P4[7:0] ← B7..B0    (8 LSB)  — P4 osoa
 * ========================================================= */
void DABihurketa(float uk) {
    unsigned int kode12;
    unsigned char behe8, goi4;
    unsigned char p3_balio;

    /* uk -10..+10 V tartean mugatu (saturazioa) */
    if (uk >  10.0f) uk =  10.0f;
    if (uk < -10.0f) uk = -10.0f;

    /* 12 biteko kode kalkulatu
     * kode = (uk + 10.0) / 20.0 * 4095.0 */
    kode12 = (unsigned int)((uk + 10.0f) / 20.0f * 4095.0f);

    /* 12 biteko kodea bi zatitan banandu:
     *   B11..B8 → goi 4 bitak (goi4)
     *   B7..B0  → behe 8 bitak (behe8) */
    behe8 = (unsigned char)(kode12 & 0x00FF);        /* Behe 8 bit */
    goi4  = (unsigned char)((kode12 >> 8) & 0x0F);   /* Goi 4 bit */

    /* P4 osoa behe8-rekin bete */
    P4 = behe8;

    /* P3ko bit 5..2 soilik aldatu, gainerakoak mantendu:
     *   Maskara:     0b11000011 = 0xC3 → bit 7,6,1,0 UKITU GABE
     *   Aldatzeko:   goi4 << 2  → bit 5..2 posiziora mugitu */
    p3_balio = P3;                       /* P3ren egungo balioa irakurri */
    p3_balio &= 0xC3;                    /* Bit 5..2 garbitu (0xC3 = 1100 0011) */
    p3_balio |= (goi4 << 2);             /* Goi 4 bitak P3[5:2] posiziora sartu */
    P3 = p3_balio;                       /* P3 eguneratu */
}
