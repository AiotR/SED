# SED Mikro-kontrolagailuen Proiektua (80552C)

Proiektu honen helburua 80552C (8051 familia) mikro-kontrolagailua programatzea da, C lengoaia eta Keil μVision plataforma erabilita. Hemen gordeko ditugu gaiari buruzko ariketak (Praktikak) eta azterketak (Azterketak). GitHub-era igotzeko prest dago egitura hau.

## Karpeten Egitura

- `Documentation/`: Datu-orriak (datasheets) eta eskuliburuak.
- `Enunciados/`: Praktiken enuntziatuak (PDF formakuan).
- `Azterketak/`: Azterketen enuntziatuak (PDF formakuan).
- `Praktikak/`: Praktiken C kodea eta μVision (Keil) proiektuak. Praktika bakoitzak bere karpeta izango du.
- `Azterketak_Ebazpenak/`: Azterketen ebazpenak (C kodean) eta euren Keil proiektuak.
- `Txantiloia/`: Proiektu berriak hasteko oinarrizko egitura (Template proiektua).
- `Nire ebazpenak/`: Erabiltzailearen benetako ariketa eta proiektu pertsonalak. **Etorkizuneko AI Agenteentzako oharra:** Karpeta hau erabiltzailearen lan-eremu nagusia da; hemen idatziko ditu bere azterketa edota ariketen soluzioak. Erabiltzaileak laguntza, iritzia, zuzenketak edo galderak ebaztea eskatzen badu, IA sistemek automatikoki karpeta honetako edukiak aztertu, funtzionamendua identifikatu eta erantzun pertsonalizatuak eskaini behar dituzte (ikuskaritza, C51 memoria maping-a, ISR konfigurazioa, optimizazioa).

## Firmware eta Garapen Ingurunea

- **Microcontroller (MCU):** 80552C (8051 arkitektura oinarritzat duena).
- **IDE:** Keil μVision.
- **Language:** C (C51 compiler).

## Goiburuko Fitxategia (`#include`)

Proiektu honetan **`reg552.h`** erabiltzen da beti, `reg52.h` generikoa erabili beharrean.

| Fitxategia | Txipa | P4 ataka | ADC / PWM / I2C |
|---|---|:---:|:---:|
| `reg52.h` | 8052 orokorra | ❌ | ❌ |
| `reg552.h` | **80C552 espezifikoa** | ✅ | ✅ |

`reg552.h`-k 80C552 txiparen **SFR (Special Function Register) guztiak** definitzen ditu. Beraz, `P4` edo `ADC` erabiltzeko `sfr P4 = 0xE8;` lerro erredundanterik jarri beharrik **ez dago**.

Keil proiektuetan (`.uvproj`) konpilatzean sortzen diren fitxategi tenporalak ez dira GitHub-era igoko, `.gitignore` fitxategiak horiek baztertuko dituelako (adibidez, `Objects/`, `Listings/`, `.obj`, `.lst` etab.).

## Nola Erabili

Ariketa edo azterketa bat egiten dugun bakoitzean:
1. `Txantiloia` karpeta kopiatu eta izena aldatu (adibidez, `Praktikak/1_Praktika`).
2. Kodea idatzi `src/` karpetaren barruan.
3. Keil μVision erabiliz konpilatu.
4. Git bidez aldaketak gorde (`git commit`) eta GitHub-era igo (`git push origin main`).

### Adimen Artifizialaren Aginduak (Workflows)

Proiektua hainbat gailutan modu erosoan sinkronizatzeko, bi komando berezi prestatu dira AI agenteentzat:
- **/hasi**: Deskargatu GitHub-etik azken aldaketak (`git pull origin main`) beste ordenagailu batean lan egiten hasi aurretik. Funtzionamendua automatizatua dago.
- **/bukatu**: Lanean amaitzerakoan aldaketa guztiak paketatu eta hodeira igotzeko (`git add .`, `git commit` eta `git push origin main`). Funtzionamendua automatizatua dago.

Gida hau eta kodea modu didaktikoan azaltzeko diseinatuta daude. Edozein zalantza baduzu, galdetu!
