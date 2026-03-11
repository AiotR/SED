---
marp: true
theme: default
paginate: true
---

# Portual Digitalak eta Sarrera/Irteerak
**80C552 Mikrokontrolagailua**

---

## 1. Atakak (Ports)
- 8-biteko atakak dira sistemarekin mundu fisikoa konektatzeko.
- 80C552-k 6 ataka nagusi ditu: **P0, P1, P2, P3, P4 erabilera orokorrerako** eta **P5 Sarrera analogikorako**.
- C kodean bitze mailako eragiketak zuzenean ahaltzen ditu.

---

## 2. Sarrera ala Irteera?
- Hardware-aren berezitasunagatik, **ez dago** Ataka Helbideratze Erregistro berezirik (adibidez Arduinoen `pinMode()`).
- Pin bat **SARRERA** gisa erabiltzeko, zirkuitu birtualki idatzi egin behar da sarrerarik onena lortzeko: elikadura jarriz.
- Praktikan: `P1 = 0xFF; // P1 guztia Sarrera bezala irakurtzeko gaitu`
- Pin bat **IRTEERA** gisa balio du zuzenean `0` (itzalita) edo `1` (piztuta) esleituz.

---

## 3. sbit (Single-Bit) Adierazleak
- `C51`-ren abantaila boteretsuena. Datu erregistro batetik Pin konkretu bat atzitzeko bidea.
- Kodean izen semantikoak definitzeko (kode irakurgarriagoa):
```c
#include <reg552.h>
sbit BOTOIA = P1^2; // Port 1, Pin 2
sbit LED    = P4^0; // Port 4, Pin 0
```
Ondorioz, programan `if (BOTOIA == 1)` edota `LED = 1;` egin daiteke.

---

## 4. Etenduren Sarrera (INT0 eta INT1)
- Zer gertatzen da botoi baten menpe sistema oso batek berehala erantzun behar badu (Larrialdietarako botoia)?
- Polling (Betizko begizta) saihesteko, hardware **Kanpo-Etendurak** existitzen dira (P3.2 eta P3.3-en).
- Modu bi:
  - **Level-triggered:** Pultsu baxua dagoen bitartean etengabe detektatzen da (IT0 = 0).
  - **Edge-triggered:** Saihets-beheantzakoa (falling-edge, "1"-etik "0"ra itzaltzean) unean birritan sakatzea ekiditen du behin bakarrik erregistratuz (IT0 = 1).

---

## 5. Kode Zatia: Etenduren Konfigurazioa

```c
// Erro nagusian (main-en konfigurazioa)
IT0 = 1; // INT0 falling-edge konfiguratu (P3.2 pinean eragiten du)
EX0 = 1; // INT0 prestatu
EA  = 1; // Etendura GUZTIAK (Globala) martxan jarri

// Funtzio kudeatzailea kanpoan...
void Larrialdia_ISR(void) interrupt 0 {
    // Burutu larrialdi sekuentzia (Adib. P4 = 0x00 motorrak itzaltzeko)
}
```

---

## LABURPENA
- Sarrerak jasotzeko portuei `1` osorik izendatzea ezinbestekoa da aurretik erregistroetatik energia sartzeko. 
- Etendurek (`interrupt x` etiketekin erabilitako ISR) beti gailentzen zaizkio `while(1)` errepikapeneko programa bati. Hori da gailu txertatuen magia!
