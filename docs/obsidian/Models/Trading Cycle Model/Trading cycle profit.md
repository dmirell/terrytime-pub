Profit is difference in [[Trading cycle capital|capital]] before and after sell.
It calculates differently for different [[Commission calculation|commission calculation models]].
$$D = C-C'$$
where:
$D$ - profit,
$C'$ - capital before [[Full Trading Cycle Model|trading cycle]],
$C$ - capital after trading cycle.

It also may be defined through the [[capital multiplier]]:
$$
D = C \cdot (M - 1)
$$
## 1. Payment by volume received

Final [[Quote asset|quote]] volume estimates as:
$$V_{q}=C \frac{B_{s}}{B_{b}}(1-f)^2$$
So profit is:
$$
\begin{aligned}
D = C \frac{B_{s}}{B_{b}}(1-f)^2 - C &=\\
C (\frac{B_{s}}{B_{b}}(1-f)^2 - 1)
\end{aligned}
$$

## 2. Payment by third currency

Final [[Quote asset|quote]] volume estimates as:
$$V_{q}+V_{tq}=C (\frac{B_{s}}{B_{b}}(1-f)- f)+C_{tq}$$
Profit is:
$$
\begin{aligned}
D =C (\frac{B_{s}}{B_{b}}(1-f)- f)+C_{tq}-(C+C_{tq}) &= \\
C(\frac{B_{s}}{B_{b}}(1-f)- f)-C &=\\
C((\frac{B_{s}}{B_{b}}(1-f)- f)-1)
\end{aligned}$$
