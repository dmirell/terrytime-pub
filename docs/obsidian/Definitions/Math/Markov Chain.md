A **Markov chain** is a [[Stochastic Process]] that models a sequence of events where the probability of each event depends solely on the state attained in the previous event. This memoryless property makes Markov chains a fundamental tool in probability theory and various applications in science, economics, and finance.

## Overview

A Markov chain is defined by its **state space** and the **transition probabilities** between these states. The key property, known as the **Markov property**, asserts that the future state of the process is independent of the past given the present state.

## Key Definitions

### Markov Property

The process $\{X_n\}_{n \geq 0}$ satisfies the Markov property if, for any $n \geq 0$ and states $x_0, x_1, \dots, x_{n+1}$:
$$
P(X_{n+1} = x \mid X_n = x_n, X_{n-1} = x_{n-1}, \dots, X_0 = x_0) = P(X_{n+1} = x \mid X_n = x_n)
$$

### Transition Matrix

For a discrete Markov chain with a finite number of states, the transition probabilities can be represented in matrix form. The **transition matrix** $P$ is given by:
$$
P = \begin{bmatrix}
p_{11} & p_{12} & \cdots & p_{1N} \\
p_{21} & p_{22} & \cdots & p_{2N} \\
\vdots & \vdots & \ddots & \vdots \\
p_{N1} & p_{N2} & \cdots & p_{NN} \\
\end{bmatrix}
$$
where each element $p_{ij} = P(X_{n+1} = j \mid X_n = i)$ represents the probability of transitioning from state $i$ to state $j$.

### State Distribution

If $v^{(0)}$ denotes the initial state distribution (a row vector), then the distribution after $n$ steps is given by:
$$
v^{(n)} = v^{(0)} P^n
$$
This formula allows us to compute the probability of being in any state after $n$ transitions.

## Properties of Markov Chains

### Stationary Distribution

A **stationary distribution** $v$ is a probability distribution that remains unchanged by the transition matrix:
$$
v = v P
$$
If a Markov chain is *irreducible* and *aperiodic*, it will converge to this unique stationary distribution regardless of the initial state:
$$
\lim_{n \to \infty} v^{(0)} P^n = v
$$

### Long-Term Behavior and Convergence

Under the above conditions, as the number of transitions increases, the chain "forgets" its initial state. This convergence is crucial for applications such as statistical inference and algorithm design (e.g., [[Monte Carlo Markov Chain]] methods).

## Mathematical Connections

- **Mathematical Expectation:** The expected value of a function $f$ over the states, when the chain is in its stationary distribution, is given by:
  $$
  \mathbb{E}[f(X)] = \sum_i v_i f(i)
  $$
- **Probability Distributions:** Markov chains intersect with other probability concepts. For instance, transitions can be modeled using distributions like the [[Poisson Distribution|Poisson Distribution]] or approximated by the [[Normal Distribution|Normal Distribution]] under the [[Central Limit Theorem (CLT)|Central Limit Theorem]].
- **Modern Portfolio Theory:** In financial applications, Markov chains can model regime-switching in market returns, offering insights similar to those explored in [[Modern Portfolio Theory|Modern Portfolio Theory]].
