/*===========================================================
 * 2. PRAKTIKA: SEMAFORO EZBERDINEN PROGRAMAZIOA
 * 80C552 Mikrokontrolagailua - C51 (Keil)
 *
 * P1 atakaKONFIGURATION:
 *   P1.7 P1.6 P1.5 P1.4 P1.3 P1.2 P1.1 P1.0
 *    G    G    G    A    A    B    B    B
 *   G=Gorria, A=Anbarra, B=Berdea
 *
 *   Pizteko '1' logikoa, itzaltzeko '0' logikoa.
 *
 *   Maskarak:
 *     GORRIA  = 0b11100000 = 0xE0
 *     ANBARRA = 0b00011000 = 0x18
 *     BERDEA  = 0b00000111 = 0x07
 *     DENA_IT = 0b00000000 = 0x00
 *
 * P4 atakaKONFIGURATION:
 *   P4.0 = Etengailua (Gau-Egun modua)
 *   P4.1 = Pultsadorea (Oinezkoen eskaera)
 *===========================================================*/

#include <reg52.h>

/* --- SFR Definizio Bereziak (80C552-rako) ---
 * reg52.h fitxategiak P0, P1, P2, P3 definitzen ditu,
 * baina P4 ez da 8052 estandarrean. 80C552k badu P4, haren
 * helbidean definituko dugu eskuz. */
sfr P4 = 0xE8; /* P4 atakarentzako SFR helbidea 80C552-n */

/* --- Bit Maskarak Argientzako --- */
#define GORRIA    0xE0  /* 1110 0000 - P1.7, P1.6, P1.5 piztuta */
#define ANBARRA   0x18  /* 0001 1000 - P1.4, P1.3 piztuta       */
#define BERDEA    0x07  /* 0000 0111 - P1.2, P1.1, P1.0 piztuta */
#define DENA_ITZ  0x00  /* 0000 0000 - Dena itzalita             */

/* --- Bit Bandera Definizioak --- */
sbit ETENGAILUA = P4^0; /* P4.0 - Gau-Egun aldatzailea */
sbit PULTSADOREA = P4^1; /* P4.1 - Oinezko pultsadorea   */

/* ================================================
 * LAGUNTZAILE FUNTZIOA: itxaron_ms (denbora-atzeratzea)
 * 80C552 @ 12MHz oinarritzat hartuz.
 * OHARRA: Keil-eko simulagailuan zehatza ez bada ere,
 * hardwarean kalibratzeko hasierako balioa da.
 * ================================================ */
void itxaron_ms(unsigned int ms) {
    unsigned int i, j;
    for (i = 0; i < ms; i++) {
        for (j = 0; j < 120; j++) { /* ~1ms @ 12MHz */
            /* Hutsik - Denbora galtzeko begizta */
        }
    }
}

/* ================================================
 * a) SEMAFORO INOZOA
 * Ziklo jarraiak:
 *   Gorria:  15 segundo
 *   Berdea:  20 segundo
 *   Anbarra: 3 segundo
 * ================================================ */
void semaforo_inozoa(void) {
    while (1) {
        P1 = GORRIA;           /* Gorria piztu */
        itxaron_ms(15000);     /* 15 segundo itxaron */

        P1 = BERDEA;           /* Berdea piztu */
        itxaron_ms(20000);     /* 20 segundo itxaron */

        P1 = ANBARRA;          /* Anbarra piztu */
        itxaron_ms(3000);      /* 3 segundo itxaron */
    }
}

/* ================================================
 * b) GAU-EGUN SEMAFOROA
 * Semaforo inozoaren funtzionaltasunaz gain:
 *   - P4.0 etengailua '1' bada (Gau modua):
 *     Ziklo arrunta amaitu, ondoren anbarra
 *     aldizkaka (1seg piztu / 1seg itzali)
 *     P4.0 '0' den arte.
 * ================================================ */
void semaforo_gau_egun(void) {
    while (1) {
        /* --- Gorria (15 seg) --- */
        P1 = GORRIA;
        itxaron_ms(15000);

        /* --- Berdea (20 seg) --- */
        P1 = BERDEA;
        itxaron_ms(20000);

        /* --- Anbarra (3 seg) --- */
        P1 = ANBARRA;
        itxaron_ms(3000);

        /* --- Ziklo amaieran, etengailua egiaztatu --- */
        if (ETENGAILUA == 1) {
            /* Gau modura sartu: anbarra aldizkaka */
            while (ETENGAILUA == 1) {
                P1 = ANBARRA;       /* Anbarra piztu */
                itxaron_ms(1000);   /* 1 segundo */
                P1 = DENA_ITZ;     /* Anbarra itzali */
                itxaron_ms(1000);   /* 1 segundo */
            }
            /* Etengailua '0' denean, ziklo arruntari itzuli */
        }
    }
}

/* ================================================
 * c) OINEZKOEN SEMAFOROA
 * Aurreko funtzionaltasunaz gain:
 *   - P4.1 pultsadorea sakatzean:
 *     * Berdean badago: Berehala anbarrera pasa.
 *     * Gorrian badago: 15 seg-ko tenporizazioa
 *       berriro hasi.
 * ================================================ */
void semaforo_oinezkoekin(void) {
    /* Egoera-makina aldagaia 
     * 0 = GORRIA, 1 = BERDEA, 2 = ANBARRA */
    unsigned char egoera;
    unsigned int i;

    while (1) {
        /* --- GORRIA (15 seg, berrekin hasita pultsadorea sakatzean) --- */
        egoera = 0;
        P1 = GORRIA;
        i = 0;
        while (i < 15000) {
            itxaron_ms(1); /* Millisegundo batero egiaztatu */
            i++;

            if (PULTSADOREA == 1) {
                /* Gorrian: berrekini 15 segundoko tenporizazioa */
                i = 0; /* Kontadorea berrezarri */
            }

            /* Gau modua ere egiaztatu ziklo bakoitzean */
            if (ETENGAILUA == 1) {
                /* Gorria amaitu eta gau modura sartu */
                /* Lehenengo ziklo gardena amaitu behar dugu:
                 * berdea + anbarra pasa eta ondoren aldizkakoa */
                P1 = BERDEA;
                itxaron_ms(20000);
                P1 = ANBARRA;
                itxaron_ms(3000);
                while (ETENGAILUA == 1) {
                    P1 = ANBARRA;
                    itxaron_ms(1000);
                    P1 = DENA_ITZ;
                    itxaron_ms(1000);
                }
                /* Ziklo arruntari itzuli */
                i = 15000; /* Gorriaren begiztatik irteteko */
            }
        }

        /* --- BERDEA (20 seg, pultsadorea sakatzean moztu) --- */
        egoera = 1;
        P1 = BERDEA;
        i = 0;
        while (i < 20000) {
            itxaron_ms(1);
            i++;

            if (PULTSADOREA == 1) {
                /* Berdean: Berehala anbarrera pasa */
                i = 20000; /* Berdea moztu */
            }

            if (ETENGAILUA == 1) {
                P1 = ANBARRA;
                itxaron_ms(3000);
                while (ETENGAILUA == 1) {
                    P1 = ANBARRA;
                    itxaron_ms(1000);
                    P1 = DENA_ITZ;
                    itxaron_ms(1000);
                }
                i = 20000;
            }
        }

        /* --- ANBARRA (3 seg) --- */
        egoera = 2;
        P1 = ANBARRA;
        itxaron_ms(3000);

        /* Ziklo amaieran gau modua egiaztatu */
        if (ETENGAILUA == 1) {
            while (ETENGAILUA == 1) {
                P1 = ANBARRA;
                itxaron_ms(1000);
                P1 = DENA_ITZ;
                itxaron_ms(1000);
            }
        }
    }
}


/* ================================================
 * MAIN - Hemen aukeratu zein semaforo erabili.
 * Komentatu nahi ez duzuna.
 * ================================================ */
void main(void) {
    P1 = DENA_ITZ; /* Hasieran dena itzali */
    P4 = 0x00;     /* P4 sarrerak hasieratu */

    /* Aukeratu semaforo bat komentarioak kenduz: */

    /* semaforo_inozoa(); */
    /* semaforo_gau_egun(); */
    semaforo_oinezkoekin(); /* c) Semaforo osoa */
}
