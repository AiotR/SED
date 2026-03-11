# Erronka Gehigarria 3: Segurtasun Tecladoa (Aurreratua)

**Helburua**: Matrize bidezko sarrerak (Keypad 4x4) maneiatzea iterazio sinkronoen bidez, makina-egoera baten trantsizio konplexuak lantzeko (Pasahitz sarrera).

## Deskribapen Orokorra
Eraikin bateko atea irekitzeko pasahitz sistema programatu behar da. Erabiltzaileak 4 digituko pasahitza sartu behar du teklatu matrizial baten bitartez (adibidez: "1-2-3-4"). Zuzena bada atea 3 segundoz irekiko da. Okerra bada blokeoa aktibatuko da.

## Hardware Konektibitatea

### Sarrerak
- **P1.0 - P1.3**: Teklatuko errenkadak (Rows). Sarrera bezala irakurtzeko.
- **P1.4 - P1.7**: Teklatuko zutabeak (Columns). Irteera bezala konfiguratzeko esplorazio (Scanning) bakoitzean bakoitzari `0` bat bidaliz.

### Irteerak
- **P4.0**: Elektrozerradura / Atea irekitzeko errelea (1 = Ireki).
- **P4.1**: LED Berdea (Pasahitz zuzena).
- **P4.2**: LED Gorria (Pasahitz okerra / Blokeoa).
- **P4.3**: Buzzer/Txistulari akustikoa (1 = Soinua atera pultsadorea sakatzean bertan finkatzeko).

## Funtzionamenduaren Arauak

1. **Teklatuaren Esplorazioa (Polling/Scanning)**:
   - Sarrera-irteerak (P1 ataka) konfiguratu, etengabe teklak begiratzeko. Tekla bat sakatzean (zutabe bati '0' txertatzean ondoko errenkada batek '0' bueltatzen badu) tekla identifikatu (0-9).
   - Eduki *debouncing* (erreboteen aurkako software atzerapena, adib. Timer ticks bidez edo For txiki baten bidez) tekla bakoitzak behin bakarrik erregistratu dezan sakatzea.
   - Tekla bat ezagutzean, Buzzer-ak (P4.3) *beep* txiki bat (0.1s dirdira) egin dezala.

2. **Pasahitza Ebaluatzea**:
   - `4` zenbaki onartuko ditu sistemak.
   - Pasahitza `1 - 2 - 3 - 4` memoria globalean definitutakoa bada:
     - Atea Igitzea (P4.0 = 1) eta LED Berdea piztuko da `3 segundoz`. Gero berrabiarazi.
   - Pasahitza oker esaten badu:
     - LED Gorria piztu. Erabiltzaileak berriro saiatzeko aukera izango du.

3. **Blokeo Segurua**:
   - Jarraian `3 aldiz` pasahitza oker jartzen bada, sistema BLOKEATU egingo da **15 segundoz**.
   - Blokeoan zehar LED Gorria keinuka (*Blinking*) egongo da `0.25 segundoro`. 
   - Ezingo da teklaturik erabili denbora horretan.

## Betekizun Teknikoak
- Timer bat derrigorrezkoa "15 segundu", "3 segundu" eta "0.25s blinka" asinkronoki neurtzeko teklatuaren eskaneoa %100 oztopatu gabe (hardware mailan optimoa izateko polling-a main-ean utzi bitartean).
