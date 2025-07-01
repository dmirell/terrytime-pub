A **stochastic process** is a collection of random variables indexed by time (or space) that models the evolution of a system under uncertainty. In simpler terms, it is a mathematical framework used to describe phenomena that change in a random manner over time.

## Formal Definition

Let $(\Omega, \mathcal{F}, P)$ be a probability space and let $T$ be an index set (often representing time). A stochastic process is defined as a family of random variables:
$$
\{ X_t \}_{t \in T},
$$
where each $X_t$ is a random variable defined on the probability space. The index $t$ can denote discrete time (e.g., $t \in \mathbb{N}$) or continuous time (e.g., $t \in \mathbb{R}_{\geq 0}$).

## Examples

- **Random Walk:**  
  A basic example is a random walk, where each state is determined by the previous state plus a random increment:
  $$
  X_{n+1} = X_n + Y_n,
  $$
  with $Y_n$ representing independent, identically distributed random variables.

- **Poisson Process:**  
  This process models the number of events occurring within a fixed period of time, assuming that these events happen independently and at a constant average rate.

- **Brownian Motion:**  
  Also known as the Wiener process, Brownian motion is a continuous-time process used to model phenomena such as the erratic movement of particles in a fluid or the fluctuation of stock prices.

## Classification of Stochastic Processes

Stochastic processes can be categorized in several ways:

- **By Time Index:**
  - *Discrete-time:* The process is defined only at specific time points (e.g., $n = 0, 1, 2, \dots$).
  - *Continuous-time:* The process is defined for every instant within a continuous interval.

- **By State Space:**
  - *Discrete-state:* The set of possible values for $X_t$ is countable.
  - *Continuous-state:* The set of possible values for $X_t$ is uncountable, such as the set of real numbers.

- **By Dependence Structure:**
  - *Markov Process:* The future state depends only on the current state and not on the history of past states.
  - *Non-Markovian Process:* The future evolution may depend on the entire history of the process.

## Key Properties

- **Stationarity:**  
  A stochastic process is stationary if its statistical properties (like [[Mathematical Expectation|mean]] and [[Dispersion|variance]]) remain constant over time.

- **Ergodicity:**  
  Ergodicity means that time averages computed from a single, long realization of the process converge to the ensemble averages over the probability space.

- **Independence of Increments:**  
  In some processes—such as the Poisson process and Brownian motion—the increments over non-overlapping intervals are independent of each other.

## Applications

Stochastic processes are fundamental in modeling systems across various disciplines:
- **Finance:** For modeling stock prices, interest rates, and market risks.
- **Physics:** In the study of particle motion and diffusion.
- **Biology:** To model population dynamics and the spread of diseases.
- **Engineering:** In signal processing and systems with random noise.

## Conclusion

A stochastic process provides a robust framework to capture and analyze the inherent randomness in systems that evolve over time. By representing the system's evolution as a collection of random variables, stochastic processes allow us to predict and understand the behavior of complex, dynamic phenomena.
