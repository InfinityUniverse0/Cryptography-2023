# 利用 Kasiski 检测法猜测 Vigenere 密码的密钥长度

import re # 正则表达式
from collections import Counter

def get_gcd(a, b):
    # 求 a 和 b 的最大公因子

    if a < b:
        a, b = b, a
    while b != 0:
        a, b = b, a % b
    return a

def kasiski_guess(cipher_text):
    # 搜索长度为 3 的相同的密文，计算其间距
    
    # 去除密文中的空格
    cipher_text = re.sub(r'\s', '', cipher_text)

    # 搜索长度为 3 的相同的密文，计算其间距
    same_cipher_text = []
    same_cipher_text_length = []
    for i in range(len(cipher_text) - 3):
        pos = cipher_text.find(cipher_text[i:i + 3], i + 3)
        if pos != -1:
            same_cipher_text.append(cipher_text[i:i + 3])
            same_cipher_text_length.append(pos - i)
    
    # 打印信息
    print('长度为 3 的相同的密文为：')
    print(same_cipher_text)
    print('其间距为：')
    print(same_cipher_text_length)
    
    # 找出重复最多的长度为 3 的密文
    same_cipher_text_most_common = Counter(same_cipher_text).most_common(1)[0][0]

    counter = []
    for i, s in enumerate(same_cipher_text):
        if s == same_cipher_text_most_common:
            counter.append(same_cipher_text_length[i])

    # 求最大公因子
    gcd = counter[0]
    for i in range(1, len(counter)):
        gcd = get_gcd(gcd, counter[i])
    
    # 最大公因子很可能是密钥长度
    return gcd
