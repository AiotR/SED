---
marp: true
theme: default
paginate: true
---

# Sentsoreen Mundua: Bihurgailu Analogiko-Digitala (ADC)
**Nola itzultzen dira datu fisikoak 80C552an?**

---

## 1. Zergatik ADC (Analog-to-Digital)?
- Mikrokontrolagailuek bitarretan (`0` eta `1`) hitz egiten dute bakarrik (0 Volt edo 5 Volt).
- Naturako magnitudeak aldiz linealak dira (Tenperatura, Argitasuna, Soinua).
- Argitasuna %40an badago edo Tenperatura 23.5ºC bada, sentsoreak tentsio tartekatu bat eskainiko dio MCU-ari (adib. 2.5V edo 1.2V). **ADC-ak tentsio analagiko hau zenbaki ulergarri batera transformatzen du.**

---

## 2. 80C552-ren ADC Propietateak
- Arkitektura gehienetan gehigarri bat erosi ohi da, baina NXP 80C552ak dakar **10-bateko ADCa barneratuta**.
- *10-bit*-ek esan nahi du `0` eta `1023` bitarteko eskalak sortzen dituela.
- **Port 5** analogikoetarako zortzi irakurle ezberdin (kanal) jartzeko bidea ematen du `P5.0 - P5.7`. Guztira 8 sentsore paraleloan konekta ditzakegu modu beregain.

---

## 3. ADCON Erregistroa eta Funtzionamendua
A/D bihurketa egiteko bi erregistro nagusi daude:
- `ADCH`: Bihurketaren balioaren Zortzi bit garrantzitsuenak pilatzen dituen erregistroa (MBS).
- `ADCON`: Konfigurazioa. Honek ADC pizteko eta bihurketaren egoera ikusteko banderak (`ADCI` eta `ADCS`) ditu, baita zein kanal aztertuko dugun izendatzeko behealdeko 3 bit ere (000 = P5.0, 111 = P5.7).

---

## 4. Polling bidezko Bihurketa Funtzioa (C51)
Kode ezberdinetan beti berrerabili beharreko formula zera da, hardwareko gako-banderak irakurri eta 10 bitak bi shift (>> edo <<) eginaz pilatzea ekuazioari esker:

```c
unsigned int ADBihurketa(unsigned char kanala) {
    if (kanala > 7) kanala = 0;
    
    // ADCS (0x08) aktibatu, behe partean kanal zehatzaren helbidea (OR funtzio bidez)
    ADCON = (0x08 | kanala); 
    
    // ADCI bandera (0x10) ez dagoen bitartean (NOT), hardwareari bihurtzen utzi (Polling)
    while (!(ADCON & 0x10)); 
    
    // Konposizio osoa bueltatzen dugu C-k emandako ezker-desplaziamenduarekin: (H + L)
    return ((unsigned int)ADCH << 2) | (ADCON & 0x03); 
}
```

---

## 5. Datuak Transformazi Matematikora
Zuk nahi duzuna balio adjetibo bat denez, `0-1023` digital zenbakia balio esanguratsuetara maperatu behar da (Interpolazio lineal sinplea). Horretarako _Floating-point_ edo Enuntziatuetan datorren formula estandarra erabiliko dugu.
Adibide bat: *`Tenperatura ºCtara neurtu`*, non 0 azpian dagoen bitartea adierazteko `-20` kentzen den.

```c
int konbertsioa = ADBihurketa(3); // Portu P5.3 kanalaren irakurketa
// Floating matematiko operadore gisa tratatu 'f' erantsiz biderkaketa ezustekoak saihesteko.
float tenperatura = (70.0f / 1023.0f) * (float)konbertsioa - 20.0f;
```

---

## BUKAERA
- Kanpo osziladoren ADC-ak tentsio fisikoa hartzen duenean, programadoreari askatasuna ematen dio mundu errealeko sentsore garestien eta automatizazio algoritmo aurreratuen (PID edo Hysteresis ON/OFF kontroletan adibidez) arteko zubi gisa erabiltzeko.
