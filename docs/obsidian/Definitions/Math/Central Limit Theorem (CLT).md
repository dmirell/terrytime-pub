## Definition  
The **Central Limit Theorem (CLT)** states that the sum (or mean) of a large number of independent and identically distributed (i.i.d.) random variables **approaches a [[Normal Distribution]]**, regardless of the original distribution of the variables.

## Formal Statement  
Let $X_1, X_2, \dots, X_n$ be i.i.d. random variables with finite [[Mathematical Expectation|mean]] $\mathbb{E}[X] = \mu$ and [[dispersion]] $D(X) = \sigma^2$. Then the normalized sum:

$$
\frac{\sum_{i=1}^{n} X_i - n\mu}{\sigma \sqrt{n}} \xrightarrow{d} \mathcal{N}(0,1), \quad \text{as } n \to \infty.
$$

where:
- $\mathcal{N}(0,1)$ is the standard **normal distribution**.
- $\xrightarrow{d}$ denotes convergence in distribution.

## Intuition  
- The distribution of the sum (or mean) of many random variables becomes approximately normal, even if the original variables are not normally distributed.  
- The larger the sample size $n$, the better the normal approximation.

## Applications  
- Justifies the use of normal distribution in **statistical inference** (confidence intervals, hypothesis testing).  
- Used in **machine learning** to approximate distributions of large datasets.  
- Important in **finance**, where aggregated returns of assets tend to follow a normal distribution.  

## Example  
If we roll a fair die $n$ times and take the average outcome, then for large $n$, the distribution of the sample mean will be approximately normal.
