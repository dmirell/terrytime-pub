## Definition  

The **standard deviation** of a random variable $X$, denoted as $\sigma(X)$ or simply $\sigma$, measures the amount of variation or [[dispersion]] from the mean. It is defined as the square root of the variance.

## Formula for Standard Deviation  

The standard deviation is given by:

$$
\sigma(X) = \sqrt{D(X)}
$$

where $D(X)$ is the **dispersion** of $X$, defined as:

$$
D(X) = E[(X - E[X])^2]
$$

Expanding the expectation:

$$
D(X) = E[X^2] - (E[X])^2
$$

Thus, the standard deviation can also be written as:

$$
\sigma(X) = \sqrt{E[X^2] - (E[X])^2}
$$

## Interpretation  

- A **higher** standard deviation indicates **greater** variability in the data.
- A **lower** standard deviation means the data points are **closer** to the mean.
- If all values of $X$ are the same, then $\sigma(X) = 0$.

## Properties of Standard Deviation  

### Non-Negativity  

Since the variance is always non-negative, the standard deviation satisfies:

$$
\sigma(X) \geq 0
$$

### Scaling Property  

For a constant $a$ and a random variable $X$:

$$
\sigma(aX) = |a| \sigma(X)
$$

### Relationship with Expectation  

If $X$ is a random variable and $c$ is a constant:

$$
\sigma(X + c) = \sigma(X)
$$

which means adding a constant does not change the standard deviation.

