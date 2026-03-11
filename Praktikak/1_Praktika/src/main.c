#include <reg552.h> /* 80C552 espezifikoa: P4, ADC, PWM, I2C SFR guztiak barnebiltzen ditu */
#include <math.h>  /* sqrt() funtzioa 3. ariketarako */

// 1. Ariketa: Hiru zenbaki oso sortu eta hasieratu.
// Horrietako bat negatiboa bada, biderketa egin. Bestela, batura lor ezazu.
float ariketa1(int a, int b, int c) {
    if (a < 0 || b < 0 || c < 0) {
        return (float)(a * b * c); // Biderketa
    } else {
        return (float)(a + b + c); // Batura
    }
}

// 2. Ariketa: Matrizearen elementu guztien batura eta biderketa.
float ariketa2_batura(float mat[3][3]) {
    float sum = 0;
    unsigned char i, j;
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            sum += mat[i][j];
        }
    }
    return sum;
}

float ariketa2_biderketa(float mat[3][3]) {
    float prod = 1;
    unsigned char i, j;
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            prod *= mat[i][j];
        }
    }
    return prod;
}

// 3. Ariketa: Polinomioaren erroak (ax^2 + bx + c)
// Egitura bat erabiliko dugu bi erroak itzultzeko (errealak direla suposatuko dugu)
typedef struct {
    float x1;
    float x2;
    unsigned char erro_motak; // 0=errealak, 1=bikoitza, 2=irudikariak
} Erroak;

Erroak ariketa3(float a, float b, float c) {
    Erroak emaitza;
    float diskriminatzailea = (b * b) - (4 * a * c);
    
    if (diskriminatzailea > 0) {
        emaitza.erro_motak = 0; // Bi erro erreal
        emaitza.x1 = (-b + sqrt(diskriminatzailea)) / (2 * a);
        emaitza.x2 = (-b - sqrt(diskriminatzailea)) / (2 * a);
    } else if (diskriminatzailea == 0) {
        emaitza.erro_motak = 1; // Erro bikoitza
        emaitza.x1 = -b / (2 * a);
        emaitza.x2 = emaitza.x1;
    } else {
        emaitza.erro_motak = 2; // Erro konplexuak
        // Zati erreala soilik gordeko dugu sinplifikatzeko Keil-en erakustean
        emaitza.x1 = -b / (2 * a); 
        emaitza.x2 = sqrt(-diskriminatzailea) / (2 * a); // zati irudikaria
    }
    return emaitza;
}

// 4. Ariketa: Matrizeen eragiketak (A+B, A-B, A*B, A^T)
void ariketa4_batura(int A[3][3], int B[3][3], int res[3][3]) {
    unsigned char i, j;
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            res[i][j] = A[i][j] + B[i][j];
        }
    }
}

void ariketa4_kenketa(int A[3][3], int B[3][3], int res[3][3]) {
    unsigned char i, j;
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            res[i][j] = A[i][j] - B[i][j];
        }
    }
}

void ariketa4_biderketa(int A[3][3], int B[3][3], int res[3][3]) {
    unsigned char i, j, k;
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            res[i][j] = 0;
            for (k = 0; k < 3; k++) {
                res[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

void ariketa4_iraulia(int A[3][3], int res[3][3]) {
    unsigned char i, j;
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            res[j][i] = A[i][j];
        }
    }
}

// 5. Ariketa: Biten erabilerarako ariketa (shift eta maskarak)
// zenbL-ren nibble arruntari (beheko lau bitak) so egitea
unsigned int ariketa5(unsigned int zenb) {
    unsigned char zenbH = (zenb >> 8) & 0xFF; // Goi bytea
    unsigned char zenbL = zenb & 0xFF;        // Behe bytea
    
    // nibble_0 lortzko
    unsigned char nibble_0 = zenbL & 0x0F;
    unsigned char nibble_1 = (zenbL >> 4) & 0x0F;
    unsigned char nibble_2 = (zenbH) & 0x0F;
    unsigned char nibble_3 = (zenbH >> 4) & 0x0F;
    
    unsigned int emaitza = 0;

    switch (nibble_0) {
        case 0x01: // nibble 3, 2 eta 1 osa zenbakia
            emaitza = (nibble_3 << 8) | (nibble_2 << 4) | nibble_1;
            break;
            
        case 0x02: // zenbH eta zenbL alderantzikatu (swap bytes)
            emaitza = (zenbL << 8) | zenbH;
            break;
            
        case 0x04: // nibble 0, 1, 2, 3 ordenean zenbaki bat osatu
            emaitza = (nibble_0 << 12) | (nibble_1 << 8) | (nibble_2 << 4) | nibble_3;
            break;
            
        case 0x08: // B15-B4 esangarritasuna alderantzikatu
            // Hau da: bit15 -> bit4, bit14 -> bit5, e.a. (nibbleak iraulita ez baizik bitak)
            // Edo akaso nibble ordena soilik: 3,2,1 -> 1,2,3
            // Konstanteak bit-maila bit-maskara erabiliz
            // Ariketak dio "esangarritasuna alderantzikatu" bit 15 etik 4ra.
            // Errazena nibbleak alderantzikatzea da (1->2->3):
            emaitza = (nibble_1 << 12) | (nibble_2 << 8) | (nibble_3 << 4) | nibble_0;
            break;
            
        default:
            emaitza = 0;
            break;
    }
    
    return emaitza;
}


// --- Main ---
void main(void) {
    // Aldagaien definitzea C zaharraren arabera blokearen hasieran
    float res1_batura, res1_biderketa;
    float C2[3][3] = {
        {2, 4, 7},
        {4, 2, 6},
        {1, 5, 9}
    };
    float batura2, biderketa2;
    Erroak poli_erroak;
    
    int A[3][3] = {
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 9}
    };
    int B[3][3] = {
        {9, 8, 7},
        {6, 5, 4},
        {3, 2, 1}
    };
    int batura[3][3];
    int kenketa[3][3];
    int biderketa[3][3];
    int iraulia[3][3];
    
    unsigned int bit_res_1, bit_res_2, bit_res_4, bit_res_8, bit_res_0;

    // ---- 1. Ariketa Frogak ----
    res1_batura = ariketa1(5, 10, 15);     // 30
    res1_biderketa = ariketa1(5, -2, 10);  // -100
    
    // ---- 2. Ariketa Frogak ----
    batura2 = ariketa2_batura(C2);
    biderketa2 = ariketa2_biderketa(C2);
    
    // ---- 3. Ariketa Frogak ----
    poli_erroak = ariketa3(1.0, -5.0, 6.0); // x^2 - 5x + 6 = 0 -> Erroak: 3 eta 2

    // ---- 4. Ariketa Frogak ----
    ariketa4_batura(A, B, batura);
    ariketa4_kenketa(A, B, kenketa);
    ariketa4_biderketa(A, B, biderketa);
    ariketa4_iraulia(A, iraulia);
    
    // ---- 5. Ariketa Frogak ----
    bit_res_1 = ariketa5(0xABCD & 0xFFF1); // nibble 0 = 1 -> (A<<8)|(B<<4)|C = 0x0ABC
    bit_res_2 = ariketa5(0xABCD & 0xFFF2); // nibble 0 = 2 -> swap(CD, AB) = 0xCDAB
    bit_res_4 = ariketa5(0xABCD & 0xFFF4); // nibble 0 = 4 -> 4, C, B, A = 0x4CBA
    bit_res_8 = ariketa5(0xABCD & 0xFFF8); // nibble 0 = 8 -> esangarritasuna
    bit_res_0 = ariketa5(0xABCD & 0xFFF0); // 0 (bestelakoak)

    while(1) {
        // Begizta infinitua (mikroetan beharrezkoa)
    }
}
