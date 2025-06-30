There are exist **few models** of paying commission:
1. **Payment by volume received.**
2. **Payment by third currency.**

## Payment by volume received

1. **Step #1**: Buy [[base asset]] volume -> pay fee from that volume. So received volume calculates like:
	$$\begin{cases}
	V'_{b} = C \frac{1}{B_{b}}(1-f),\\
	f'_{q}=V'_{b}B_{b}f=Cf, \\
	V'_{b}+f'_{q}=C
	\end{cases}$$
	where:
	$V'_{b}$ - volume at this stage in [[base asset]],
	$C$ - initial [[Quote asset|quote]] capital,
	$B_{b}$ - base buy price in [[quote asset]],
	$f$ - [[Trading platform fee|fee]], 
	$f'_{q}$ - fee payed on this stage in [[Quote asset|quote]].
	
```ad-note
title: correctness check
collapse:
$$\begin{aligned}
V'_{b}B_{b}+f'_{q}&=\\C \frac{1}{B_{b}}(1-f)B_{b}+Cf&=\\C((1-f)+f)&=C\end{aligned}$$
```
	
2. **Steps #2**: Sell [[base asset]] and receive [[Quote asset|quote]] volume -> pay fee from that. Received volume:
	$$\begin{cases}
	V_{q} = V'_{b}B_{s}(1-f),\\
	f_{q}=V'_{b}B_{s}f, \\
	V_{q}+f_{q}=V'_{b}B_{s}
	\end{cases}
	\Rightarrow
	\begin{cases}
	V_{q} = C \frac{B_{s}}{B_{b}}(1-f)^2,\\
	f_{q} =C \frac{B_{s}}{B_{b}}(1-f)f, \\
	V_{q}+f_{q}=V'_{b}B_{s}
	\end{cases}$$
	$$V_{q}=C \frac{B_{s}}{B_{b}}(1-f)^2$$
	where
	$V_{q}$ - volume at this stage in [[quote asset]],
	$B_{s}$ - [[Base asset|base]] sell price in [[quote asset]],
	$f_{q}$ - fee payed on this stage in [[Quote asset|quote]].
```ad-note
title: correctness check
collapse:
$$\begin{aligned}
V_{q}+f_{q}&=\\C \frac{B_{s}}{B_{b}}(1-f)^2+C \frac{B_{s}}{B_{b}}(1-f)f&=\\C \frac{B_{s}}{B_{b}}((1-f)^2+(1-f)f)&=\\ C \frac{B_{s}}{B_{b}}(1-2f+f^2+f-f^2)&=\\ C \frac{B_{s}}{B_{b}}(1-f)&=V'_{b}B_{s}
\end{aligned}$$
```

## Payment by third currency

*Note: for simplicity assume that third currency exchange rate is always the same, so exchange a -> third -> b, is the same as exchange a -> b*

1. **Step #1**: Buy [[base asset]] volume -> pay fee from the third currency. So received volume calculates like:
	$$\begin{cases}
	V'_{b} = C \frac{1}{B_{b}},\\
	V'_{tq} = C_{tq} - f'_{q}, \\
	f'_{q}=V'_bB_{b}f=Cf, \\
	V'_{b}B_{b}+V'_{tq}+f'_{q}=C+C_{tq}
	\end{cases}$$
	where
	$V'_{b}$ - volume at this stage in [[base asset]],
	$V'_{tq}$ - third currency volume at this stage in [[quote asset]],
	$C$ - initial [[Quote asset|quote]] capital,
	$C_{tq}$ - initial third currency capital in [[quote asset]],
	$B_{b}$ - base buy price in [[quote asset]],
	$f$ - [[Trading platform fee|fee]], 
	$f'_{q}$ - fee payed on this stage in [[Quote asset|quote]].
	 
```ad-note
title: correctness check
collapse:
$$\begin{aligned}
	V'_{b}B_{b}+V'_{tq}+f'_{q} &=\\
	C \frac{1}{B_{b}}B_{b} + C_{tq} - f'_{q} + C \frac{1}{B_{b}}f &=\\
	C \frac{1}{B_{b}}B_{b} + C_{tq} - C \frac{1}{B_{b}}f + C \frac{1}{B_{b}}f &=C+C_{tq}
	\end{aligned}$$
```
2. **Steps #2**: Sell [[base asset]] and receive [[Quote asset|quote]] volume -> pay fee from the third currency. So received volume calculates like:
	$$\begin{cases}
	V_{q} = V'_{b}B_{s},\\
	f_{q}=V_{q}f,\\
	V_{tq}=V'_{tq}-f_q,\\
	V_{q}+V_{tq}+f_{q}=V'_{b}B_{s}+V'_{tq}
	\end{cases}
	\Rightarrow
	\begin{cases}
	V_{q} = C \frac{B_{s}}{B_{b}},\\
	f_{q}=V_{q}f,\\
	V_{tq}=C_{tq} - f'_{q}-f_q,\\
	V_{q}+V_{tq}+f_{q}=V'_{b}B_{s}+V'_{tq}
	\end{cases}$$
	 $$V_{q}+V_{tq}=C (\frac{B_{s}}{B_{b}}(1-f)- f)+C_{tq}$$
```ad-note
title: correctness check
collapse:
$$\begin{aligned}
V_{q}+V_{tq}+f_{q} &=\\
C (\frac{B_{s}}{B_{b}}(1-f)- f)+C_{tq}+C\frac{B_{s}}{B_{b}}f &=\\
C (\frac{B_{s}}{B_{b}}-\frac{B_{s}}{B_{b}}f- f+\frac{B_{s}}{B_{b}}f)+C_{tq}&=\\
C (\frac{B_{s}}{B_{b}}- f)+C_{tq}&=\\
C \frac{1}{B_{b}}B_{s}+C_{tq}- Cf&=V'_bB_s+V'_{tq}
\end{aligned}
$$
```