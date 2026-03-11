---
marp: true
theme: default
paginate: true
---

# AZTERKETAK: Kasu Praktikoen Ebazpena
**Konektibitate konplexua eta Segurtasun ereduak**

> **Irudiaren Prompta (Google Nano):** A tense examination setting with a student focusing intently on programming a microcontroller, dramatic spotlight, engineering challenge.

---

## 1. Nahasketa Sistema (2022)
- **Erronka:** Tankeen kontrol ziklikoa osagai likido/solidoekin.
- **Irtenbidea:** *State-Machine* (Egoera-Makina) purua. Ziklo bakoitza Timer ISR-an erregistratu da, sentsore batzuen konparaketa bidez. Prozesu berehalakoan INT0 sartu zen "STOP Larrialdia" modura (motorrak unean moztuz).

> **Irudiaren Prompta (Google Nano):** A futuristic automated liquid mixing tank in a state-of-the-art intelligent manufacturing plant, glowing indicators.

---

## 3. NFC Hotel Sarraila (2025)
- **Erronka:** Pasahitz edo "Identifikadore (ID)" logikaren bitartez ate bat zaintzea, bestelako segurtasun baldintzekin uztartuz.
- **Irtenbidea:** Sarreretan "polling" bidez NFC irakurketa bat simulatzea. Baliozcota bada 5 segundoz ireki sarraila (Timer Mode 2 bitartez). Alarma sute asinkronoa bitartean P5.1 sentsorez irakurtzeko etendura-mailaren (Priority) jokoa prestatu behar izan zen.

> **Irudiaren Prompta (Google Nano):** A glowing NFC card being held up to a sleek, modern electronic hotel door lock, emitting light blue wireless connection waves.

---

## 5. GIE2 Eiekzio Sistema (2024)
- **Erronka:** Segurtasun kritikoa duen hegazkin-sistema sinkronizatzea. Akuts zehatza egitea hiltzekoa litzateke.
- **Irtenbidea:** Sekuentzia oso espezifikoa (Arnesa -> 5s -> Motorea -> 3s -> ...). Tenporizadore ISR baten barruan, ataza-kudeatzaile bat egituratu genuen. Polling ADC sentsorea erabili genuen `arrisku-mailaren` arabera ekintzak behartzeko.

> **Irudiaren Prompta (Google Nano):** An abstract technological sequence showing an airplane ejection seat mechanism with glowing digital timers counting down safely, high tech HUD.

---

## 7. Izotza eta Semaforoa (2019)
- **Erronka:** Bi prozesu paralelo bateratzea. Bata 100% ziklikoa (Semaforoa), bestea pultsu / alarma bat.
- **Irtenbidea:** P4 pinean oinezkoen pultsadoreari "debounce" bitala asmatu zitzaion (prozesua errepikatu ez dadin pertsona botoian luzaro dagoelako). ADC tenperatura etengabe kontrolatu zen 10s-eko abisuk erakusteko eragozpenik gabe.

> **Irudiaren Prompta (Google Nano):** A smart city traffic light covered in light frost with a glowing digital ice warning sign, winter city night background.

---

## 8. Su Artifizialen Palmondoa (2020)
- **Erronka:** Kontagailu (Counter) fisiko / software bakar bat prozesu guztiarentzat asmatzea "Funtzionamendu Aurreratua" helburu.
- **Irtenbidea:** Berrabiarazten ez den Timer aldagai oso azkar bat erabili `if (denbora == 5000)` edo `if (denbora == 12000)` arkitekturarekin suzirien jaurtiketa zuzendaria osatzeko akats barik.

> **Irudiaren Prompta (Google Nano):** Beautiful glowing fireworks shaped like a palm tree lighting up the night sky, with a digital counter overlaid on the sky, festive and technical.

---

## ONDORIO Orokorra Azterketetan
- Azterketetako baldintza nagusia **Konkurrentzia** simulatzea da gailu mono-nuklear batean (ISR eta Polling-loop menderatuz).
- Banderak (`prozesua_martxan = 1`, `alarma = 0`) ezinbesteko "mintzaira" dira etendura eta zeregin nagusiaren artean elkar ulertzeko, egitura apurtu gabe.

> **Irudiaren Prompta (Google Nano):** A conductor directing an orchestra composed of robotic arms and electronics, representing concurrency and smooth multitasking in a single-core system.
