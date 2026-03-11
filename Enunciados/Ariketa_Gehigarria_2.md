# Erronka Gehigarria 2: Semaforoa 7-Segmentuko Pantailarekin (Countdown)

**Helburua**: Irteera digital paraleloen erabilera aurreratua (7-segmentuko display), Timer etendurekin batera sinkronizatutako atzerako kontaketa bat egiteko. Baita INT1 (Beste kanpo etendura) sartzea ere.

## Deskribapen Orokorra
Bidegurutze garrantzitsu bateko oinezkoen semaforoa diseinatuko duzu. Oinezkoen semaforo hau beti Gorrian dago. Oinezko batek pultsadorea sakatzen duenean, semaforoa Berdera pasatzen da eta 9-tik 0-ra bitarteko atzerako kontaketa bat erakusten du 7-segmentuko pantaila batean, segunduro eguneratuta.

## Hardware Konektibitatea

### Sarrerak
- **P3.3 (INT1)**: Oinezkoen pultsadorea. Falling-edge detektatu behar du (P3.3 '0'-ra pasatzean etendura bidez).
- **P1.7**: "Gaueko Modua" etengailua. Aktibatuta badago (1 logikoa), semaforoa Laranja kolorean keinuka egongo da etengabe, eta pultsadoreak ez du eraginik izango.

### Irteerak
- **P1.0, P1.1, P1.2**: Semaforoaren argiak (0=Berdea, 1=Laranja, 2=Gorria). `1` igortzean pizteko.
- **P4 ataka osoa (P4.0 - P4.7)**: 7-segmentuko pantaila kontrolatzeko (Katodo Komuna). P4 porta 8-bit gisa erabiliko da, A-G eta DP segmentuei dagokienez.

## Funtzionamenduaren Arauak

1. **Hasierako Egoera (IDLE)**: 
   - Semaforoa **Gorrian** (P1.2 = 1, besteak 0).
   - 7-segmentuko pantaila itzalita egon behar da.

2. **Gaueko Modua (P1.7 = 1)**:
   - Gorria eta Berdea itzali.
   - P1.1 (Laranja) distiratzen hasiko da (piztu eta itzali) `0.5` segunduro (Timer bidez egina).
   - Pantailak itzalita jarraitzen du.
   - Oinezkoak botoia (INT1) sakatzen badu, ezer ez da gertatuko.

3. **Oinezkoak Sakatzean (INT1)**:
   - Zuzenean pantailan `9` zenbakia agertuko da eta argi **Berdea** piztuko da (besteak itzaliz).
   - Zehazki **Segundu 1eko** tarteetan (Timer batekin erregulaturik), pantailako zenbakia jaitsiz joango da (`8, 7, 6... 0`).
   - `0` zenbakira heltzean, Berdea itzaliko da, pantaila itzaliko da eta **Gorria** piztuko da berriro.
   - Kontaktua dagoen bitartean berriro sakatzeak ez luke hasieratik jarri behar kontaketa (Flag bat erabili hori saihesteko).

## Pisetako Gakoak
- `$7-Segmentuko Taula`: Kontuan izan P4-ra zehazki hexadezimalak (`0x3F` => 0, `0x06` => 1...) bidali behar dituzula zenbaki bakoitza ikusteko. Horretarako *Look-Up Table* bat (Array bat `code` memorian) ezin hobea da.
- Timer 0 erabiliz 1 segundo eta 0.5 segundo lortzeko oinarria 50ms (edo antzekoa) ISR batean ezarri.
