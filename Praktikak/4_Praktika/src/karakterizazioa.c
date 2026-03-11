/*===========================================================
 * karakterizazioa.c — SETxAD erregresio linealaren inplementazioa
 * 4. Praktika: K.Z. motor baten posizio-kontrola
 *===========================================================*/

#include "karakterizazioa.h"

/* =========================================================
 * 2. ATAZA: ADKodeaThetara
 *
 * Sentsorea + Egokitze Txartela + ADC (SETxAD) multzoaren
 * karakterizazioan lortutako erregresio linealaren ekuazioa:
 *
 *   θ[rad] = M_AD * ADKodea + OFFSET_AD
 *
 * OHARRA: M_AD eta OFFSET_AD .h fitxategian definituak daude.
 *         Laborategian neurtu behar dira MATLAB script-arekin!
 * ========================================================= */
float ADKodeaThetara(unsigned int ADKodea) {
    return (M_AD * (float)ADKodea) + OFFSET_AD;
}
