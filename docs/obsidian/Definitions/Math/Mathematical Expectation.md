## Definition

The **expectation** of a random variable $X$, denoted as $E[X]$ or $\mathbb{E}[X]$, represents the average value that $X$ takes in the long run when repeated many times. It is a fundamental concept in probability theory and statistics.

## Expectation for a Discrete Random Variable

If $X$ is a [[discrete random variable]] taking values $x_{1}, x_{2}, \dots, x_{n}$ with corresponding probabilities $p_{1}, p_{2}, \dots, p_{n}$, the expectation is defined as:

$$
E[X] = \sum_{i}^{n} x_i P(X = x_i)
$$

where $P(X = x_{i})$ represents the probability of $X$ taking the value $x_{i}$.

## Expectation for a Continuous Random Variable

If $X$ is a continuous random variable with [[probability density function (PDF)]] $f(x)$, then its expectation is given by:

$$
E[X] = \int_{-\infty}^{\infty} x f(x) \, dx
$$

where $f(x)$ is the probability density function of $X$.

## Properties of Expectation

### Linearity

For any two random variables $X$ and $Y$, and constants $a, b$:

$$
E[aX + bY] = aE[X] + bE[Y]
$$

### Expectation of a Constant

If $c$ is a constant, then:

$$
E[c] = c
$$

### Non-Negativity (for Non-Negative Variables)

If $X \geq 0$ always, then:

$$
E[X] \geq 0
$$
