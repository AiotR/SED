<div align="center">

<img src="https://readme-typing-svg.demolab.com?font=Fira+Code&weight=600&size=26&pause=1000&color=C1D10C&center=true&vCenter=true&width=650&lines=SED+%E2%80%94+80552C+Mikrokontrolagailua;Sistemas+Empotrados+Digitales;C+%2B+Keil+%CE%BCVision+%2B+8051+Arkitektura" alt="Typing SVG">

<br/>

**80552C Mikrokontrolagailua programatzeko proiektua · Proyecto de Microcontroladores**

<br/>

![C](https://img.shields.io/badge/C-A8B9CC?style=for-the-badge&logo=c&logoColor=black)
![Keil](https://img.shields.io/badge/Keil_μVision-IDE-blue?style=for-the-badge)
![MCU](https://img.shields.io/badge/MCU-80552C_(8051)-c1d10c?style=for-the-badge)
![Architecture](https://img.shields.io/badge/Arch-8051_Familia-orange?style=for-the-badge)

</div>

---

## 🎯 Helburua / Objetivo

Proiektu honen helburua **80552C** (8051 familia) mikro-kontrolagailua programatzea da, **C lengoaia** eta **Keil μVision** plataforma erabilita. Hemen gordetzen dira:
- 📋 **Praktikak**: Ariketak eta soluzioak
- 📝 **Azterketak**: Azterketa enuntziatuak eta ebazpenak

---

## ⚠️ Garrantzizkoa: `reg552.h` vs `reg52.h`

> Proiektu honetan **beti** `reg552.h` erabiltzen da, ez `reg52.h` generikoa.

| Fitxategia | Txipa | P4 ataka | ADC / PWM / I2C |
|-----------|-------|----------|-----------------|
| `reg52.h` | 8052 orokorra | ❌ | ❌ |
| `reg552.h` | **80C552 espezifikoa** | ✅ | ✅ |

`reg552.h`-k 80C552 txiparen SFR (Special Function Register) guztiak definitzen ditu. `P4` edo `ADC` erabiltzeko ez da `sfr P4 = 0xE8;` lerro erredundanterik behar.

---

## 📂 Karpeten Egitura / Estructura de Carpetas

```
SED/
├── 📁 Documentation/        # Datasheets eta eskuliburuak
├── 📁 Enunciados/            # Praktiken enuntziatuak (PDF)
├── 📁 Azterketak/            # Azterketen enuntziatuak (PDF)
├── 📁 Praktikak/             # C kodea + Keil proiektuak
│   ├── 📁 1_Praktika/
│   ├── 📁 2_Praktika/
│   └── ...
├── 📁 Azterketak_Ebazpenak/  # Azterketa soluzioak (C + Keil)
├── 📁 Txantiloia/            # Proiektu berrien Template-a
└── 📁 Nire_Ebazpenak/        # ⭐ Erabiltzailearen lan-eremu nagusia
```

> 🤖 **AI Agenteak**: `Nire_Ebazpenak/` karpeta da lan-eremu nagusia. Erabiltzaileak laguntza eskatzen badu, automatikoki karpeta hau aztertu eta erantzun pertsonalizatuak eskaini (C51 memoria mapping, ISR konfigurazioa, optimizazioa).

---

## 🚀 Nola Erabili / Cómo Usar

```
1. Txantiloia/ karpeta kopiatu → izena aldatu (adibidez: Praktikak/3_Praktika)
2. Kodea idatzi src/ karpetaren barruan
3. Keil μVision → Konpilatu
4. Aldaketak gorde → GitHub-era igo
```

---

<details>
<summary><b>🤖 AI Agenteen Komandoak / Comandos para Agentes IA</b></summary>
<br>

Proiektua hainbat gailutan sinkronizatzeko:

**`/hasi`** — Lan hasi aurretik:
```bash
git pull origin main
```

**`/bukatu`** — Lan amaitu ondoren:
```bash
git add .
git commit -m "feat: ariketa/azterketa berria"
git push origin main
```

</details>

<details>
<summary><b>🛠️ Garapeneko Ingurumena / Entorno de Desarrollo</b></summary>
<br>

| | Zehaztapena |
|--|------------|
| **MCU** | 80552C (8051 arkitektura) |
| **IDE** | Keil μVision |
| **Lengoaia** | C (C51 konpilagailua) |
| **Header** | `reg552.h` (ez `reg52.h`) |

> Keil proiektuetan konpilatzean sortutako fitxategi tenporalak (Objects/, Listings/, .obj, .lst) `.gitignore`-k baztertzen ditu.

</details>

---

<div align="center">
  <img src="https://img.shields.io/badge/Ikasketarako_Proiektua-c1d10c?style=flat-square" alt="Ikasketa">
  <img src="https://img.shields.io/badge/Euskadi-🇪🇸-red?style=flat-square" alt="Euskadi">
</div>
