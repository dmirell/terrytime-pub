**Autocorrelation** (also known as serial correlation) measures the correlation of a time series with its own past and future values. It quantifies the degree of similarity between a given time series and a lagged version of itself over successive time intervals. This concept is fundamental in time series analysis as it helps to identify patterns, seasonality, or trends within the data.

## Formal Definition

For a stationary time series $\{X_t\}$, the autocorrelation at lag $k$ is defined as:

$$
\rho(k)=\frac{\operatorname{Cov}(X_t,X_{t+k})}{\operatorname{Var}(X_t)}
$$

where:
- $\operatorname{Cov}(X_t,X_{t+k})=E[(X_t-\mu)(X_{t+k}-\mu)]$ is the [[covariance]] between $X_t$ and $X_{t+k}$,
- $\operatorname{Var}(X_t)=E[(X_t-\mu)^2]$ is the [[dispersion]] of $X_t$,
- $\mu=E[X_t]$ is the [[Mathematical Expectation|mean]] of the series.

In practice, for a finite sample of $n$ observations $x_1,x_2,\dots,x_n$, the sample autocorrelation function (ACF) at lag $k$ is estimated as:

$$
\hat{\rho}(k)=\frac{\sum_{t=1}^{n-k}(x_t-\bar{x})(x_{t+k}-\bar{x})}{\sum_{t=1}^{n}(x_t-\bar{x})^2}
$$

where $\bar{x}$ is the sample mean.

## Intuitive Explanation

- **Lag:**  
  Lag $k$ represents the number of time steps by which the series is shifted. For example, lag 1 compares each value with the value immediately following it.

- **Interpretation:**  
  - A high positive autocorrelation at a particular lag suggests that the series tends to remain at similar levels from one time step to the next (i.e., persistence).
  - A negative autocorrelation indicates a tendency for the series to oscillate, meaning a high value is likely to be followed by a low value.
  - An autocorrelation near zero suggests little to no linear relationship between observations at that lag.

## Examples

1. **Random Series:**  
   In a completely random (white noise) series, the autocorrelation is close to zero for all lags (except lag 0, which is always 1).

2. **Seasonal Data:**  
   For data with a seasonal pattern, autocorrelation can be high at lags corresponding to the seasonality period. For example, daily temperature might show high autocorrelation at lag 365 (in days) for annual seasonality.

## Python Code Example

The following Python code demonstrates how to calculate and plot the autocorrelation function (ACF) for a time series using both manual computation and the `statsmodels` library.

```python
import numpy as np
import matplotlib.pyplot as plt
from statsmodels.graphics.tsaplots import plot_acf

# Generate a sample time series (for example, an AR(1) process)
np.random.seed(42)
n = 200
phi = 0.8  # AR coefficient
x = np.zeros(n)
x[0] = np.random.normal()
for t in range(1, n):
    x[t] = phi * x[t-1] + np.random.normal()

# Manual computation of sample autocorrelation for lags up to 20
def sample_autocorrelation(series, lag):
    n = len(series)
    series_mean = np.mean(series)
    numerator = np.sum((series[:n-lag] - series_mean) * (series[lag:] - series_mean))
    denominator = np.sum((series - series_mean)**2)
    return numerator / denominator

lags = 20
autocorr = [sample_autocorrelation(x, lag) for lag in range(lags+1)]

# Plot manually computed autocorrelation
plt.figure(figsize=(10, 4))
plt.stem(range(lags+1), autocorr, use_line_collection=True)
plt.xlabel("Lag")
plt.ylabel("Autocorrelation")
plt.title("Sample Autocorrelation Function (Manual Calculation)")
plt.show()

# Alternatively, using statsmodels to plot ACF
plt.figure(figsize=(10, 4))
plot_acf(x, lags=20)
plt.title("Sample Autocorrelation Function (Statsmodels)")
plt.show()
