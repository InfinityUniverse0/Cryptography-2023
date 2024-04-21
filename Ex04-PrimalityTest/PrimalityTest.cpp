// 采用 Miller-Rabin 素性检测算法
#include <iostream>
#include <random>
using namespace std;

long long int square_and_mul(long long int a, long long int m, long long int n) {
    // a^m (mod n)
    a = a % n;
    long long int i = (1LL << (64-2));
    while ((m & i) == 0) i >>= 1;

    __int128 ans = 1; // __int128: in case of overflow
    while (i) {
        ans = (__int128)((ans * ans) % n);
        if (m & i) ans = (__int128)((ans * a) % n);
        i >>= 1;
    }

    return (long long int)ans;
}

bool Miller_Rabin(long long int n, int round = 5) {
    if (n == 2) return true;
    if (n < 2 || (n & 1) == 0) return false;

    long long int m = n - 1;
    long long int k = 0;
    while (m & 1 == 0) {
        m >>= 1;
        k++;
    }

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<long long int> dis(2, n - 1);

    while (round--) { // Loop `round` times
        long long int a = dis(gen); // random init: [2, n-1]
        long long int b = square_and_mul(a, m, n);
        
        if (b == 1) continue;
        for (int i = 0; i < k; i++) {
            if (b == n - 1) {
                b = 1;
                break;
            }
            b = square_and_mul(b, 2, n);
        }
        if (b != 1)
            return false; // Failed Fermat test
    }
    return true;
}


int main() {
    long long int p;
    cin >> p;
    if (Miller_Rabin(p)) cout << "Yes";
    else cout << "No";
    return 0;
}
