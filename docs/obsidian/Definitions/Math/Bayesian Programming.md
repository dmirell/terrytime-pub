**Bayesian programming** is a way of writing computer programs that handle uncertainty by treating unknowns as random variables and updating our beliefs as we see data.

## Key Idea

We start with a **prior** belief about a quantity, observe data, and use **Bayes’ theorem** to get an updated **posterior** belief:

$$
P(\theta \mid D) = \frac{P(D \mid \theta)\,P(\theta)}{P(D)}.
$$

- $P(\theta)$ is the **prior**: what we believe about $\theta$ before seeing data.  
- $P(D \mid \theta)$ is the **likelihood**: how likely the data $D$ is if $\theta$ were true.  
- $P(\theta \mid D)$ is the **posterior**: our updated belief about $\theta$ after seeing $D$.  
- $P(D)$ is the **evidence** (a normalizing constant).

## Simple Example: Coin Flip

We want to learn the probability $\theta$ of flipping heads:

1. **Prior**: assume a uniform prior (we’re initially agnostic):
   $$
   \theta \sim \mathrm{Beta}(1,1).
   $$
2. **Data**: flip the coin 10 times, observe 7 heads.
3. **Likelihood**:
   $$
   P(D \mid \theta) = \binom{10}{7}\,\theta^7\,(1-\theta)^3.
   $$
4. **Posterior**: combine prior and likelihood:
   $$
   P(\theta \mid D) \propto \theta^7 (1-\theta)^3 \times 1
   $$
   which gives
   $$
   \theta \mid D \sim \mathrm{Beta}(8,4).
   $$

## Writing a Bayesian Program

In a probabilistic programming language (e.g., PyMC, Stan, Pyro), you:

1. **Define priors** for your unknowns.
2. **Specify how data is generated** (the likelihood).
3. **Run inference** (e.g., MCMC) to approximate the posterior.

### Pseudo-code

```python
# Define a prior for theta
theta = sample('theta', Beta(1, 1))

# Observe data (each flip is 0 or 1)
for flip in data:
    observe(Bernoulli(theta), flip)

# Perform inference
posterior = infer(samples=1000)
