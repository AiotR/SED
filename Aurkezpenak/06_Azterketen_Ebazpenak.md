---
marp: true
theme: default
paginate: true
---

# AZTERKETAK: Kasu Praktikoen Ebazpena
**Konektibitate konplexua eta Segurtasun ereduak**

---

## 1. Nahasketa Sistema (2022)
- **Erronka:** Tankeen kontrol ziklikoa osagai likido/solidoekin.
- **Irtenbidea:** *State-Machine* (Egoera-Makina) purua. Ziklo bakoitza Timer ISR-an erregistratu da, sentsore batzuen konparaketa bidez. Prozesu berehalakoan INT0 sartu zen "STOP Larrialdia" modura (motorrak unean moztuz).

---

## 3. NFC Hotel Sarraila (2025)
- **Erronka:** Pasahitz edo "Identifikadore (ID)" logikaren bitartez ate bat zaintzea, bestelako segurtasun baldintzekin uztartuz.
- **Irtenbidea:** Sarreretan "polling" bidez NFC irakurketa bat simulatzea. Baliozcota bada 5 segundoz ireki sarraila (Timer Mode 2 bitartez). Alarma sute asinkronoa bitartean P5.1 sentsorez irakurtzeko etendura-mailaren (Priority) jokoa prestatu behar izan zen.

---

## 5. GIE2 Eiekzio Sistema (2024)
- **Erronka:** Segurtasun kritikoa duen hegazkin-sistema sinkronizatzea. Akuts zehatza egitea hiltzekoa litzateke.
- **Irtenbidea:** Sekuentzia oso espezifikoa (Arnesa -> 5s -> Motorea -> 3s -> ...). Tenporizadore ISR baten barruan, ataza-kudeatzaile bat egituratu genuen. Polling ADC sentsorea erabili genuen `arrisku-mailaren` arabera ekintzak behartzeko.

---

## 7. Izotza eta Semaforoa (2019)
- **Erronka:** Bi prozesu paralelo bateratzea. Bata 100% ziklikoa (Semaforoa), bestea pultsu / alarma bat.
- **Irtenbidea:** P4 pinean oinezkoen pultsadoreari "debounce" bitala asmatu zitzaion (prozesua errepikatu ez dadin pertsona botoian luzaro dagoelako). ADC tenperatura etengabe kontrolatu zen 10s-eko abisuk erakusteko eragozpenik gabe.

---

## 8. Su Artifizialen Palmondoa (2020)
- **Erronka:** Kontagailu (Counter) fisiko / software bakar bat prozesu guztiarentzat asmatzea "Funtzionamendu Aurreratua" helburu.
- **Irtenbidea:** Berrabiarazten ez den Timer aldagai oso azkar bat erabili `if (denbora == 5000)` edo `if (denbora == 12000)` arkitekturarekin suzirien jaurtiketa zuzendaria osatzeko akats barik.

---

## ONDORIO Orokorra Azterketetan
- Azterketetako baldintza nagusia **Konkurrentzia** simulatzea da gailu mono-nuklear batean (ISR eta Polling-loop menderatuz).
- Banderak (`prozesua_martxan = 1`, `alarma = 0`) ezinbesteko "mintzaira" dira etendura eta zeregin nagusiaren artean elkar ulertzeko, egitura apurtu gabe.
