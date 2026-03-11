---
marp: true
theme: default
paginate: true
---

# 80C552 Mikrokontrolagailua: Sarrera eta Oinarriak
**Sistema Elektroniko Digitalak (SED)**
Egilea: [Zure Izena]

---

## 1. Zer da 80C552?
- **Familia:** 8051 arkitekturaren oinordekoa (Philips / NXP konpainiak garatua).
- **Zortzi biteko (8-bit)** mikrokontrolagailua.
- Industria eta hezkuntza arkitektura klasikoetako bat da.
- Erraz ikasteko instruzkio-jokoa du, baina ahalmen sendoak aplikazio errealetarako.

---

## 2. Zergatik 80C552 eta ez 8051 arrunta?
- 8051 estandarretik haratago doazen hardware ezaugarriak ditu:
  - **A/D Bihurgailua (10-bit)** sentsore analogikoak irakurtzeko.
  - **PWM (Pulse Width Modulation)** seinale garbiak sortzeko.
  - **I2C autobusa** gailuekin komunikatzeko.
  - **I/O ataka gehigarriak** (P4 eta P5, guztira askoz pin gehiago sarrera/irteeretarako).

---

## 3. Garapen Ingurunea: Keil μVision
- **C lengoaiako programazioa:** Mihiztadura (Assembly) alde batera utzi eta C erabiliz programatzen da (`C51` konpiladorea).
- **Abantailak:**
  - `reg552.h` liburutegiak erregistro guztiak zuzenean mapatzen ditu C lengoaian.
  - Hardwarearen kontrol zehatza (sbit adierazpenekin).
  - Arazketa eta simulazioa software beretik egiteko gaitasuna.

---

## 4. Hardwarearen Bihotza: Osziladorea
- **Maiztasuna:** Datu teknikoen arabera 12 MHz, 11.0592 MHz, edo 16 MHz-eko kuartzo kristala izaten du.
- **Ziklo-makina (Machine Cycle):** 
  - 8051 estandarrean instrukzio batek 12 erloju-ziklo behar ditu.
  - Adibidea: $f_{osc} = 12 \text{ MHz}$ bada -> Ziklo bat = $1 \text{ }\mu\text{s}$.
  - Adibidea: $f_{osc} = 11.0592 \text{ MHz}$ bada -> Ziklo bat ≈ $1.085 \text{ }\mu\text{s}$.

---

## 5. C51 Arkitekturaren Memoria-Eredua
- Mikrokontrolagailu honek memoria ezberdinak fisikoki banatuta ditu (Harvard Arkitetkura):
  - **Program Memory (ROM):** Kodea gordetzeko (Flash ikuspegian).  `code` espazioa. C51en gako hitz honekin taulak edo konstanteak defini daitezke memoria irakurri-bakarrean gordetzeko.
  - **Data Memory (RAM):** Aldagaiak eta hardware erregistroak irakurri/idazteko prozesatzeko unean. Berau oso txikia da.

---

## ONDORIOAK
- 80C552 oinarri sendoa da mikrokontrolagailuen munduan murgiltzeko.
- Proiektuen logika `main` diseinu batera eta Etenduren (Interrupts) diseinu batera egokitu behar da espazio txiroa eraginkortasunez aprobetxatzeko.
