# Erronka Gehigarria 1: Berotegi Adimentsua (Smart Greenhouse)

**Helburua**: 80C552 mikrokontrolagailuaren erabilera aurreratua, sarrera analogiko anitz, Timer-ak eta kanpo-etendurak uztartuz.

## Deskribapen Orokorra
Berotegi adimentsu baten kontrol-sistema inplementatu behar duzu. Sistemak barruko tenperatura eta argitasuna neurtuko ditu etengabe, eta baldintzen arabera, berogailua piztu, leihoak ireki edo ureztatze-sistema martxan jarriko du.

## Hardware Konektibitatea

### Sarrerak
- **P5.4 (ADC 4. kanala)**: Tenperatura sentsorea. 
  - Formula: `Tenperatura (ºC) = (100.0 / 1023) * kodeDigitala`
- **P5.5 (ADC 5. kanala)**: Argitasun sentsorea.
  - Formula: `Argitasuna (%) = (100.0 / 1023) * kodeDigitala`
- **P3.2 (INT0)**: Mantentze-etengailua (0 = Mantentze-lanetan / 1 = Modu automatikoa).
- **P1.0**: Ureztatze eskuzko sarrera (1 = Piztu ponpa).

### Irteerak
- **P4.0**: Berogailua (1 logikoa = Piztu)
- **P4.1**: Sapaiko Leihoa (1 logikoa = Ireki)
- **P4.2**: Ureztatze Balbula (1 logikoa = Ura askatu)
- **P4.7**: Alarma LED-a (1 logikoa = Piztu)

## Funtzionamenduaren Arauak

1. **Modu Automatikoa (P3.2 = 1)**:
   - Tenperatura irakurketa **5 segunduro** egingo da. Tenperatura `15ºC`-tik behera badago, berogailua aktibatu (P4.0). Tenperatura `25ºC`-tik gora badago, leihoa ireki (P4.1).
   - Argitasun irakurketa **10 segunduro** egingo da. Argitasuna `%20`-tik behera badago, LED Alarma piztu (P4.7) uzta arriskuan dagoela abisatzeko.
   - Ureztatzea: Eskuzko sarrerak (P1.0) 1 balioa jasotzen badu, ureztatze balbula ireki (P4.2) **zehazki 8 segundoz**, ondoren itxi (nahiz eta sarrerak 1 izaten jarraitu, hots, goranzko saihets / pultsu gisa tratatu).

2. **Mantentze Modua (P3.2 = 0 - INT0 bidez kudeatua)**:
   - Erabiltzaileak etengailua jaistean (falling-edge), sistema berehala mantentze-modura pasako da. 
   - Irteera guztiak desaktibatuko dira (P4 = 0x00).
   - Sistema blokeatuta geratuko da etengailua berriro 1 posiziora itzuli arte. Bueltatzean, denbora kontagailuak hutsetik hasiko dira.

## Betekizun Teknikoak
- Timer 0 (edo Timer 1) eskatzen diren 5s, 10s eta 8s denbora-tarteak neurtzeko erabili. 
- 11.0592 MHz-eko osziladorea duzula suposatu.
- Gidalerroetako (`.agent/rule/sed_arauen_gidalerroak.md`) Memory Optimization eta C51 teknikak jarraitu.
