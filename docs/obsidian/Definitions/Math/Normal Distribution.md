## Definition  
The **Normal Distribution**, also known as the **Gaussian Distribution**, is a continuous probability distribution characterized by its bell-shaped curve. It is widely used in statistics, finance, and natural sciences.

## Probability Density Function (PDF)  
The probability density function of a normal distribution with mean $\mu$ and variance $\sigma^2$ is given by:

$$
f(x)=\frac{1}{\sigma\sqrt{2\pi}} e^{-\frac{(x-\mu)^2}{2\sigma^2}}
$$

where:
- $\mu$ is the **mean** ([[mathematical expectation]]).
- $\sigma^2$ is the [[dispersion]].
- $\sigma$ is the [[standard deviation]].

## Standard Normal Distribution  
A **Standard Normal Distribution** is a special case where $\mu=0$ and $\sigma^2=1$. Its PDF simplifies to:

$$
\phi(z)=\frac{1}{\sqrt{2\pi}} e^{-\frac{z^2}{2}}
$$

A random variable $Z$ following a standard normal distribution is denoted as:

$$
Z\sim\mathcal{N}(0,1)
$$

## Cumulative Distribution Function (CDF)  
The cumulative distribution function (CDF) is:

$$
\Phi(x)=\int_{-\infty}^{x} \frac{1}{\sigma\sqrt{2\pi}} e^{-\frac{(t-\mu)^2}{2\sigma^2}} dt
$$

This function gives the probability that a normal variable takes a value less than or equal to $x$.

## Properties  
- **Symmetric** around the mean $\mu$.  
- The **68-95-99.7 Rule**:  
  - **68%** of values lie within $1\sigma$ of $\mu$.  
  - **95%** of values lie within $2\sigma$ of $\mu$.  
  - **99.7%** of values lie within $3\sigma$ of $\mu$.  
- The sum of independent normal variables is also normally distributed.

## Applications  
- **Statistics**: Hypothesis testing, confidence intervals.  
- **Machine Learning**: Gaussian Naïve Bayes, Gaussian Mixture Models (GMM).  
- **Finance**: Risk modeling, asset returns.  

