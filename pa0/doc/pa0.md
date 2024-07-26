# 矩阵绘制工具

# https://www.latexlive.com/home

# 在线草图工具

# https://tilipa.zlsam.com/handdraw/

# 旋转矩阵的推演

<img src="P 绕原点旋转θ角度得到P'.png" alt="示例图片" width="1000">

## 如上图所示

### 1. P (1,0) 绕圆点旋转 θ 角度,得到 P1(cosθ,sinθ)

### 2. P2(0,1) 绕圆点旋转 θ 角度,得到 P3(-sinθ,cosθ)

### 基于矩阵的计算公式:

<font size=100>$\begin{Bmatrix}a&b\\c&d\end{Bmatrix}*\begin{Bmatrix}x\\y\end{Bmatrix}=\begin{Bmatrix}ax+by\\cx+dy\end{Bmatrix}$</font>

### 基于 1 我们将 P(1,0) 带入公式

-   ## a+b\*0=cosθ => a=cosθ

-   ## c+d\*0=sinθ => c=sinθ

### 基于 2 我们将 P2(0,1) 带入公式

-   ## a\*0+b=-sinθ => b=-sinθ

-   ## c\*0+d=cosθ => d=cosθ

### 综上得出旋转矩阵为:(该旋转矩阵重点记忆,推导三维旋转矩阵的基础)

<font size=100>$\begin{Bmatrix}cosθ&-sinθ \\sinθ  &cosθ\end{Bmatrix}$</font>

### 平移矩阵:
<font size=100>$\begin{Bmatrix} 1 & 0 &Δx \\ 0 & 1 &Δy  \\ 0 & 0  & 1\end{Bmatrix}* \begin{Bmatrix} x\\ y\\1\end{Bmatrix}=\begin{Bmatrix} x+Δx\\ y+Δy\\1\end{Bmatrix}$
</font>


### 先平移后旋转 (矩阵变换操作从右边往左边看)

<font size=10>$\begin{Bmatrix} cosθ &-sinθ  &0 \\ sinθ & cosθ & 0\\  0& 0 &1\end{Bmatrix}*\begin{Bmatrix} 1 & 0 &Δx \\ 0 & 1 &Δy  \\ 0 & 0  & 1\end{Bmatrix}=\begin{Bmatrix} cosθ &-sinθ  &cosθ*Δx- sinθ*Δx \\ sinθ & cosθ &sinθ*Δy+ cosθ*Δy \\  0& 0 &1\end{Bmatrix}$
</font>

### 先旋转后平移 (矩阵变换操作从右边往左边看)
<font size=10> $\begin{Bmatrix} 1 & 0 &Δx \\ 0 & 1 &Δy  \\ 0 & 0  & 1\end{Bmatrix}* \begin{Bmatrix} cosθ &-sinθ  &0 \\ sinθ & cosθ & 0\\  0& 0 &1\end{Bmatrix}=\begin{Bmatrix} cosθ &-sinθ  &Δx \\ sinθ & cosθ &Δy \\  0& 0 &1\end{Bmatrix}$
</font>

![alt text](image.png)