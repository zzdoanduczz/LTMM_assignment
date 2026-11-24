#include <stdio.h>
#include <stdlib.h>

#define dathuctoigian 0x1B // m(x) = x^8 + x^4 + x^3 + x + 1

// Bang thay the S-box (256 byte)
const unsigned char S_box[256] = {
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
};

// Hang so vong RC[1..10]
const unsigned char RC[11] = {
    0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1B, 0x36
};

// Phep nhan trong truong huu han GF(2^8)
unsigned char gf28_mul(unsigned char a, unsigned char b) {
    unsigned char p = 0;

    while (b > 0) {
        if (b & 1) {
            p ^= a;
        }

        unsigned char hi_bit_set = a & 0x80;
        a <<= 1;

        if (hi_bit_set) {
            a ^= dathuctoigian;
        }

        a &= 0xFF;
        b >>= 1;
    }
    return p;
}

// Mo rong khoa AES-128: tu 16 byte khoa goc tao ra 44 word (176 byte)
void moRongKhoa(const unsigned char khoaChinh[16], unsigned char w[44][4]) {
    unsigned char temp[4];

    // 4 word dau tien (w[0..3]) lay truc tiep tu khoa chinh
    for (int i = 0; i < 4; i++) {
        w[i][0] = khoaChinh[4 * i];
        w[i][1] = khoaChinh[4 * i + 1];
        w[i][2] = khoaChinh[4 * i + 2];
        w[i][3] = khoaChinh[4 * i + 3];
    }

    // Tinh tiep w[4..43]
    for (int i = 4; i < 44; i++) {
        for (int j = 0; j < 4; j++) {
            temp[j] = w[i - 1][j];
        }

        if (i % 4 == 0) {
            // RotWord: dich vong trai 1 byte
            unsigned char t = temp[0];
            temp[0] = temp[1];
            temp[1] = temp[2];
            temp[2] = temp[3];
            temp[3] = t;

            // SubWord: thay the tung byte qua S-box
            for (int j = 0; j < 4; j++) {
                temp[j] = S_box[temp[j]];
            }

            // XOR byte dau tien voi hang so vong RC[i/4]
            temp[0] ^= RC[i / 4];
        }

        for (int j = 0; j < 4; j++) {
            w[i][j] = w[i - 4][j] ^ temp[j];
        }
    }
}

// Bien doi AddRoundKey
void addRoundKey(unsigned char state[4][4], unsigned char w[44][4], int vong) {
    for (int cot = 0; cot < 4; cot++) {
        for (int hang = 0; hang < 4; hang++) {
            state[hang][cot] ^= w[vong * 4 + cot][hang];
        }
    }
}

// Bien doi SubBytes
void subBytes(unsigned char state[4][4]) {
    for (int hang = 0; hang < 4; hang++) {
        for (int cot = 0; cot < 4; cot++) {
            state[hang][cot] = S_box[state[hang][cot]];
        }
    }
}

// Bien doi ShiftRows
void shiftRows(unsigned char state[4][4]) {
    unsigned char temp;

    // Hang 1: dich trai 1 byte
    temp = state[1][0];
    state[1][0] = state[1][1];
    state[1][1] = state[1][2];
    state[1][2] = state[1][3];
    state[1][3] = temp;

    // Hang 2: dich trai 2 byte
    temp = state[2][0];
    state[2][0] = state[2][2];
    state[2][2] = temp;
    temp = state[2][1];
    state[2][1] = state[2][3];
    state[2][3] = temp;

    // Hang 3: dich trai 3 byte (tuong duong dich phai 1 byte)
    temp = state[3][3];
    state[3][3] = state[3][2];
    state[3][2] = state[3][1];
    state[3][1] = state[3][0];
    state[3][0] = temp;
}

// Bien doi MixColumns
void mixColumns(unsigned char state[4][4]) {
    unsigned char c[4];

    for (int cot = 0; cot < 4; cot++) {
        for (int hang = 0; hang < 4; hang++) {
            c[hang] = state[hang][cot];
        }

        state[0][cot] = gf28_mul(0x02, c[0]) ^ gf28_mul(0x03, c[1]) ^ c[2] ^ c[3];
        state[1][cot] = c[0] ^ gf28_mul(0x02, c[1]) ^ gf28_mul(0x03, c[2]) ^ c[3];
        state[2][cot] = c[0] ^ c[1] ^ gf28_mul(0x02, c[2]) ^ gf28_mul(0x03, c[3]);
        state[3][cot] = gf28_mul(0x03, c[0]) ^ c[1] ^ c[2] ^ gf28_mul(0x02, c[3]);
    }
}

// Ham ma hoa AES-128
void maHoaAES128(const unsigned char banRo[16], const unsigned char khoa[16], unsigned char banMa[16]) {
    unsigned char state[4][4];
    unsigned char w[44][4];

    // Nap 16 byte ban ro vao ma tran trang thai 4x4 theo thu tu tung cot
    for (int cot = 0; cot < 4; cot++) {
        for (int hang = 0; hang < 4; hang++) {
            state[hang][cot] = banRo[cot * 4 + hang];
        }
    }

    // Sinh 44 word khoa vong
    moRongKhoa(khoa, w);

    // Vong khoi tao (Round 0)
    addRoundKey(state, w, 0);

    // 9 vong lap chinh (Round 1 -> 9)
    for (int vong = 1; vong <= 9; vong++) {
        subBytes(state);
        shiftRows(state);
        mixColumns(state);
        addRoundKey(state, w, vong);
    }

    // Vong cuoi cung (Round 10 - khong co MixColumns)
    subBytes(state);
    shiftRows(state);
    addRoundKey(state, w, 10);

    // Xuat ma tran trang thai ra mang 16 byte ban ma theo thu tu tung cot
    for (int cot = 0; cot < 4; cot++) {
        for (int hang = 0; hang < 4; hang++) {
            banMa[cot * 4 + hang] = state[hang][cot];
        }
    }
}

void inKhoiHex(const char *nhan, const unsigned char duLieu[16]) {
    printf("%s", nhan);
    for (int i = 0; i < 16; i++) {
        printf("%02x", duLieu[i]);
    }
    printf("\n");
}

int main() {
}