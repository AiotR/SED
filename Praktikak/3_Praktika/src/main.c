/*===========================================================
 * 3. PRAKTIKA: ARGIZKO ERROTULU BATEN PROGRAMAZIOA
 * 80C552 Mikrokontrolagailua - C51 (Keil)
 *
 * "ZORIONAK" errotulua P1 atakarentz:
 *   P1.7 = Z
 *   P1.6 = O
 *   P1.5 = R
 *   P1.4 = I
 *   P1.3 = O  (bigarrena)
 *   P1.2 = N
 *   P1.1 = A
 *   P1.0 = K
 *
 *   '1' logikoa = letra piztu
 *   '0' logikoa = letra itzali
 *
 * P4 atakaKONFIGURATION:
 *   P4.0 = Etengailu orokorra (1=martxan, 0=geldi)
 *   P4.1 = Noranzkoa (1=aurrera/goitik-behera, 0=atzera/behetik-gora)
 *   P4.2 = Abiadura (1=0.5 seg/konfig, 0=1 seg/konfig)
 *===========================================================*/

#include <reg552.h> /* 80C552 espezifikoa: P4, ADC, PWM, I2C... SFR guztiak barnebiltzen ditu */

/* OHARRA: reg552.h-k P4 dagoeneko definituta du.
 * Aurreko bertsioan 'sfr P4 = 0xE8;' eskuz definitu behar genuen (reg52.h generikoarekin),
 * baina orain ez da beharrezkoa. */

/* --- P4 Bit Definizioak --- */
sbit ETENGAILU_OROK = P4^0; /* 1=Martxan, 0=Geldi */
sbit NORANZKOA      = P4^1; /* 1=Aurrera, 0=Atzera */
sbit ABIADURA       = P4^2; /* 1=0.5seg, 0=1seg    */

/* ===================================================================
 * KONFIGURAZIO TAULA (Look-Up Table)
 * Praktikaren giltzarria da hau. "Taula" bat erabiltzen dugu
 * 17 konfigurazio guztiak memorian gordetzeko.
 * Tabula honetako balio bakoitza P1 atakarentzako byte bat da.
 *
 * Adibideak:
 *  Z O R I O N A K    <- Letrak (P1.7 ... P1.0)
 *  1 0 0 0 0 0 0 0 = 0x80   <- Z bakarrik
 *  0 1 0 0 0 0 0 0 = 0x40   <- O bakarrik
 *  1 1 1 1 1 1 1 1 = 0xFF   <- ZORIONAK dena
 * ================================================================== */
#define NUM_KONFIG 17

/* Taularen adierazpena: kodeak memorian (Flash/ROM) gordetako baita
 * 8051 arkitekturan 'code' gako-hitza erabiliz RAM aurrezten dugu */
unsigned char code konfig_taula[NUM_KONFIG] = {
    0x80,   /*  0: Z------- */
    0x40,   /*  1: -O------ */
    0x20,   /*  2: --R----- */
    0x10,   /*  3: ---I---- */
    0x08,   /*  4: ----O--- */
    0x04,   /*  5: -----N-- */
    0x02,   /*  6: ------A- */
    0x01,   /*  7: -------K */
    0xAA,   /*  8: Z-R-O-A- (1010 1010) */
    0x55,   /*  9: -OI-N-K  (0101 0101) */  
    0x80,   /* 10: Z------- */
    0xC0,   /* 11: ZO------ */
    0xE0,   /* 12: ZOR----- */
    0xF0,   /* 13: ZORI---- */
    0xF8,   /* 14: ZORIO--- */
    0xFC,   /* 15: ZORION-- */
    0xFE,   /* 16: ZORIONA- */
    /* 0xFF -> ZORIONAK (1. konfiguraziora itzuli aurretik) */
};

/* Azken konfigurazioa (ZORIONAK dena) bereizita definitua */
#define AZKEN_KONFIG 0xFF  /* Z O R I O N A K = 1111 1111 */

/* ================================================================
 * LAGUNTZAILE FUNTZIOA: itxaron_ms
 * ================================================================ */
void itxaron_ms(unsigned int ms) {
    unsigned int i, j;
    for (i = 0; i < ms; i++) {
        for (j = 0; j < 120; j++) {
            /* Denbora galtzeko begizta hutsa */
        }
    }
}

/* ================================================================
 * LAGUNTZAILE FUNTZIOA: lortu_atzerapena
 * ABIADURA etengailuaren arabera atzerapen egokia itzultzen du.
 * ================================================================ */
unsigned int lortu_atzerapena(void) {
    if (ABIADURA == 1) {
        return 500;  /* 0.5 segundo */
    } else {
        return 1000; /* 1 segundo   */
    }
}

/* ================================================================
 * FUNTZIO NAGUSIA: errotulua_exekutatu
 *
 * Logika:
 * 1. NORANZKOA == 1 → aurretik atzera (indizea 0etik 16ra)
 * 2. NORANZKOA == 0 → atzera etik aurrera (indizea 16tik 0ra)
 *
 * Konfigurazio bakoitzean ETENGAILU_OROK egiaztatzen da.
 * Gelditua bada (0), P1 itzali eta itxaron aktibo den arte.
 * ================================================================ */
void errotulua_exekutatu(void) {
    signed char indizea;  /* signed char [-128..127] begiztan behera joateko */
    unsigned int atzerapen;

    while (1) {
        /* Etengailu orokorra egiaztatu - geldi badago, itxaron */
        if (ETENGAILU_OROK == 0) {
            P1 = 0x00;  /* Dena itzali */
            while (ETENGAILU_OROK == 0) {
                /* Aktibo den arte itxaron */
            }
        }

        atzerapen = lortu_atzerapena();

        /* --- NORANZKOA: 1 = goitik behera (0 → 16 → AZKEN) --- */
        if (NORANZKOA == 1) {
            for (indizea = 0; indizea < NUM_KONFIG; indizea++) {
                if (ETENGAILU_OROK == 0) { indizea = NUM_KONFIG; break; } /* Eten */
                P1 = konfig_taula[indizea];
                itxaron_ms(lortu_atzerapena());
            }
            /* Azken konfigurazioa: dena piztuta */
            if (ETENGAILU_OROK == 1) {
                P1 = AZKEN_KONFIG;
                itxaron_ms(lortu_atzerapena());
            }
        }
        /* --- NORANZKOA: 0 = behetik gora (AZKEN → 16 → 0) --- */
        else {
            /* Lehenengo azken konfigurazioa (ZORIONAK osoa) erakutsi */
            if (ETENGAILU_OROK == 1) {
                P1 = AZKEN_KONFIG;
                itxaron_ms(lortu_atzerapena());
            }
            for (indizea = NUM_KONFIG - 1; indizea >= 0; indizea--) {
                if (ETENGAILU_OROK == 0) { break; } /* Eten */
                P1 = konfig_taula[indizea];
                itxaron_ms(lortu_atzerapena());
            }
        }
    }
}

/* ================================================================
 * MAIN
 * ================================================================ */
void main(void) {
    P1 = 0x00;  /* Dena itzali hasieran */
    P4 = 0x00;  /* P4 sarrerak hasieratu */

    errotulua_exekutatu();
}
