---
marp: true
theme: default
paginate: true
---

# PRAKTIKAK: Ebazpenen Bilduma
**Sistema Elektroniko Digitalak (80C552)**
Oinarrizko Kontzeptuetatik Arkitektura Aurreratura

---

## 1. Praktika: C51 Oinarriak eta μVision
- **Helburua:** IDE-aren menperatzea eta C lengoaiako eragiketak (matematika, bit-maila).
- **Gakoak:**
  - $16-bit$ aldagaiekin esangarritasun transformazioak egiteko egiturak (Shift-ak, maskarak).
  - Software simulagailuaren erabilera (Watch leihoak, exekuziozko pausoak).
  - Puntu Bikoitzeko zehaztasun gabezia ulertzea arkitektura txiki honetan.

---

## 2. Praktika: Erantzun Sinkronoak (Itxarote Aktiboa vs ISR)
- **Helburua:** Kontrol bidea hautatzea LED matrizekin edo Pantaila logikoekin jolastuz.
- **Gakoak:**
  - *Polling* teknika erabiltzearen kostua CPU-arentzat itxarote akitboetan (`Delay()` funtzioak harrapaturik).
  - Lehen hurbilpena etendura batera (Sarrera aldaketak automatikoki detektatzeko aipamena).
  - Sistema asinkronoen zama zergatik ekidin behar den ulertzea.

---

## 3. Praktika: Timer 0 eta 1 (Milisegunduen Dantza)
- **Helburua:** Osziladorea softwarearen bihotz bihurtzea etendura (ISR) bidez.
- **Gakoak:**
  - Modu-hautaketa `TMOD`-ren bidez.
  - Timer baten Auto-Reload balioaren ekuazioa kalkulagailuan $f_{osc} / 12$ zatituz.
  - Aldagai laguntzaile orokorrak `main` blokeari berri emateko. Sistema "tick" gisa neurtzen ikastea.

---

## 4. Praktika: Karakterizazioa eta ADC/DAC
- **Helburua:** Kontrol Legea (Ekuazio diferentzialen erabilera)
- **Gakoak:**
  - Sentsorearen datu kurbatua lineala nola bihurtu software bidez (Kalibrazioa / Look-up table).
  - Seinale Analogiko irakurketa `ADCON` eta `ADCH` manipulazioaz sekuentzialki.
  - DAC (Bihurgailu Digital-Analogikoa). Gure kasuan sarritan PWM irteera batekin asimilatzen da edo seinale digital baten batezbestekoaz.

---

## ONDORIO Orokorrak Praktiketan
- Ez daude soluzio bakarrak diseinuan (Guztiak Timer bidez ISR barruan ebatzi daitezke ala gertaerak `main`-en jokatzen utzi).
- "Zatikatu eta Garaitu" (Divide and Conquer) inplementazio zuzenena da kodea fitxategi/funtziotan banatuz (`karakterizazioa.c`, `bihurketak.c`).
- Proiektua askoz egonkorragoa izaten da `delay` atzerapenak erabat baztertzen direnean.
