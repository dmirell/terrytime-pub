## Definition  

The **covariance** of two random variables $X$ and $Y$, denoted as $\text{Cov}(X, Y)$, measures how much the variables change together. It quantifies the relationship between $X$ and $Y$ in terms of their [[Standard Deviation|deviation]] from their [[Mathematical Expectation|expected]] values.

## Formula for Covariance  

The covariance is defined as:

$$
\text{Cov}(X, Y) = E[(X - E[X])(Y - E[Y])]
$$
Expanding the expectation:

$$
\text{Cov}(X, Y) = E[XY] - E[X]E[Y]
$$
where:  
- $E[X]$ and $E[Y]$ are the expected values (means) of $X$ and $Y$.  
- $E[XY]$ is the expected value of the product of $X$ and $Y$.  

## Interpretation  

- **$\text{Cov}(X, Y) > 0$** → $X$ and $Y$ tend to increase or decrease together (positive correlation).
- **$\text{Cov}(X, Y) < 0$** → When $X$ increases, $Y$ tends to decrease, and vice versa (negative correlation).
- **$\text{Cov}(X, Y) = 0$** → $X$ and $Y$ are uncorrelated (no linear relationship).

## Properties of Covariance  

### Symmetry  

$$
\text{Cov}(X, Y) = \text{Cov}(Y, X)
$$

### Covariance of a Variable with Itself  

$$
\text{Cov}(X, X) = D(X)
$$

### Scaling Property  

For constants $a$ and $b$:

$$
\text{Cov}(aX, bY) = ab \, \text{Cov}(X, Y)
$$

### Addition Property  

For any random variables $X, Y, Z$ and constants $a, b$:

$$
\text{Cov}(aX + bY, Z) = a \text{Cov}(X, Z) + b \text{Cov}(Y, Z)
$$

### Relationship with Correlation  

The **correlation coefficient** $\rho(X, Y)$ is the normalized form of covariance:

$$
\rho(X, Y) = \frac{\text{Cov}(X, Y)}{\sigma(X) \sigma(Y)}
$$

where $\sigma(X)$ and $\sigma(Y)$ are the standard deviations of $X$ and $Y$.

