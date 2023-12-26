# 采用重合指数法，破译维吉尼亚密码

import re # 正则表达式
from guess_m import * # 导入 guess_m.py 中的 kasiski_guess() 和 get_gcd() 函数

# 字母频率表
letter_frequency_table = {
    # 大写字母
    'A': 0.082,
    'B': 0.015,
    'C': 0.028,
    'D': 0.043,
    'E': 0.127,
    'F': 0.022,
    'G': 0.020,
    'H': 0.061,
    'I': 0.070,
    'J': 0.002,
    'K': 0.008,
    'L': 0.040,
    'M': 0.024,
    'N': 0.067,
    'O': 0.075,
    'P': 0.019,
    'Q': 0.001,
    'R': 0.060,
    'S': 0.063,
    'T': 0.091,
    'U': 0.028,
    'V': 0.010,
    'W': 0.023,
    'X': 0.001,
    'Y': 0.020,
    'Z': 0.001
}

def shift_letter(letter, shift):
    '''
    字母移位
    :param letter: 字母
    :param shift: 移位数
    :return 移位后的字母
    '''

    # 大写字母
    if letter.isupper():
        letter = chr((ord(letter) - ord('A') + shift) % 26 + ord('A'))
    # 小写字母
    elif letter.islower():
        letter = chr((ord(letter) - ord('a') + shift) % 26 + ord('a'))
    # 非字母
    else:
        pass
    
    return letter

def index_of_coincidence_method(m, cipher_text):
    '''
    重合指数法
    :param m: 密钥长度
    :param cipher_text: 密文
    :return 明文, 密钥
    '''

    # 过滤密文空白
    cipher_text = re.sub(r'\s', '', cipher_text)

    # 分组
    cipher_text_group = []
    for i in range(m):
        cipher_text_group.append(cipher_text[i::m])
    
    # 计算每一组的重合指数，以进一步辅助判断密钥长度 m 的正确性
    index_of_coincidence_group = []
    letter_count_group = []
    for i in range(m):
        # 计算每个字母出现的次数
        letter_count = {}
        for letter in cipher_text_group[i]:
            if letter in letter_count:
                letter_count[letter] += 1
            else:
                letter_count[letter] = 1
        letter_count_group.append(letter_count)
        
        # 计算重合指数
        cipher_text_length = len(cipher_text_group[i])
        index_of_coincidence = 0
        for letter in letter_count:
            index_of_coincidence += letter_count[letter] * (letter_count[letter] - 1)
        index_of_coincidence /= cipher_text_length * (cipher_text_length - 1)
        index_of_coincidence_group.append(index_of_coincidence)
    
    # 打印每一组的重合指数
    print('每一组的重合指数为：')
    for i in range(m):
        print('第 %d 组：' % (i + 1), end='')
        print(cipher_text_group[i], end=' ')
        print(index_of_coincidence_group[i])
    
    # 猜测每一组的密钥
    key_group = []
    for i, letter_count in enumerate(letter_count_group):
        M_max = 0
        key = None
        for g in range(26):
            # 计算M_g
            cipher_text_length = len(cipher_text_group[i])
            M = 0
            for letter in letter_frequency_table:
                try:
                    M += letter_frequency_table[letter] * letter_count[shift_letter(letter, g)]
                except:
                    # 此时出现频数为零的情况
                    pass
            M /= cipher_text_length
            
            # 找出最大的 M_g
            if M > M_max:
                M_max = M
                key = g
        key_group.append(key)
    
    # 解密
    plain_text = ''
    for i in range(len(cipher_text)):
        plain_text += shift_letter(cipher_text[i], -key_group[i % m]) # 解密：移位数取反
    plain_text = plain_text.lower()

    return plain_text, key_group


if __name__ == '__main__':
    cipher_text = """
    KCCPKBGUFDPHQTYAVINRRTMVGRKDNBVFDETDGILTXRGUD
    DKOTFMBPVGEGLTGCKQRACQCWDNAWCRXIZAKFTLEWRPTYC
    QKYVXCHKFTPONCQQRHJVAJUWETMCMSPKQDYHJVDAHCTRL
    SVSKCGCZQQDZXGSFRLSWCWSJTBHAFSIASPRJAHKJRJUMV
    GKMITZHFPDISPZLVLGWTFPLKKEBDPGCEBSHCTJRWXBAFS
    PEZQNRWXCVYCGAONWDDKACKAWBBIKFTIOVKCGGHJVLNHI
    FFSQESVYCLACNVRWBBIREPBBVFEXOSCDYGZWPFDTKFQIY
    CWHJVLNHIQIBTKHJVNPIST
    """

    # 猜测密钥长度
    m = kasiski_guess(cipher_text)
    print('密钥长度为：{}'.format(m))

    # 重合指数法破译密钥并解密
    plain_text, key_group = index_of_coincidence_method(m, cipher_text)
    print('密钥为：')
    print(key_group)
    print('明文为：')
    print(plain_text)

# ------Output------
'''
长度为 3 的相同的密文为：
['MVG', 'BVF', 'DDK', 'KFT', 'KFT', 'HJV', 'HJV', 'HCT', 'RLS', 'KCG', 'AFS', 'RWX', 'VYC', 'WBB', 'BBI', 'HJV', 'JVL', 'VLN', 'LNH', 'NHI', 'HJV']
其间距为：
[156, 264, 198, 18, 156, 18, 138, 84, 18, 121, 60, 12, 42, 36, 36, 54, 54, 54, 54, 54, 12]
密钥长度为：6
每一组的重合指数为：
第 1 组：KGQNGVGGTGCQWAWQHNJEPJTKQFWAPJGHPWKCTAQVNCIVJFVNIVCPQJQJT 0.06265664160401002
第 2 组：CUTRRFIUFEKCCKRKKCVTKVRCDRSFRRKFZTEEJFNYWKKKVFYVRFDFIVIV 0.08376623376623377
第 3 组：CFYRKDLDMGQWRFPYFQAMQDLGZLJSJJMPLFBBRSRCDAFCLSCREEYDYLBN 0.04935064935064935
第 4 组：PDATDETDBLRDXTTVTQJCDASCXSTIAUIDVPDSWPWGDWTGNQLWPXGTCNTP 0.06493506493506493
第 5 组：KPVMNTXKPTANILYXPRUMYHVZGWBAHMTILLPHXEXAKBIGHEABBOZKWHKI 0.04285714285714286
第 6 组：BHIVBDROVGCAZECCOHWSHCSQSCHSKVZSGKGCBZCOABOHISCBBSWFHIHS 0.07337662337662337
密钥为：
[2, 17, 24, 15, 19, 14]
明文为：
ilearnedhowtocalculatetheamountofpaperneededforaroomwheniwasatschoolyoumultiplythesquare
footageofthewallsbythecubiccontentsofthefloorandceilingcombinedanddoubleityouthenallowha
lfthetotalforopeningssuchaswindowsanddoorsthenyouallowtheotherhalfformatchingthepatternt
henyoudoublethewholethingagaintogiveamarginoferrorandthenyouorderthepaper
'''
