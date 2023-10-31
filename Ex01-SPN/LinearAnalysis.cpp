// SPN Linear Analysis
#include <iostream>
#include <stdlib.h>
#include <fstream>
using namespace std;

// abs 宏用于求绝对值
#define abs(a) ((a)>0?(a):(-(a)))
// extract 宏用于提取某个 bit
#define extract(x, n) (((x) & (1 << ((n) - 1))) != 0)

uint8_t S_box[16] = {0xE, 0x4, 0xD, 0x1, 0x2, 0xF, 0xB, 0x8, 0x3, 0xA, 0x6, 0xC, 0x5, 0x9, 0x0, 0x7};
uint8_t P_box[16] = {1, 5, 9, 13, 2, 6, 10, 14, 3, 7, 11, 15, 4, 8, 12, 16};

// S-Box-Inverse
uint8_t S_box_Inverse[16] = {0xE, 0x3, 0x4, 0x8, 0x1, 0xC, 0xA, 0xF, 0x7, 0xD, 0x9, 0x6, 0xB, 0x2, 0x0, 0x5};

const char K_global[] = "00111010100101001101011000111111"; // 密钥


unsigned int SPN_GenPair(unsigned int K) {
    unsigned short x, y;
    x = rand() % (1 << 16); // 随机生成明文
    unsigned short x_ = x;
    unsigned short v = 0;
    for (int i = 0; i < 3; i++) {
        x ^= ((unsigned int)(K << (4 * i)) >> 16); // 轮密钥异或
        v = 0;
        for (int j = 0; j < 4; j++) { // S-Box
            v |= (S_box[(x >> (4 * j)) & 0xF] << (4 * j));
        }
        x = 0;
        for (int j = 0; j < 16; j++) { // P-Box
            x |= ((extract(v, (16 - j))) << (16 - P_box[j]));
        }
    }
    x ^= ((unsigned int)(K << (4 * 3)) >> 16); // 轮密钥异或
    v = 0;
    for (int j = 0; j < 4; j++) { // S-Box
        v |= (S_box[(x >> (4 * j)) & 0xF] << (4 * j));
    }
    y = x = v ^ (K & (0xFFFF));
    unsigned int xy = y | (x_ << 16); // x, y
    return xy;
}


// 线性密码分析
uint8_t LinearAnalysis(int T) {
    /*
     * SPN 线性密码分析
     * 使用 uint8_t (即unsigned char)，并采用位运算加速
     * @param T: T 对 (PlainText, CipherText) Pairs
     * @return maxkey: 该攻击算法确定出的最大可能的 K5 轮密钥的 8 个子密钥比特
     */
    uint8_t maxkey; // 返回值
    int count[(1 << 8)] = {0}; // 计数器，初始化为 0
    unsigned short x, y; // 明密文对
    unsigned int xy, K = 0;

    // 密钥：字符串转为 unsigned int
    for (int i = 0; i < 32; i++)
        K = 2 * K + K_global[i] - '0';
    
    int _ = 0; // 用于控制循环次数
    for (int i = 0; i < T; i++) {
        // 获取明密文对
        xy = SPN_GenPair(K);
        // 分解明密文对
        x = xy >> 16;
        y = xy & 0xFFFF;

        _ = 0;
        for (uint8_t L = 0; _ <= 0xFF; L++) {
            _++;
            uint8_t t = ((y & 0x0F00) >> 4); // y_<2>
            t |= (y & 0x0F); // y_<2>, y_<4>
            t ^= L; // 异或
            t = (S_box_Inverse[((t & 0xF0) >> 4)] << 4) | (S_box_Inverse[(t & 0x0F)]); // S-Box Inverse
            uint8_t z = (extract(x, 9)) ^ (extract(x, 10)) ^ (extract(x, 12)); // x_5 ^ x_7 ^ x_8
            z ^= (extract(t, 1)) ^ (extract(t, 3)) ^ (extract(t, 5)) ^ (extract(t, 7));
            if (!z)
                count[L] += 1;
        }
    }
    int max = -1;
    _ = 0;
    for (uint8_t L = 0; _ <= 0xFF; L++) {
        _++;
        count[L] = abs((count[L] - (T / 2)));
        if (count[L] > max) {
            max = count[L];
            maxkey = L;
        }
    }
    return maxkey;
}


int main() {
    uint8_t maxkey = LinearAnalysis(8000);
    // 转换为二进制输出
    char ans[9];
    for (int i = 7; i >= 0; i--) {
        ans[i] = (maxkey & 1) + '0';
        maxkey >>= 1;
    }
    ans[8] = '\0';
    cout << "The 8 bits of K5 round key is: " << ans << endl;
    return 0;
}
