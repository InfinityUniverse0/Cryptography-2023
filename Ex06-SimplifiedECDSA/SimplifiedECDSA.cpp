// Simplified ECDSA
#include <iostream>
using namespace std;

int Inv(int a, int p) {
    /*
    * 求 a 模 p 的乘法逆元
    * return: a^(-1) (mod p)
    */
    int r0 = p, r = a; // 余数序列
    int q0 = -1, q = -1; // 商序列
    int s0 = 1, s = 0;
    int t0 = 0, t = 1;

    while (r0 % r) { // 余数非零，则循环
        int tmp = r0;
        r0 = r;
        r = tmp % r0;

        q0 = q;
        q = tmp / r0;

        tmp = s0;
        s0 = s;
        s = tmp - s0 * q;

        tmp = t0;
        t0 = t;
        t = tmp - t0 * q;
    }

    if (r == 1) { // 可用扩展欧几里得算法求逆元(乘法逆元存在)
        // 转为最小正缩系中
        if (s < 0)
            s += a;
        if (t < 0)
            t += p;
        return t;
    }
    return 0; // 实则不需要
}

int* P_plus_Q(int a, int b, int p, int x1, int y1, int x2, int y2) {
    /*
     * calculate P + Q
     * Args:
     *     a, b, p: parameters of the elliptic curve Ep(a, b)
     *     x1, y1: coordinates of the point P
     *     x2, y2: coordinates of the point Q
     * Return:
     *     P + Q: the coordinates of the point P + Q  (int[0], int[1])
     *          (-1, -1): if P + Q is the point at infinity
     */
    int* R = new int[2];
    x1 %= p;
    y1 %= p;
    x2 %= p;
    y2 %= p;
    if ((x1 == -1) && (y1 == -1)) {
        R[0] = x2;
        R[1] = y2;
        return R;
    }
    if ((x2 == -1) && (y2 == -1)) {
        R[0] = x1;
        R[1] = y1;
        return R;
    }
    
    if ((x1 == x2) && ((y1 + y2 == p) || (y1 + y2 == 0))) { // 互为逆元
        R[0] = R[1] = -1;
        return R;
    }

    // calculate k
    long long k;
    if ((x1 == x2) && (y1 == y2)) {
        k = ((long long)((long long)x1 * (long long)x1)) % p;
        k = (3 * k) % p;
        k = (k + a) % p;
        k = (long long)(k * (long long)Inv((long long)(2 * (long long)y1) % p, p)) % p;
    }
    else {
        k = y2 - y1;
        if (k < 0)
            k += p;
        int a = x2 - x1;
        if (a < 0)
            a += p;
        k = (long long)(k * (long long)Inv(a, p)) % p;
    }

    // calculate (x3, y3)
    long long x3, y3;
    x3 = (long long)(k * k) % p;
    x3 -= x1;
    if (x3 < 0)
        x3 += p;
    x3 -= x2;
    if (x3 < 0)
        x3 += p;
    
    y3 = x1 - x3;
    if (y3 < 0)
        y3 += p;
    y3 = (long long)(k * (long long)y3) % p;
    y3 -= y1;
    if (y3 < 0)
        y3 += p;
    
    R[0] = (int)x3;
    R[1] = (int)y3;
    return R;
}

int* kP(int a, int b, int p, int x, int y, int k) {
    /*
     * calculate kP
     * Args:
     *     a, b, p: parameters of the elliptic curve Ep(a, b)
     *     x, y: coordinates of the point P
     *     k: the integer k (non-negative)
     * Return:
     *     kP: the coordinates of the point kP  (int[0], int[1])
     *          (-1, -1): if kP is the point at infinity
     */
    int* R = new int[2];
    if ((k == 0) || (x == -1 && y == -1)) {
        // 无穷远点
        R[0] = R[1] = -1;
        return R;
    }

    // extract binary
    int i = 1 << (32 - 2);
    while (!(k & i)) i >>= 1;
    i >>= 1; // ignore the MSB

    // double and add
    R[0] = x;
    R[1] = y;
    while (i) {
        int* tmp = R;
        R = P_plus_Q(a, b , p, R[0], R[1], R[0], R[1]); // double
        delete[] tmp;

        if (k & i) {
            tmp = R;
            R = P_plus_Q(a, b, p, R[0], R[1], x, y); // add
            delete[] tmp;
        }
        i >>= 1;
    }

    return R;
}

int square_and_mul(int a, int m, int n) {
    // a^m (mod n)
    a = a % n;
    int i = (1 << (32-2));
    while ((m & i) == 0) i >>= 1;

    long long ans = 1; // in case of overflow
    while (i) {
        ans = (long long)((ans * ans) % n);
        if (m & i) ans = (long long)((ans * (long long)a) % n);
        i >>= 1;
    }

    return (int)ans;
}

bool ECDSA(int x0, int y0, int m, int x, int k) {
    int p = 11, q = 13, a = 1, b = 6; // parameters defined
    x0 %= p; y0 %= p;
    m %= q; k %= q;
    int* kA = kP(a, b, p, x0, y0, k);
    int r = kA[0] % q;
    int s = ((Inv(k, q) % q) * ((square_and_mul(2, x, q) % q + (m * r) % q) % q) % q);
    // 原则上以下应该需要判断，但是Oj会让你输出 r,s=0 的情况
    // if (r == 0 || s == 0)
    //     return false;
    cout << r << " " << s;
    return true;
}

int main() {
    int x0, y0, m, x, k;
    cin >> x0 >> y0;
    cin >> m >> x >> k;
    ECDSA(x0, y0, m, x, k);
    return 0;
}
