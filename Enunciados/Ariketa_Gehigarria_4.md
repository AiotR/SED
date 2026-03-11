# Erronka Gehigarria 4: Industria Motorraren Abiadura Kontrola (PWM + Serial)

**Helburua**: Keil C51 arkitekturan seinale PWM (Pulse Width Modulation) pertsonalizatu bat asmatzea Timer-ak erabiliz, ADC irakurketak PWM-aren ziklo-era (Duty Cycle) konektatzeko, eta datuak UART (Portu Seriale) bidez ordenagailura bidaltzeko.

## Deskribapen Orokorra
Industriako zinta garraiatzaile baten abiadura potentziometro (ADC) baten bitartez doitu behar duzu. Gainera, abiaduraren ehunekoa (0-100%) UART bidez ordenagailu batera bidaliko da 1 segunduro (monitorizazioa). 

## Hardware Konektibitatea

### Sarrerak
- **P5.0 (ADC 0. kanala)**: Potentziometro analogikoa, abiadura eskatzeko.

### Irteerak
- **P4.0**: PWM seinalea DC motorra kontrolatzeko hobi transistore batera. (Abiadura anplifikatzeko).

### Komunikazioa
- **UART (P3.0/P3.1)**: Baud rate arrunt bat konfiguratuko da (adib. 9600 bps), datatxuak `TxD`-tik bidaltzeko.

## Funtzionamenduaren Arauak

1. **Software PWM (Timer 0)**:
   - 80C552 mikrokontrolagailuak hardware PWM dezakeen arren, ezagutza sendotzeko **Software PWM** bat eraikiko duzu Timer 0 (edo Timer 1) erabiliz Mode 2an (Auto-Reload 8-bit).
   - Tarte osoa 100 urratsetan banatu. Ziklo-aktiboa (Duty Cycle) $0\%$ eta $100\%$ artean kudeatu:
     Adibidez, `P4.0` 1 egon dadila `X` tikez, eta 0 egon dadila `100 - X` tikez. Ondoren prozesua errepikatu. Hori etengabe ISR barruan egingo da P4.0 aldatuz.

2. **Abiadura Irakurri (ADC)**:
   - Polling bitartez (main begiztan), ADCren bidez uneko `0-1023` bitarteko balioa irakurri.
   - Balio hori `0-100` (%) tartera mapatu, eta hori izango da gure Software PWM-ak erabiliko duen `X` Duty Cycle berria.

3. **UART Transmisio Sinkronoa (Monitorizazioa)**:
   - Konfiguratu portu seriala. (Timer 1 baud rate sortzaile bezala ezaguna bada, Timer 2 edo hardware bitarikoak ikertu beharko dituzu, edota Timer 0 erabili PWMrako eta Timer 1 seriera!).
   - Behin `1 segundo` betetzen denean (honek PWMaren erloju berdina edo software denbora aldagaiak aprobetxatzea suposatzen du), mikrokontrolagailuak String / Testu bat bidaliko du ordenagailura.
   - Testua formatu honetan: `"Motorraren abiadura: %45 \r\n"`. `printf` funtzio estandarra `stdio.h` gehitu ezkero (eta `putchar` Keilentzat gainidatziz, edo UART erregistroei eskuaz `SBUF` idatziz).

## Ebaluazio eta Zailtasuna
Ariketa hau oso maila altukoa da. Ezaugarri ugarik talka egin dezakete:
- Serial baud rate sortzeko timer bereizi bat behar da.
- PWM etendura zehatza izateak serial irakurketa blokeatu lezake arretarik gabe.
Hau diseinu aurreratuen gakoa da!
