## Definition
A **probability distribution** describes how the probabilities are distributed over the values of a random variable. It defines the likelihood of different outcomes in an experiment or a random process.

### **Two Main Types of Probability Distributions:**
1. **Discrete Probability Distributions** – For random variables that can take a finite or countably infinite set of values.
2. **Continuous Probability Distributions** – For random variables that can take any value within a certain interval or range.

---

## 1. Discrete Probability Distributions
In a **discrete distribution**, the random variable $X$ takes distinct or countable values.

### **Examples of Discrete Distributions:**

1. **Bernoulli Distribution**  
   - Describes a random experiment with exactly two possible outcomes: "success" or "failure."
     $$P(X = x) = p^x (1-p)^{1-x}, \quad x \in \{0, 1\}$$  
     where $p$ is the probability of success.

2. **Binomial Distribution**  
   - Describes the number of successes in a fixed number of independent Bernoulli trials.  
     $$ P(X = k) = \binom{n}{k} p^k (1-p)^{n-k}, \quad k = 0,1,2,...,n $$  
     where $n$ is the number of trials and $p$ is the probability of success on a single trial.

3. **Poisson Distribution**  
   - Describes the number of events occurring in a fixed interval of time/space, with a known constant mean rate $\lambda$.  
     $$ P(X = k) = \frac{\lambda^k e^{-\lambda}}{k!}, \quad k = 0, 1, 2, \dots $$  
     where $\lambda$ is the average rate of occurrence.

---

## 2. Continuous Probability Distributions
In a **continuous distribution**, the random variable $X$ can take any value within a given range or interval.

### **Examples of Continuous Distributions:**

1. **Uniform Distribution**  
   - Describes a random variable where all outcomes in an interval are equally likely. 
     $$ f_X(x) = \frac{1}{b - a}, \quad a \leq x \leq b $$  
     where $a$ and $b$ are the bounds of the interval.

2. **Normal (Gaussian) Distribution**  
   - Describes a continuous random variable with a symmetric bell-shaped curve, characterized by its mean $\mu$ and standard deviation $\sigma$.  
     $$ f_X(x) = \frac{1}{\sigma\sqrt{2\pi}} e^{-\frac{(x - \mu)^2}{2\sigma^2}} $$  
     where $\mu$ is the [[Mathematical Expectation|mean]] and $\sigma$ is the [[standard deviation]].

3. **Exponential Distribution**  
   - Describes the time between events in a Poisson process, with a constant average rate $\lambda$.  
     $$ f_X(x) = \lambda e^{-\lambda x}, \quad x \geq 0 $$  
     where $\lambda$ is the rate parameter.

---

## Key Properties of Probability Distributions

1. **Probability Mass Function (PMF)**  
   For discrete distributions, the PMF gives the probability that a discrete random variable takes a specific value.

2. **Probability Density Function (PDF)**  
   For continuous distributions, the PDF is used to describe the likelihood of a random variable falling within a specific range of values. It must satisfy:  
   $$ \int_{-\infty}^{\infty} f_X(x) \, dx = 1 $$  
   where $f_X(x)$ is the PDF.

3. **Cumulative Distribution Function (CDF)**  
   The CDF gives the probability that a random variable is less than or equal to a particular value. It is defined as:  
   $$ F_X(x) = P(X \leq x) $$  
   - For discrete distributions:  
     $$ F_X(x) = \sum_{k=-\infty}^x P(X = k) $$  
   - For continuous distributions:  
     $$ F_X(x) = \int_{-\infty}^x f_X(t) \, dt $$

4. **Expected Value (Mean)**  
   The expected value (or mean) of a random variable gives the long-run average or central location of its distribution.  
   - For discrete:  
     $$ \mathbb{E}[X] = \sum_{x} x P(X = x) $$  
   - For continuous:  
     $$ \mathbb{E}[X] = \int_{-\infty}^{\infty} x f_X(x) \, dx $$

5. **Variance**  
   The [[Dispersion|variance]] measures the spread or dispersion of a distribution. It is defined as:  
   $$ \text{Var}(X) = \mathbb{E}[X^2] - (\mathbb{E}[X])^2 $$

---

## Applications of Probability Distributions
- **Statistics**: Estimation, hypothesis testing, regression models.
- **Machine Learning**: Classification, clustering, and prediction tasks.
- **Finance**: Modeling asset returns, risk assessment, pricing of financial instruments.
- **Queueing Theory**: Modeling waiting times, service rates, etc.
- **Reliability Engineering**: Assessing system failure rates, lifespan modeling.

---

## Summary Table

| Distribution    | Type       | Parameters                                        | PDF/PMF Formula                                                           |
| --------------- | ---------- | ------------------------------------------------- | ------------------------------------------------------------------------- |
| **Bernoulli**   | Discrete   | $p$ (probability of success)                      | $P(X = x) = p^x(1-p)^{1-x}$                                               |
| **Binomial**    | Discrete   | $n$ (number of trials), $p$ (success probability) | $P(X = k) = \binom{n}{k} p^k (1-p)^{n-k}$                                 |
| **Poisson**     | Discrete   | $\lambda$ (rate)                                  | $P(X = k) = \frac{\lambda^k e^{-\lambda}}{k!}$                            |
| **Uniform**     | Continuous | $a, b$ (interval bounds)                          | $f_X(x) = \frac{1}{b - a}$                                                |
| **Normal**      | Continuous | $\mu$ (mean), $\sigma$ (standard deviation)       | $f_X(x) = \frac{1}{\sigma\sqrt{2\pi}} e^{-\frac{(x - \mu)^2}{2\sigma^2}}$ |
| **Exponential** | Continuous | $\lambda$ (rate)                                  | $f_X(x) = \lambda e^{-\lambda x}$                                         |

