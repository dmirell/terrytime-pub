As for first research iteration will be taken [[Simplified Trading Cycle Model]].
The main success factor of the Trading Strategy Model is [[Mathematical Expectation|expectation]] of received [[Trading cycle profit|profit]].
$$\mathbb{E}[D] = (1 - o(\Delta t)) \cdot \mathbb{E}[D_g] + o(\Delta t) \cdot \mathbb{E}[D_l], \quad o(\Delta t) = P(D_{real} < 0)$$where:
$\mathbb{E}[D_g]$ - expected [[gain]] profit,
$\mathbb{E}[D_l]$ - expected [[loss]] profit,
$\Delta t$ - time distance to future,
$D_{real}$ - real observed profit after $\Delta t$,  
$o(\Delta t)$ - probability to get loss deal.

From the [[Trading cycle profit|profit]] definition, profit is $D = M - 1$, so formula also can be rewritten using [[capital multiplier]]:
$$
\mathbb{E}[M] = (1 - o(\Delta t)) \cdot \mathbb{E}[M_g] + o(\Delta t) \cdot \mathbb{E}[M_l], \quad o(\Delta t) = P(M_{real} < 1)
$$
where:
$M$ - capital multiplier.

```ad-note
title: Proof
collapse:
$$
o(\Delta t) = P(D_{real} < 0) = P(M_{real} - 1 < 0) = P(M_{real} < 1).
$$
$$\begin{aligned}
\mathbb{E}[D] &= (1 - o(\Delta t)) \cdot \mathbb{E}[D_g] + o(\Delta t) \cdot \mathbb{E}[D_l] &=\\
\mathbb{E}[M] - 1 &= (1 - o(\Delta t)) \cdot (\mathbb{E}[M_g] - 1) + o(\Delta t) \cdot (\mathbb{E}[M_l] - 1) &=\\
\mathbb{E}[M] - 1 &= \mathbb{E}[M_g] - 1 - o(\Delta t) \cdot \mathbb{E}[M_g] + o(\Delta t) + o(\Delta t) \cdot \mathbb{E}[M_l] - o(\Delta t) &=\\
\mathbb{E}[M] &= \mathbb{E}[M_g] - o(\Delta t) \cdot \mathbb{E}[M_g] + o(\Delta t) \cdot \mathbb{E}[M_l] &=\\
\mathbb{E}[M] &= (1 - o(\Delta t)) \cdot \mathbb{E}[M_g] + o(\Delta t) \cdot \mathbb{E}[M_l].
\end{aligned}$$
```

> **Central formula of TTS represents the mathematical expectation of [[capital multiplier]] from a trade initiated at the current time and concluded after a specified time interval (delta time).**


### Goals

No matter which [[capital multiplier]] will be chosen, single variable to research is $\mathbb{E}[B_s]$:
#### 1. Payment by volume received 
$$\mathbb{E}[M] =\mathbb{E}\bigg{[}\frac{B_s}{B_b} \cdot (1 - f)^2\bigg{]}= \mathbb{E}[B_s] \cdot \frac{(1 - f)^2}{B_b}. $$
#### 2. Payment by volume received 
$$\mathbb{E}[M] =\mathbb{E}\bigg{[}\frac{B_{s}}{B_{b}}(1-f)- f\bigg{]}= \mathbb{E}\bigg{[}B_{s}\frac{1-f}{B_{b}}\bigg{]}- f=\mathbb{E}[B_{s}] \cdot \frac{1-f}{B_{b}}- f$$

**So from central formula those parts need to be determined:**
1. $\mathbb{E}[B_s]$ - expected sell price.
2. $o(\Delta t)$ - function of probability to get loss deal.
