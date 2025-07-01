The **Metropolis-Hastings algorithm** is one of the fundamental methods within the family of [[Monte Carlo Markov Chain]] algorithms. It allows us to sample from complex probability distributions where direct, independent sampling is challenging or infeasible.

## Overview

Unlike independent sampling — where each sample is drawn directly from the target distribution — the Metropolis-Hastings algorithm constructs a Markov chain. Each new state is generated based on the current state, and the chain is designed so that its stationary distribution is the target distribution. This means that, as the chain evolves, the distribution of its states converges to the desired target distribution.

## Detailed Explanation

The algorithm operates through the following steps:

1. **Initialization:**  
   Choose an initial state $x_0$.

2. **Proposal Step:**  
   Given the current state $x$, generate a candidate $x'$ using a proposal distribution $q(x' \mid x)$. A common choice is a symmetric distribution, such as a normal distribution.

3. **Acceptance Probability:**  
   Compute the acceptance probability:
   $$
   \alpha(x, x') = \min\left( 1, \frac{f(x') \, q(x \mid x')}{f(x) \, q(x' \mid x)} \right)
   $$
   where $f(x)$ is the target density from which we want to sample.

4. **Acceptance or Rejection:**  
   Accept the candidate with probability $\alpha(x, x')$. If accepted, set the new state $x_{\text{new}} = x'$; if not, retain the current state ($x_{\text{new}} = x$).

5. **Iteration:**  
   Repeat steps 2–4 to form a sequence of states $\{x_0, x_1, x_2, \dots\}$. Under suitable conditions (e.g., irreducibility and aperiodicity), the distribution of the states converges to $f(x)$ as the number of iterations increases.

## Explanation of Proposal Term Cancellation in Metropolis-Hastings

In the general acceptance probability formula for the Metropolis - Hastings algorithm, we have:

$$
\alpha(x, x') = \min\left( 1, \frac{f(x') \, q(x \mid x')}{f(x) \, q(x' \mid x)} \right)
$$

Here,  
- $f(x)$ is the target density from which we wish to sample, and  
- $q(x' \mid x)$ is the proposal distribution that gives the probability of proposing a move from state $x$ to state $x'$.

## Symmetric Proposal Distributions

A common choice for $q$ is a symmetric proposal distribution. This means that the probability of proposing a move from $x$ to $x'$ is the same as the probability of proposing a move from $x'$ back to $x$:

$$
q(x' \mid x) = q(x \mid x')
$$

Because of this symmetry, the proposal terms in the acceptance ratio cancel out:

$$
\frac{q(x \mid x')}{q(x' \mid x)} = 1
$$

Thus, the acceptance probability simplifies to:

$$
\alpha(x, x') = \min\left( 1, \frac{f(x')}{f(x)} \right)
$$

### Example: Sampling from a Standard Normal Distribution

Below is an example in Python demonstrating the Metropolis - Hastings algorithm for sampling from a standard normal distribution $N(0, 1)$:

```python
import numpy as np
import matplotlib.pyplot as plt

def target_distribution(x):
    """
    Target density: Standard Normal distribution N(0, 1).
    Computes the probability density at point x.
    """
    return np.exp(-0.5 * x**2) / np.sqrt(2 * np.pi)

def metropolis_hastings(target, initial, iterations, proposal_std):
    """
    Metropolis - Hastings algorithm for sampling from a target distribution.

    Parameters:
    - target: Function representing the target probability density.
    - initial: Initial value of the chain.
    - iterations: Number of iterations.
    - proposal_std: Standard deviation for the proposal distribution.
    
    Returns:
    - samples: Array of generated samples.
    """
    samples = [initial]
    current = initial
    for _ in range(iterations):
        # Generate candidate from a normal distribution centered at the current value
        candidate = np.random.normal(current, proposal_std)
        # Compute the acceptance probability
        acceptance_prob = min(1, target(candidate) / target(current))
        # Accept candidate with probability acceptance_prob
        if np.random.rand() < acceptance_prob:
            current = candidate
        samples.append(current)
    return np.array(samples)

if __name__ == "__main__":
    # Parameters for the algorithm
    initial_value = 0.0      # Starting point of the chain
    num_iterations = 10000   # Number of iterations
    proposal_std = 1.0       # Standard deviation for the proposal distribution

    # Run the Metropolis - Hastings algorithm
    samples = metropolis_hastings(target_distribution, initial_value, num_iterations, proposal_std)
    
    # Plot the target density and histogram of the generated samples
    x = np.linspace(-4, 4, 1000)
    plt.plot(x, target_distribution(x), label="Target Density (N(0, 1))", color="red")
    plt.hist(samples, bins=50, density=True, alpha=0.5, label="M - H Samples")
    plt.xlabel("x")
    plt.ylabel("Density")
    plt.legend()
    plt.title("Metropolis - Hastings Algorithm")
    plt.show()
