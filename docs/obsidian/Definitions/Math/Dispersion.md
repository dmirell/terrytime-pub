## Definition  

The **dispersion** of a random variable $X$, denoted as $D(X)$, measures how far the values of $X$ spread out from the [[Mathematical Expectation|expectation]] value (mean). It is the **expectation** of the squared deviation from the mean.

## Formula for Variance  

The dispersion is defined as:

$$
D(X) = E[(X - E[X])^2]
$$

Expanding the **expectation**:

$$
D(X) = E[X^2] - (E[X])^2
$$

where:  
- $E[X]$ is the expected value (mean) of $X$,  
- $E[X^2]$ is the expected value of $X^2$.  

## Interpretation  

- A **higher** variance indicates a **greater** spread of values.
- A **lower** variance means values are **closer** to the mean.
- If all values of $X$ are the same, then $D(X) = 0$.

## Properties of Variance  

### Non-Negativity  

Since it is an expectation of a squared quantity:

$$
D(X) \geq 0
$$

### Scaling Property  

For a constant $a$ and a random variable $X$:

$$
D(aX) = a^2 D(X)
$$

### Addition Property  

For independent random variables $X$ and $Y$:

$$
D(X + Y) = D(X) + D(Y)
$$

If $X$ and $Y$ are not independent, the formula includes a [[Covariance]] term:

$$
D(X + Y) = D(X) + D(Y) + 2 \text{Cov}(X, Y)
$$

where:

$$
\text{Cov}(X, Y) = E[(X - E[X])(Y - E[Y])]
$$

### Effect of Adding a Constant  

Adding a constant $c$ does not change the **dispersion**:

$$
D(X + c) = D(X)
$$

