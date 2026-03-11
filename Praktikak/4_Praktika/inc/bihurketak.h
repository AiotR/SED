/*===========================================================
 * bihurketak.h — A/D eta D/A bihurketa funtzioen liburutegia
 * 4. Praktika: K.Z. motor baten posizio-kontrola
 * 80C552 Mikrokontrolagailua - Keil C51
 *
 * Funtzio nagusiak:
 *   - ADBihurketa(): ADCaren irakurketa 10 bitetan
 *   - DABihurketa(): DACari 12 biteko kodea bidali
 *===========================================================*/

#ifndef BIHURKETAK_H
#define BIHURKETAK_H

#include <reg552.h>

/* =========================================================
 * ADBihurketa: Sarrera analogikoa digitalizatu
 *
 *   Parametroa:  kanala (0..7) — irakurri nahi den ADC kanala
 *   Itzulera:    unsigned int (0..1023) — 10 biteko kode digitala
 *
 *   80C552 ADC erregistroak:
 *     ADCON (0xDE): ADCaren kontrol erregistroa
 *       - ADCON[2:0]: Kanal hautaketa (MUX)
 *       - ADCON[3]  : ADCS — Bihurketa hasteko bita (1=Hasi)
 *       - ADCON[4]  : ADCI — Bihurketa amaieraren bandera (1=Bete)
 *     ADCH  (0xC5): Emaitzaren goi-bytea (bit 9..2)
 *                   + ADCON biteko bi azpikoak (bit 1..0)
 * ========================================================= */
unsigned int ADBihurketa(unsigned char kanala);

/* =========================================================
 * DABihurketa: Kontrol seinalea D/A bihurgailura bidali
 *
 *   Parametroa: uk — kontrol seinalearen tentsioa (float, -10..+10 V)
 *
 *   D/A bihurgailua: 12 biteko, -10V (kode=0) eta +10V (kode=4095)
 *
 *   Bit banaketa:
 *     P3[5:2] = B11..B8  (4 bit garrantzitsuenak)
 *     P4[7:0] = B7..B0   (8 bit gutxigarrantzitsuenak)
 *
 *   KONTUZ: P3.7, P3.6, P3.1, P3.0 ez aldatu!
 *           µK-ak beste atazak ditu pin horietan.
 * ========================================================= */
void DABihurketa(float uk);

#endif /* BIHURKETAK_H */
