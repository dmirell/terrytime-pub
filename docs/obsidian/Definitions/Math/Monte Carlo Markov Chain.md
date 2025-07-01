A specific variant of the [[Monte Carlo Method]] is the **Monte Carlo Markov Chain (MCMC)**. This technique combines the Monte Carlo method with [[Markov Chain]] to sample from complex, high-dimensional probability distributions. Unlike independent random sampling, MCMC constructs a Markov chain whose stationary distribution is the target distribution.

### Detailed Explanation

In traditional Monte Carlo sampling, each sample is drawn independently from the target distribution. However, in many practical situations, especially in high dimensions, directly sampling from the target distribution is challenging or computationally expensive.

MCMC overcomes this by constructing a Markov chain—a sequence of samples—such that:
- **Each sample depends on the previous one:** The algorithm generates the next sample based on the current sample, creating a dependency between successive states.
- **Stationary distribution:** The chain is designed so that, after many iterations, the distribution of the samples converges to the desired target distribution. This property is known as having the target distribution as its *stationary distribution*.

Example: [[Metropolis-Hastings Algorithm]]

### Key Points

- **Dependence vs. Independence:** Unlike independent sampling, where each sample is drawn without any relation to others, MCMC creates a chain where each sample is dependent on the previous one.
- **Convergence:** Under suitable conditions (e.g., the chain is irreducible and aperiodic), the chain will converge to a stationary distribution that equals the target distribution. Once this convergence is reached, the samples can be used to approximate expectations and probabilities as if they were drawn directly from the target distribution.
