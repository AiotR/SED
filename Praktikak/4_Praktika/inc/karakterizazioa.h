/*===========================================================
 * karakterizazioa.h — SETxAD multzoaren karakterizazioa
 * 4. Praktika: K.Z. motor baten posizio-kontrola
 *
 * ADKodea → θ[rad] bihurtzeko erregresio linealaren koefizienteak
 * eta funtzio prototipoa.
 *===========================================================*/

#ifndef KARAKTERIZAZIOA_H
#define KARAKTERIZAZIOA_H

/* =========================================================
 * Erregresio linealaren koefizienteak
 * Laborategian neurtutako datuetan oinarritu:
 *   θ[rad] = M_AD * ADKodea + OFFSET_AD
 *
 * OHARRA: Balio hauek LABORATEGIAN neurtu behar dira!
 *         Hemen placeholder balioak dira.
 *         Errealak MATLAB script-arekin kalkulatu.
 * ========================================================= */
#define M_AD      0.006136f   /* [rad/kode] — placeholder */
#define OFFSET_AD (-3.14159f) /* [rad]      — placeholder */

/* =========================================================
 * ADKodeaThetara: ADCaren kodea angeluara bihurtu
 *
 *   Parametroa:  ADKodea (0..1023)
 *   Itzulera:    theta [rad] — motorraren posizioa
 * ========================================================= */
float ADKodeaThetara(unsigned int ADKodea);

#endif /* KARAKTERIZAZIOA_H */
