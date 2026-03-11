---
marp: true
theme: default
paginate: true
---

# 80C552 Mikrokontrolagailua: Sarrera eta Oinarriak
**Sistema Elektroniko Digitalak (SED)**
Egilea: [Zure Izena]

> **Irudiaren Prompta (Google Nano):** A high-quality, photorealistic macro shot of a vintage 8051 Philips microcontroller chip on a clean white background, studio lighting, hyper-detailed.

---

## 1. Zer da 80C552?
- **Familia:** 8051 arkitekturaren oinordekoa (Philips / NXP konpainiak garatua).
- **Zortzi biteko (8-bit)** mikrokontrolagailua.
- Industria eta hezkuntza arkitektura klasikoetako bat da.
- Erraz ikasteko instruzkio-jokoa du, baina ahalmen sendoak aplikazio errealetarako.

> **Irudiaren Prompta (Google Nano):** A futuristic digital illustration showing an 8-bit microcontroller glowing with blue neon data lines on a dark background, cyber-physical aesthetic, technology.

---

## 2. Zergatik 80C552 eta ez 8051 arrunta?
- 8051 estandarretik haratago doazen hardware ezaugarriak ditu:
  - **A/D Bihurgailua (10-bit)** sentsore analogikoak irakurtzeko.
  - **PWM (Pulse Width Modulation)** seinale garbiak sortzeko.
  - **I2C autobusa** gailuekin komunikatzeko.
  - **I/O ataka gehigarriak** (P4 eta P5, guztira askoz pin gehiago sarrera/irteeretarako).

> **Irudiaren Prompta (Google Nano):** A 3D schematic diagram highlighting the internal blocks of a microcontroller: ADC, PWM, I2C, glowing colorful blocks on a circuitry grid, modern tech abstract.

---

## 3. Garapen Ingurunea: Keil μVision
- **C lengoaiako programazioa:** Mihiztadura (Assembly) alde batera utzi eta C erabiliz programatzen da (`C51` konpiladorea).
- **Abantailak:**
  - `reg552.h` liburutegiak erregistro guztiak zuzenean mapatzen ditu C lengoaian.
  - Hardwarearen kontrol zehatza (sbit adierazpenekin).
  - Arazketa eta simulazioa software beretik egiteko gaitasuna.

> **Irudiaren Prompta (Google Nano):** A modern software developer interface on a sleek laptop screen showing C code and a debugger, with a microcontroller development board sitting next to the laptop on a clean wooden desk, warm lighting.

---

## 4. Hardwarearen Bihotza: Osziladorea
- **Maiztasuna:** Datu teknikoen arabera 12 MHz, 11.0592 MHz, edo 16 MHz-eko kuartzo kristala izaten du.
- **Ziklo-makina (Machine Cycle):** 
  - 8051 estandarrean instrukzio batek 12 erloju-ziklo behar ditu.
  - Adibidea: $f_{osc} = 12 \text{ MHz}$ bada -> Ziklo bat = $1 \text{ }\mu\text{s}$.
  - Adibidea: $f_{osc} = 11.0592 \text{ MHz}$ bada -> Ziklo bat ≈ $1.085 \text{ }\mu\text{s}$.

> **Irudiaren Prompta (Google Nano):** A macro close-up of a silver quartz crystal oscillator component on a green printed circuit board (PCB), sharp focus, engineering details.

---

## 5. C51 Arkitekturaren Memoria-Eredua
- Mikrokontrolagailu honek memoria ezberdinak fisikoki banatuta ditu (Harvard Arkitetkura):
  - **Program Memory (ROM):** Kodea gordetzeko (Flash ikuspegian).  `code` espazioa. C51en gako hitz honekin taulak edo konstanteak defini daitezke memoria irakurri-bakarrean gordetzeko.
  - **Data Memory (RAM):** Aldagaiak eta hardware erregistroak irakurri/idazteko prozesatzeko unean. Berau oso txikia da.

> **Irudiaren Prompta (Google Nano):** An abstract 3D visualization of computer memory blocks (ROM and RAM), glowing translucent cubes transferring data beams, high tech, vivid colors.

---

## ONDORIOAK
- 80C552 oinarri sendoa da mikrokontrolagailuen munduan murgiltzeko.
- Proiektuen logika `main` diseinu batera eta Etenduren (Interrupts) diseinu batera egokitu behar da espazio txiroa eraginkortasunez aprobetxatzeko.

> **Irudiaren Prompta (Google Nano):** A professional electronics engineering workbench with soldering iron, multimeters, and an 8051 microcontroller board, warmly lit, academic and industrial atmosphere.
