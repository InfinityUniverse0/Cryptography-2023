# Substitution Permutation Network (SPN) & Linear Cryptanalysis

## SPN

See `SPN.cpp` [SPN.cpp](./SPN.cpp).

这里为了 SPN 的**通用性**，采用字符数组来实现。因此牺牲了一定的性能，但是得到了更好的通用性和扩展性。

默认情况为参考教材中的 SPN 结构。

采用位运算操作实现的**高效** SPN 可以参见 [LinearAnalysis.cpp](./LinearAnalysis.cpp)中的部分。关键代码如下：

```C++
// 随机生成一对明-密文对
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
```

## Linear Cryptanalysis

See `LinearAnalysis.cpp` [LinearAnalysis.cpp](./LinearAnalysis.cpp).

文档详见 [2112515-信息安全-武桐西-Ex01.md](./2112515-信息安全-武桐西-Ex01.md)。

SPN 的结构同上。

线性分析的实现参考了教材中的方法和伪代码。

采用了位运算操作，提高线性分析的攻击**效率**。

在这次的[实现](./LinearAnalysis.cpp)中，将上面的 SPN 利用**位运算**重新实现了一遍，算是把坑填上了。
