# Simplified ECDSA

设$p$是一个大素数，$\mathrm{E}$是定义在$\mathbb{F}_{p}$上的椭圆曲线。设$A=\left(x_{0}, y_{0}\right)$是$\mathrm{E}$上阶数为$q$（$q$为素数）的一个点（非无穷远点）。对于$K=(p, q, E, A, m, B)$，$B=mA$。其中$m \in Z_{q}$是私钥，其他参数是公开信息。

于是我们有如下的椭圆曲线数字签名算法：设选定秘密随机数$k \in Z_{q}^{*}$，对于给定的消息$x$，定义$\operatorname{sig}_{K}(x, k)=(r, s)$，其中$kA=(u, v)$，$r=u \bmod q$，以及$s=k^{-1}(\operatorname{Hash}(x)+mr) \bmod q$。（如果$r=0$或$s=0$，重新选取$k$）

这里我们考虑椭圆曲线定义在$Z_{11}$上的$y^{2}=x^{3}+x+6$，注意到该椭圆曲线加法群阶数$q=13$。定义哈希函数$\operatorname{Hash}(x)=2^{x} \bmod q$，其中消息$x \in Z_{q-1}$。于是请你计算给定消息的数字签名。

**Input:**
 $x_{0}, y_{0}$  // 给定椭圆曲线加法群中的非无穷远点 $A$ 
 $m, x, k$  // private key $m \in Z_{q}$ ; message $x \in Z_{q-1}$ ; random number $k \in Z_{q}^{*}$ 
**Output:**
 $r, s$ 
