---
marp: true
theme: default
paginate: true
---

# Tenporizadoreak (Timers) 80C552-an
**Denboraren Kudeaketa Mikrokontrolagailuetan**

> **Irudiaren Prompta (Google Nano):** A futuristic digital clock mechanism made of glowing circuitry, abstract time concept, embedded systems, blue and gold lights.

---

## 1. Zergatik daude Tenporizadoreak?
- *Begizta Atzerapenak (Delay Loops)* erabiltzeak CPU osoa blokeatzen du software hutsean (adib: `for(i=0; i<1000; i++);`).
- Sistema Industrialetan ezin da CPUa aspertu. 
- **Timers-ak** zirkuitu hardware bitarteko kontagailuak dira, beren kabuz handitzen direnak CPUak beste lan batzuk egiten dituen bitartean.

> **Irudiaren Prompta (Google Nano):** A frustrated robotic CPU character being blocked by a giant unmoving loop, funny tech 3D illustration.

---

## 2. Zenbat Timer ditu 80C552-k?
Era tradizionaleko 8051ak bi timer ditu (**Timer 0** eta **Timer 1**). Baina 80C552 txipek beste batzuk ere gehitzen dituzte eragiketa konplexuagoetara begira:
- **Timer 0 / Timer 1**: Tradizionalak (Gehienez 16-bit). Baud Rate-ak eta denbora orokorra kalkulatzeko.
- **Timer 2**: 16 biteko kontalaria izaten da kapturarekin (Capture) edota Auto-Reload konplexuekin.
- **Hasierako TMOD (Timer Mode)** erregistroa erabiltzen da funtzionamendua hautatzeko.

> **Irudiaren Prompta (Google Nano):** Three glowing chronometers floating inside an electronic microchip, sleek modern 3D illustration, representing Timer 0, Timer 1, and Timer 2.

---

## 3. Modu Esanguratsuak (TMOD)
`TMOD` 8 biteko erregistroa erdibituta dago (4 bit Timer 1-tzat, 4 bit Timer 0-rentzat):
- **Modu 1 (16-bit hutsa)**: `0x0000`-tik `0xFFFF`-ra iristen da. Ondoren, `Overflow` edo gainezka egin eta TF (Timer Flag) aktibatuz *Interrupt* baten abisua sortzen du.
- **Modu 2 (8-bit Auto-Reload)**: `0x00`-tik `0xFF`-ra heltzean, automatikoki software kargarik gabe jatorrizko balio iraunkor batera jauzi egiten du berriz. Milisegunduko edo mikrosegunduko ekitaldi oso errepikakorretan (Semaforoak, erlojuak) oso zehatza da.

> **Irudiaren Prompta (Google Nano):** A perpetual mechanical loop representing an 'Auto-Reload' mechanism in an industrial setting, perpetual motion concept, high tech.

---

## 4. Matematika (Denbora Kalkuluak)
* 12 MHz-eko osziladorea hartzen badugu, Timer batek bere kontagailua $1 \mu s$ bakoitzeko gehitzen du.
* Demagun zehazki `50 millisekundo` ($50,000 \mu s$) atzerapen bat behar duzula **Timer 0 Modu 1** erabiliz:
  1. 16-bit erregistroa da: $65,536$
  2. $65536 - 50000 = 15536$
  3. Hau Base-16-ra (Hexadezimalera) itzultzen da: `0x3CB0`
  4. Kodean: `TH0 = 0x3C; TL0 = 0xB0;` idazten da ISR bakoitzean.

> **Irudiaren Prompta (Google Nano):** Glowing green mathematical equations and hex values like '0x3CB0' floating on a dark blackboard background, hacker style.

---

## 5. C51 Adibidea: Etendura Sinkronoa
```c
int ticks_50ms = 0; // Aldagai globala 1 segundoa zenbatzeko
void isr_timer0(void) interrupt 1 { // '1' zifra bidez bilatzen du Timer 0-ren isr hobia
    // TH eta TL 16 bitetako erregistroak berridatzi
    TH0 = 0x3C; 
    TL0 = 0xB0;
    
    ticks_50ms++;
    if (ticks_50ms >= 20) {
        ticks_50ms = 0; // Behin berrabiarazi
        // Hemen pasa den zehazki SEGUNDU BAT!
        P4 ^= 0x01; // Adibidez: LED bat piztu-itzali segunduro
    }
}
```

> **Irudiaren Prompta (Google Nano):** A green LED blinking on a circuit board, capturing the exact moment of turning on, long exposure showing the light trail, macro shot.

---

## ONDORIOA
- Timer batekin software bat ehunka aldiz zehatzagoa errotzen da.
- Beti ISR barruan jokatutako datuak aldagai orokor batzuetan pilatzen dira `main()` progarmak erabaki dezan noiz jokatu. Ezin diogu CPUri ukatu zereginik pisu eta denbora handien menpean.

> **Irudiaren Prompta (Google Nano):** A highly organized computer brain with separate mechanical gears turning smoothly to represent background tasks and CPU efficiency, 3D render.
