// 计算素数域中逆元
#include <iostream>
#include <vector>
using namespace std;

int Inv(int a, int p) {
	/*
	* 求 a 模 p 的乘法逆元
	* return: a^(-1) (mod p)
	*/
	vector<int> r; // 余数序列
    r.push_back(p);
    r.push_back(a);
	vector<int> q; // 商序列
	q.push_back(-1); // q[0]中的值无效
	vector<int> s;
	s.push_back(1);
	s.push_back(0);
	vector<int> t;
	t.push_back(0);
	t.push_back(1);

	int x = 0; // 索引
	while (r[x] % r[x + 1]) { // 余数非零，则循环
		r.push_back(r[x] % r[x + 1]);
		q.push_back(r[x] / r[x + 1]);
		s.push_back(s[x] - s[x + 1] * q[x + 1]);
		t.push_back(t[x] - t[x + 1] * q[x + 1]);
		x++;
	}

	int l = r.size() - 1; // 序列的末尾元素下标
	if (r[l] == 1) { // 可用扩展欧几里得算法求逆元(乘法逆元存在)
		// 转为最小正缩系中
        if (s[l] < 0)
            s[l] = a + s[l];
        if (t[l] < 0)
            t[l] = p + t[l];
        return t[l];
	}
	return 0; // 实则不需要
}

int main() {
    int a, p;
    cin >> a >> p;
    cout << Inv(a, p);
    return 0;
}
