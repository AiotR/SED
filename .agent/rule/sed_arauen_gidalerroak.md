# Sistema Elektroniko Digitalak (SED) - 80552C Proiektua

1. **Hizkuntza:** Erabiltzailearekin laguntzaile gisa aritzean, komunitazioa BETI EUSKARAZ (Basque) egin egingo da. Kodearen iruzkinak, aldagaien izenak (ahal bada) eta azalpenak ere euskaraz idatzi.
2. **Mikrokontrolagailua:** 80552C (8051 arkitektura). Mikrokontrolagailu honen ezaugarriak, zikloak eta hardware mugak errespetatu.
3. **Liburutegiak:** BETI `#include <reg552.h>` erabili hasieran. INOIZ EZ erabili `<reg52.h>`, honek ez baititu 80552C-ren ezaugarriak (P4, ADC, PWM).
4. **Memoriaren Optimizazioa:** 8051 arkitekturak oso datu-memoria (RAM) mugatua du.
   - Ahalik eta datu-mota txikienak erabili (`bit` banderentzat, `unsigned char` kontagailu txikientzat).
   - Aldagai globalak soilik ezinbesteko kasuetan erabili (adibidez, ISR (Etendura) eta `main` artean datuak partekatzeko).
5. **Etendurak (Interrupts):** Etendurak erabiltzean, Keil C51 sintaxia erabili: `void Nire_ISR(void) interrupt X using Y`.
6. **Pinak:** Pin hardwareak definitzeko `sbit` erabili (adibidez: `sbit LED = P4^0;`). Ez berdefinitu P4 edo ADC sarrerak.
7. **Ebazpenak:** Ikusi `Nire ebazpenak/` karpeta erabiltzaileak konpondu nahi dituen ariketak jarraitzeko eta laguntzeko.
