#include <stdio.h>
#include <stdlib.h>

// Hoan vi khoi tao IP (64 bit)
const int IP[64] = {
    58, 50, 42, 34, 26, 18, 10, 2,
    60, 52, 44, 36, 28, 20, 12, 4,
    62, 54, 46, 38, 30, 22, 14, 6,
    64, 56, 48, 40, 32, 24, 16, 8,
    57, 49, 41, 33, 25, 17,  9, 1,
    59, 51, 43, 35, 27, 19, 11, 3,
    61, 53, 45, 37, 29, 21, 13, 5,
    63, 55, 47, 39, 31, 23, 15, 7
};

// Hoan vi dao nguoc IP^-1 (64 bit)
const int IP_inv[64] = {
    40, 8, 48, 16, 56, 24, 64, 32,
    39, 7, 47, 15, 55, 23, 63, 31,
    38, 6, 46, 14, 54, 22, 62, 30,
    37, 5, 45, 13, 53, 21, 61, 29,
    36, 4, 44, 12, 52, 20, 60, 28,
    35, 3, 43, 11, 51, 19, 59, 27,
    34, 2, 42, 10, 50, 18, 58, 26,
    33, 1, 41,  9, 49, 17, 57, 25
};

// Bang mo rong E (32 bit -> 48 bit)
const int E_table[48] = {
    32,  1,  2,  3,  4,  5,
     4,  5,  6,  7,  8,  9,
     8,  9, 10, 11, 12, 13,
    12, 13, 14, 15, 16, 17,
    16, 17, 18, 19, 20, 21,
    20, 21, 22, 23, 24, 25,
    24, 25, 26, 27, 28, 29,
    28, 29, 30, 31, 32,  1
};

// Hoan vi P trong ham F (32 bit)
const int P_table[32] = {
    16,  7, 20, 21, 29, 12, 28, 17,
     1, 15, 23, 26,  5, 18, 31, 10,
     2,  8, 24, 14, 32, 27,  3,  9,
    19, 13, 30,  6, 22, 11,  4, 25
};

// Lua chon hoan vi PC-1 (64 bit -> 56 bit)
const int PC1[56] = {
    57, 49, 41, 33, 25, 17,  9,
     1, 58, 50, 42, 34, 26, 18,
    10,  2, 59, 51, 43, 35, 27,
    19, 11,  3, 60, 52, 44, 36,
    63, 55, 47, 39, 31, 23, 15,
     7, 62, 54, 46, 38, 30, 22,
    14,  6, 61, 53, 45, 37, 29,
    21, 13,  5, 28, 20, 12,  4
};

// Lua chon hoan vi PC-2 (56 bit -> 48 bit)
const int PC2[48] = {
    14, 17, 11, 24,  1,  5,
     3, 28, 15,  6, 21, 10,
    23, 19, 12,  4, 26,  8,
    16,  7, 27, 20, 13,  2,
    41, 52, 31, 37, 47, 55,
    30, 40, 51, 45, 33, 48,
    44, 49, 39, 56, 34, 53,
    46, 42, 50, 36, 29, 32
};

// So bit dich trai qua 16 vong sinh khoa
const int soBitDich[16] = {
    1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1
};

// 8 hop S-box (moi hop 4 hang x 16 cot)
const unsigned char S_box[8][4][16] = {
    // S1
    {
        {14,  4, 13,  1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9,  0,  7},
        { 0, 15,  7,  4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5,  3,  8},
        { 4,  1, 14,  8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10,  5,  0},
        {15, 12,  8,  2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0,  6, 13}
    },
    // S2
    {
        {15,  1,  8, 14,  6, 11,  3,  4,  9,  7,  2, 13, 12,  0,  5, 10},
        { 3, 13,  4,  7, 15,  2,  8, 14, 12,  0,  1, 10,  6,  9, 11,  5},
        { 0, 14,  7, 11, 10,  4, 13,  1,  5,  8, 12,  6,  9,  3,  2, 15},
        {13,  8, 10,  1,  3, 15,  4,  2, 11,  6,  7, 12,  0,  5, 14,  9}
    },
    // S3
    {
        {10,  0,  9, 14,  6,  3, 15,  5,  1, 13, 12,  7, 11,  4,  2,  8},
        {13,  7,  0,  9,  3,  4,  6, 10,  2,  8,  5, 14, 12, 11, 15,  1},
        {13,  6,  4,  9,  8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7},
        { 1, 10, 13,  0,  6,  9,  8,  7,  4, 15, 14,  3, 11,  5,  2, 12}
    },
    // S4
    {
        { 7, 13, 14,  3,  0,  6,  9, 10,  1,  2,  8,  5, 11, 12,  4, 15},
        {13,  8, 11,  5,  6, 15,  0,  3,  4,  7,  2, 12,  1, 10, 14,  9},
        {10,  6,  9,  0, 12, 11,  7, 13, 15,  1,  3, 14,  5,  2,  8,  4},
        { 3, 15,  0,  6, 10,  1, 13,  8,  9,  4,  5, 11, 12,  7,  2, 14}
    },
    // S5 
    {
        { 2, 12,  4,  1,  7, 10, 11,  6,  8,  5,  3, 15, 13,  0, 14,  9},
        {14, 11,  2, 12,  4,  7, 13,  1,  5,  0, 15, 10,  3,  9,  8,  6},
        { 4,  2,  1, 11, 10, 13,  7,  8, 15,  9, 12,  5,  6,  3,  0, 14},
        {11,  8, 12,  7,  1, 14,  2, 13,  6, 15,  0,  9, 10,  4,  5,  3}
    },
    // S6
    {
        {12,  1, 10, 15,  9,  2,  6,  8,  0, 13,  3,  4, 14,  7,  5, 11},
        {10, 15,  4,  2,  7, 12,  9,  5,  6,  1, 13, 14,  0, 11,  3,  8},
        { 9, 14, 15,  5,  2,  8, 12,  3,  7,  0,  4, 10,  1, 13, 11,  6},
        { 4,  3,  2, 12,  9,  5, 15, 10, 11, 14,  1,  7,  6,  0,  8, 13}
    },
    // S7
    {
        { 4, 11,  2, 14, 15,  0,  8, 13,  3, 12,  9,  7,  5, 10,  6,  1},
        {13,  0, 11,  7,  4,  9,  1, 10, 14,  3,  5, 12,  2, 15,  8,  6},
        { 1,  4, 11, 13, 12,  3,  7, 14, 10, 15,  6,  8,  0,  5,  9,  2},
        { 6, 11, 13,  8,  1,  4, 10,  7,  9,  5,  0, 15, 14,  2,  3, 12}
    },
    // S8
    {
        {13,  2,  8,  4,  6, 15, 11,  1, 10,  9,  3, 14,  5,  0, 12,  7},
        { 1, 15, 13,  8, 10,  3,  7,  4, 12,  5,  6, 11,  0, 14,  9,  2},
        { 7, 11,  4,  1,  9, 12, 14,  2,  0,  6, 10, 13, 15,  3,  5,  8},
        { 2,  1, 14,  7,  4, 10,  8, 13, 15, 12,  9,  0,  3,  5,  6, 11}
    }
};

// Ham thuc hien hoan vi bit tong quat (tinh bit 1 tu ben trai / MSB)
unsigned long long hoanVi(unsigned long long dauVao, const int *bang, int lenVao, int lenRa) {
    unsigned long long ketQua = 0;
    for (int i = 0; i < lenRa; i++) {
        int viTri = bang[i];
        unsigned long long bit = (dauVao >> (lenVao - viTri)) & 1ULL;
        ketQua = (ketQua << 1) | bit;
    }
    return ketQua;
}

// Dich vong trai thanh ghi 28 bit
unsigned int dichTrai28(unsigned int val, int soBit) {
    val &= 0x0FFFFFFF;
    return ((val << soBit) | (val >> (28 - soBit))) & 0x0FFFFFFF;
}

// Sinh 16 khoa con 48-bit tu khoa goc 64-bit
void sinhKhoaCon(unsigned long long khoaChinh, unsigned long long khoaVong[16]) {
    unsigned long long khoa56 = hoanVi(khoaChinh, PC1, 64, 56);
    unsigned int C = (unsigned int)((khoa56 >> 28) & 0x0FFFFFFF);
    unsigned int D = (unsigned int)(khoa56 & 0x0FFFFFFF);

    for (int i = 0; i < 16; i++) {
        C = dichTrai28(C, soBitDich[i]);
        D = dichTrai28(D, soBitDich[i]);

        unsigned long long CD = (((unsigned long long)C) << 28) | D;
        khoaVong[i] = hoanVi(CD, PC2, 56, 48);
    }
}

// Ham Feistel F(R, K_i)
unsigned int hamF(unsigned int R, unsigned long long K) {
    // 1. Mo rong R tu 32 bit len 48 bit qua bang E
    unsigned long long moRong = hoanVi((unsigned long long)R, E_table, 32, 48);

    // 2. XOR voi khoa vong K (48 bit)
    unsigned long long sauXor = moRong ^ K;

    // 3. Thay the qua 8 hop S-box (48 bit -> 32 bit)
    unsigned int sauSbox = 0;
    for (int i = 0; i < 8; i++) {
        // Tach tung nhom 6 bit tu trai sang phai
        unsigned char khoi6 = (sauXor >> (42 - i * 6)) & 0x3F;
        // 2 bit ngoai (bit 5 va bit 0) xac dinh hang
        unsigned char hang = ((khoi6 & 0x20) >> 4) | (khoi6 & 0x01);
        // 4 bit giua (bit 4..1) xac dinh cot
        unsigned char cot = (khoi6 >> 1) & 0x0F;

        sauSbox = (sauSbox << 4) | S_box[i][hang][cot];
    }

    // 4. Hoan vi P (32 bit)
    return (unsigned int)hoanVi((unsigned long long)sauSbox, P_table, 32, 32);
}

// Ham ma hoa DES (64 bit)
unsigned long long maHoaDES(unsigned long long banRo, unsigned long long khoaVong[16]) {
    // Hoan vi khoi tao IP
    unsigned long long sauIP = hoanVi(banRo, IP, 64, 64);

    unsigned int L = (unsigned int)((sauIP >> 32) & 0xFFFFFFFF);
    unsigned int R = (unsigned int)(sauIP & 0xFFFFFFFF);

    // 16 vong Feistel
    for (int i = 0; i < 16; i++) {
        unsigned int L_cu = L;
        unsigned int R_cu = R;

        L = R_cu;
        R = L_cu ^ hamF(R_cu, khoaVong[i]);
    }

    // Hoan doi 32 bit cuoi (R16 || L16) va hoan vi nguoc IP^-1
    unsigned long long truocIPinv = (((unsigned long long)R) << 32) | L;
    return hoanVi(truocIPinv, IP_inv, 64, 64);
}

int main() {
}