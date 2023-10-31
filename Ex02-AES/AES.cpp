// AES-128 Implementation
#include <iostream>
using namespace std;

// State Matrix
uint8_t state[4][4];

// Key Array
uint8_t key[16];

// Round Key
unsigned int w[44];

// S-Box
uint8_t S_box[16][16] = {
    {0x63, 0x7C, 0x77, 0x7B,
     0xF2, 0x6B, 0x6F, 0xC5,
     0x30, 0x01, 0x67, 0x2B,
     0xFE, 0xD7, 0xAB, 0x76},

    {0xCA, 0x82, 0xC9, 0x7D,
     0xFA, 0x59, 0x47, 0xF0,
     0xAD, 0xD4, 0xA2, 0xAF,
     0x9C, 0xA4, 0x72, 0xC0},

    {0xB7, 0xFD, 0x93, 0x26,
     0x36, 0x3F, 0xF7, 0xCC,
     0x34, 0xA5, 0xE5, 0xF1,
     0x71, 0xD8, 0x31, 0x15},

    {0x04, 0xC7, 0x23, 0xC3,
     0x18, 0x96, 0x05, 0x9A,
     0x07, 0x12, 0x80, 0xE2,
     0xEB, 0x27, 0xB2, 0x75},

    {0x09, 0x83, 0x2C, 0x1A,
     0x1B, 0x6E, 0x5A, 0xA0,
     0x52, 0x3B, 0xD6, 0xB3,
     0x29, 0xE3, 0x2F, 0x84},

    {0x53, 0xD1, 0x00, 0xED,
     0x20, 0xFC, 0xB1, 0x5B,
     0x6A, 0xCB, 0xBE, 0x39,
     0x4A, 0x4C, 0x58, 0xCF},

    {0xD0, 0xEF, 0xAA, 0xFB,
     0x43, 0x4D, 0x33, 0x85,
     0x45, 0xF9, 0x02, 0x7F,
     0x50, 0x3C, 0x9F, 0xA8},

    {0x51, 0xA3, 0x40, 0x8F,
     0x92, 0x9D, 0x38, 0xF5,
     0xBC, 0xB6, 0xDA, 0x21,
     0x10, 0xFF, 0xF3, 0xD2},
    
    {0xCD, 0x0C, 0x13, 0xEC,
     0x5F, 0x97, 0x44, 0x17,
     0xC4, 0xA7, 0x7E, 0x3D,
     0x64, 0x5D, 0x19, 0x73},

    {0x60, 0x81, 0x4F, 0xDC,
     0x22, 0x2A, 0x90, 0x88,
     0x46, 0xEE, 0xB8, 0x14,
     0xDE, 0x5E, 0x0B, 0xDB},

    {0xE0, 0x32, 0x3A, 0x0A,
     0x49, 0x06, 0x24, 0x5C,
     0xC2, 0xD3, 0xAC, 0x62,
     0x91, 0x95, 0xE4, 0x79},

    {0xE7, 0xC8, 0x37, 0x6D,
     0x8D, 0xD5, 0x4E, 0xA9,
     0x6C, 0x56, 0xF4, 0xEA,
     0x65, 0x7A, 0xAE, 0x08},

    {0xBA, 0x78, 0x25, 0x2E,
     0x1C, 0xA6, 0xB4, 0xC6,
     0xE8, 0xDD, 0x74, 0x1F,
     0x4B, 0xBD, 0x8B, 0x8A},

    {0x70, 0x3E, 0xB5, 0x66,
     0x48, 0x03, 0xF6, 0x0E,
     0x61, 0x35, 0x57, 0xB9,
     0x86, 0xC1, 0x1D, 0x9E},

    {0xE1, 0xF8, 0x98, 0x11,
     0x69, 0xD9, 0x8E, 0x94,
     0x9B, 0x1E, 0x87, 0xE9,
     0xCE, 0x55, 0x28, 0xDF},

    {0x8C, 0xA1, 0x89, 0x0D,
     0xBF, 0xE6, 0x42, 0x68,
     0x41, 0x99, 0x2D, 0x0F,
     0xB0, 0x54, 0xBB, 0x16}
};

// Random Column Matrix
unsigned int RCon[10] = {
    0x01000000, 0x02000000, 0x04000000,
    0x08000000, 0x10000000, 0x20000000,
    0x40000000, 0x80000000, 0x1B000000,
    0x36000000
};

// RotWord
unsigned int RotWord(unsigned int word) {
    return (word << 8) | (word >> 24);
}

// SubWord
unsigned int SubWord(unsigned int word) {
    unsigned int ret = 0, tmp;
    for (int i = 0; i < 4; i++) {
        tmp = (unsigned int)(word & (0xFF << (i << 3))) >> (i << 3); // 提取出对应 8 位
        ret |= (unsigned int)(S_box[(tmp & 0xF0) >> 4][tmp & 0x0F]) << (i << 3); // S-box
    }
    return ret;
}

/*
// 调试辅助函数：word2str
void word2str (unsigned int word) {
    char Output[9];
    for (int i = 0; i < 8; i++) {
        char c = (word & (0xF << (i << 2))) >> (i << 2);
        if (c >= 0 && c <= 9) {
            Output[7-i] = c + '0';
        } else if (c >= 10 && c <= 15) {
            Output[7-i] = c - 10 + 'A';
        }
    }
    Output[8] = '\0';
    cout << Output;
}
*/

// Key Expansion
void KeyExpansion() {
    int i = 0;
    for (; i < 4; i++) {
        w[i] = key[4*i + 3]; // & 0xFF
        w[i] |= (key[4*i] << 24);
        w[i] |= (key[4*i + 1] << 16);
        w[i] |= (key[4*i + 2] << 8);
    }
    unsigned tmp;
    for (i = 4; i < 44; i++) {
        tmp = w[i-1];
        if ((i & 0x3) == 0) { // i % 4 == 0
            tmp = SubWord(RotWord(tmp)) ^ RCon[(i >> 2) - 1];
        }
        w[i] = w[i-4] ^ tmp;
    }
}

void SubBytes() {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            state[i][j] = S_box[(state[i][j] & 0xF0) >> 4][state[i][j] & 0x0F];
        }
    }
}

void ShiftRows() {
    uint8_t t = state[1][0];
    state[1][0] = state[1][1];
    state[1][1] = state[1][2];
    state[1][2] = state[1][3];
    state[1][3] = t;

    t = state[2][0];
    state[2][0] = state[2][2];
    state[2][2] = t;
    t = state[2][1];
    state[2][1] = state[2][3];
    state[2][3] = t;

    t = state[3][0];
    state[3][0] = state[3][3];
    state[3][3] = state[3][2];
    state[3][2] = state[3][1];
    state[3][1] = t;
}

uint8_t FieldMult_x(uint8_t a) {
    uint8_t ret = a << 1;
    if (a & 0x80) {
        ret ^= 0x1B;
    }
    return ret;
}

void MixColumn(int c) {
    uint8_t a[4];
    for (int i = 0; i < 4; i++) {
        a[i] = state[i][c]; // 备份
    }
    state[0][c] = FieldMult_x(a[0]) ^ FieldMult_x(a[1]) ^ a[1] ^ a[2] ^ a[3];
    state[1][c] = FieldMult_x(a[1]) ^ FieldMult_x(a[2]) ^ a[2] ^ a[3] ^ a[0];
    state[2][c] = FieldMult_x(a[2]) ^ FieldMult_x(a[3]) ^ a[3] ^ a[0] ^ a[1];
    state[3][c] = FieldMult_x(a[3]) ^ FieldMult_x(a[0]) ^ a[0] ^ a[1] ^ a[2];
}

void AddRoundKey(int round) {
    for (int i = 0; i < 4; i++) {
        state[0][i] ^= (w[round*4 + i] >> 24);
        state[1][i] ^= (w[round*4 + i] >> 16) & 0xFF;
        state[2][i] ^= (w[round*4 + i] >> 8) & 0xFF;
        state[3][i] ^= w[round*4 + i] & 0xFF;
    }
}

void hex2key(char key_str[]) { // 十六进制字符串转换为 16 字节的 key
    char c;
    for (int i = 0; i < 16; i++) {
        int idx = 2 * i;
        c = key_str[idx];
        if (c >= '0' && c <= '9') {
            key[i] = (c - '0');
        } else if (c >= 'a' && c <= 'f') {
            key[i] = (c - 'a' + 10);
        }

        key[i] <<= 4; // 高 4 位
        idx++;
        c = key_str[idx];
        if (c >= '0' && c <= '9') {
            key[i] |= (c - '0');
        } else if (c >= 'a' && c <= 'f') {
            key[i] |= (c - 'a' + 10);
        }
    }
}

void hex2plaintext(char x[]) { // 十六进制字符串转换为 16 字节的明文，初始化 state
    char c;
    for (int j = 0; j < 4; j++) {
        for (int i = 0; i < 4; i++) {
            int idx = 2 * (4 * j + i);
            c = x[idx];
            if (c >= '0' && c <= '9') {
                state[i][j] = (c - '0');
            } else if (c >= 'a' && c <= 'f') {
                state[i][j] = (c - 'a' + 10);
            }

            state[i][j] <<= 4; // 高 4 位
            idx++;
            c = x[idx];
            if (c >= '0' && c <= '9') {
                state[i][j] |= (c - '0');
            } else if (c >= 'a' && c <= 'f') {
                state[i][j] |= (c - 'a' + 10);
            }
        }
    }
}

void Output_State() { // 输出 state
    char Output[33];
    for (int j = 0; j < 4; j++)
        for (int i = 0; i < 4; i++) {
            int idx = 2 * (4 * j + i);
            char c = (state[i][j] & 0xF0) >> 4;
            if (c >= 0 && c <= 9) {
                Output[idx] = c + '0';
            } else if (c >= 10 && c <= 15) {
                Output[idx] = c - 10 + 'A';
            }
            c = state[i][j] & 0x0F;
            if (c >= 0 && c <= 9) {
                Output[idx+1] = c + '0';
            } else if (c >= 10 && c <= 15) {
                Output[idx+1] = c - 10 + 'A';
            }
        }
    
    Output[32] = '\0';
    cout << Output;
}


int main() {
    char key_str[33], x[33];
    cin >> key_str;
    hex2key(key_str); // Convert key string to key array
    cin >> x;
    hex2plaintext(x); // Convert plaintext string to state matrix

    KeyExpansion(); // Expand Key

    // Add Round Key
    AddRoundKey(0);

    for (int i = 1; i < 10; i++) {
        SubBytes(); // SubBytes
        ShiftRows(); // ShiftRows
        for (int j = 0; j < 4; j++) {
            MixColumn(j); // MixColumn
        }
        AddRoundKey(i); // AddRoundKey
    }

    SubBytes(); // SubBytes
    ShiftRows(); // ShiftRows
    AddRoundKey(10); // AddRoundKey

    // Output
    Output_State();

    return 0;
}
