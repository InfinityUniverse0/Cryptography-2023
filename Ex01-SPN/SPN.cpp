// SPN Encryption
#include<iostream>
#include<cstring>
using namespace std;

class SPN {
    int l, m; // l: 每组 l 个 bit, m: 共 m 组
    int Nr; // 加密轮数
    int *S_box; // S-Box 长度 2^l
    int *P_box; // P-Box 长度 l*m
    char* x; // 明文
    char* K; // 密钥
    char* y; // 密文
public:
    SPN(int _l, int _m, int _Nr);
    void Encryption(const char* _x, const char* _K);
    void show_cipher();
    ~SPN();
};

SPN::SPN(int _l = 4, int _m = 4, int _Nr = 4) : l(_l), m(_m), Nr(_Nr), x(nullptr), y(nullptr), K(nullptr) {
    S_box = new int[16] {0xE, 0x4, 0xD, 0x1, 0x2, 0xF, 0xB, 0x8, 0x3, 0xA, 0x6, 0xC, 0x5, 0x9, 0x0, 0x7};
    P_box = new int[16] {1, 5, 9, 13, 2, 6, 10, 14, 3, 7, 11, 15, 4, 8, 12, 16};
}

SPN::~SPN() {
    if (S_box) delete[] S_box;
    if (P_box) delete[] P_box;
    if (x) delete[] x;
    if (y) delete[] y;
    if (K) delete[] K;
}

void SPN::Encryption(const char* _x, const char* _K) {
    if (x) delete[] x;
    if (y) delete[] y;
    if (K) delete[] K;
    x = new char[l*m + 1];
    y = new char[l*m + 1];
    K = new char[l * (m + Nr) + 1];

    strcpy(x, _x);
    x[l*m] = '\0';
    strcpy(K, _K);
    K[l * (m + Nr)] = '\0';

    char* w = new char[l*m + 1];
    strcpy(w, x);
    w[l*m] = '\0';

    // 轮密钥
    char* K_r = new char[l * m + 1];
    for (int r = 0; r < Nr - 1; r++) {
        // 生成轮密钥
        for (int i = 0; i < l*m; i++) {
            K_r[i] = K[r * l + i];
        }
        K_r[l*m] = '\0';
        // 轮密钥异或
        for (int i = 0; i < l*m; i++) {
            if (w[i] == K_r[i])
                w[i] = '0';
            else
                w[i] = '1';
        }
        // S-Box
        for (int i = 0; i < m; i++) {
            int index = 0;
            for (int j = 0; j < l; j++) {
                index = index * 2 + w[i*l + j] - '0';
            }
            int t = S_box[index];
            for (int j = l - 1; j >= 0; j--) {
                if (t % 2)
                    w[i*l + j] = '1';
                else
                    w[i*l + j] = '0';
                t = t >> 1;
            }
        }
        // P-Box
        char* _w = new char[l*m + 1];
        strcpy(_w, w);
        _w[l*m] = '\0';
        for (int i = 0; i < l*m; i++) {
            w[i] = _w[P_box[i] - 1];
        }
        delete[] _w;
    }

    // 生成轮密钥
    for (int i = 0; i < l*m; i++) {
        K_r[i] = K[l * (Nr-1) + i];
    }
    K_r[l*m] = '\0';
    // 轮密钥异或
    for (int i = 0; i < l*m; i++) {
        if (w[i] == K_r[i])
            w[i] = '0';
        else
            w[i] = '1';
    }
    // S-Box
    for (int i = 0; i < m; i++) {
        int index = 0;
        for (int j = 0; j < l; j++) {
            index = index * 2 + w[l*i +j] - '0';
        }
        int t = S_box[index];
        for (int j = l - 1; j >= 0; j--) {
            if (t % 2)
                w[l*i + j] = '1';
            else
                w[l*i + j] = '0';
            t = t >> 1;
        }
    }
    // 最后一轮 轮密钥
    for (int i = 0; i < l*m; i++) {
        K_r[i] = K[l*Nr + i];
    }
    K_r[l*m] = '\0';
    // 轮密钥异或
    for (int i = 0; i < l*m; i++) {
        if (w[i] == K_r[i])
            w[i] = '0';
        else
            w[i] = '1';
    }
    // 密文
    strcpy(y, w);
    y[l*m] = '\0';

    // delete
    delete[] w;
    delete[] K_r;
}

void SPN::show_cipher() {
    cout << y;
}


int main() {
    SPN Net;
    char* x = new char[16 + 1];
    char* K = new char[32 + 1];
    cin >> x;
    cin >> K;
    Net.Encryption(x, K);
    Net.show_cipher();
    delete[] x;
    delete[] K;
    return 0;
}