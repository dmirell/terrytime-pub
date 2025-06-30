## Definition  
The **Poisson distribution** describes the probability of a given number of events occurring in a fixed interval of time or space, assuming that the events happen independently and at a constant average rate $\lambda$.  

A random variable $X$ following a Poisson distribution is denoted as:  

$$
X \sim \text{Poisson}(\lambda)
$$

where the **probability mass function (PMF)** is:  

$$
P(X = k) = \frac{\lambda^k e^{-\lambda}}{k!}, \quad k = 0,1,2,\dots
$$

---
## Properties 
- **Mean** ([[Mathematical Expectation]]): $\mathbb{E}[X]=\lambda$ 
- [[Dispersion]]: $D(X)=\lambda$ 
- **Memoryless property**: Unlike the exponential distribution, the Poisson distribution does **not** have the memoryless property. 
- **Additivity**: If $X_1\sim\text{Poisson}(\lambda_1)$ and $X_2\sim\text{Poisson}(\lambda_2)$ are independent, then their sum is also Poisson-distributed: $$ X_1+X_2\sim\text{Poisson}(\lambda_1+\lambda_2) $$
### Proofs
#### **1. Mean (Expectation)**
By the definition of expectation:  

$$
\mathbb{E}[X] = \sum_{k=0}^{\infty} k P(X=k)
$$

Substituting the Poisson PMF:

$$
\mathbb{E}[X] = \sum_{k=0}^{\infty} k \frac{\lambda^k e^{-\lambda}}{k!}
$$

Rewriting $k$ as $k = k \cdot \frac{1}{(k-1)!} (k-1)!$:

$$
\mathbb{E}[X] = \sum_{k=1}^{\infty} \lambda \frac{\lambda^{k-1} e^{-\lambda}}{(k-1)!}
$$

Changing the summation index $m = k - 1$:

$$
\mathbb{E}[X] = \lambda \sum_{m=0}^{\infty} \frac{\lambda^m e^{-\lambda}}{m!}
$$

Since the sum is the **total probability of a Poisson-distributed variable (which equals 1)**:

$$
\mathbb{E}[X] = \lambda \cdot 1 = \lambda
$$

✔ **Proved!**

---

#### **2. Dispersion**

[[Dispersion]](Variance) is defined as:

$$
\text{Var}(X) = \mathbb{E}[X^2] - (\mathbb{E}[X])^2
$$

#### Step 1: Compute $\mathbb{E}[X^2]$
By definition:

$$
\mathbb{E}[X^2] = \sum_{k=0}^{\infty} k^2 P(X = k)
$$

Using the identity $k^2 = k(k-1) + k$:

$$
\mathbb{E}[X^2] = \sum_{k=0}^{\infty} (k(k-1) + k) P(X=k)
$$

Splitting the sum:

$$
\mathbb{E}[X^2] = \sum_{k=0}^{\infty} k(k-1) P(X=k) + \sum_{k=0}^{\infty} k P(X=k)
$$

From the expectation proof, we already know:

$$
\sum_{k=0}^{\infty} k P(X=k) = \lambda
$$

Now, computing $\sum_{k=0}^{\infty} k(k-1) P(X=k)$:

$$
\sum_{k=2}^{\infty} k(k-1) \frac{\lambda^k e^{-\lambda}}{k!}
$$

Rewriting $k(k-1)$ as $k(k-1) = \lambda^2 \cdot \frac{1}{(k-2)!} (k-2)!$:

$$
\sum_{k=2}^{\infty} \lambda^2 \frac{\lambda^{k-2} e^{-\lambda}}{(k-2)!}
$$

Changing the summation index $m = k - 2$:

$$
\lambda^2 \sum_{m=0}^{\infty} \frac{\lambda^m e^{-\lambda}}{m!} = \lambda^2 \cdot 1 = \lambda^2
$$

Thus:

$$
\mathbb{E}[X^2] = \lambda^2 + \lambda
$$

#### Step 2: Compute Dispersion
Now using the variance formula:

$$
\text{Var}(X) = \mathbb{E}[X^2] - (\mathbb{E}[X])^2
$$

Substituting the values:

$$
\text{Var}(X) = (\lambda^2 + \lambda) - \lambda^2 = \lambda
$$

✔ **Proved!**

---

#### **3. Additivity Property**

Using the **moment generating function (MGF)**:

The MGF of a Poisson-distributed random variable is:

$$
M_X(t) = \exp(\lambda (e^t - 1))
$$

For independent Poisson variables:

$$
M_{X_1 + X_2}(t) = M_{X_1}(t) M_{X_2}(t)
$$

$$
= \exp(\lambda_1 (e^t - 1)) \cdot \exp(\lambda_2 (e^t - 1))
$$

$$
= \exp((\lambda_1 + \lambda_2)(e^t - 1))
$$

which is the MGF of a **Poisson($\lambda_1 + \lambda_2$)** distribution.

✔ **Proved!**

---

## Applications  
- **Queueing theory**: Modeling arrivals of customers.  
- **Physics**: Counting radioactive decay events.  
- **Finance**: Modeling rare events like insurance claims.  
- **Machine learning**: Poisson regression for count-based predictions.  

---
**Related Notes:**  
- [[Exponential Distribution]]  
- [[Probability Distributions]]  
