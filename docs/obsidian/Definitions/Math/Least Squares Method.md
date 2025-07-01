The **least squares method** is a fundamental approach in regression analysis and numerical optimization. It is widely used to find the best-fitting line (or curve) through a set of data points by minimizing the sum of the squares of the residuals—the differences between observed and predicted values.

## Basic Concept

Suppose we have a set of $n$ data points $(x_1,y_1), (x_2,y_2),\dots,(x_n,y_n)$. We want to fit a linear model of the form

$$
y=\beta_0+\beta_1x+\varepsilon
$$

where:
- $\beta_0$ is the intercept,
- $\beta_1$ is the slope,
- $\varepsilon$ is the error term.

The objective of the least squares method is to find $\beta_0$ and $\beta_1$ that minimize the sum of squared errors (SSE):

$$
SSE=\sum_{i=1}^{n}(y_i-\beta_0-\beta_1x_i)^2
$$

## Derivation

To find the optimal $\beta_0$ and $\beta_1$, we set the partial derivatives of $SSE$ with respect to $\beta_0$ and $\beta_1$ to zero:

$$
\frac{\partial SSE}{\partial \beta_0}=-2\sum_{i=1}^{n}(y_i-\beta_0-\beta_1x_i)=0
$$

$$
\frac{\partial SSE}{\partial \beta_1}=-2\sum_{i=1}^{n}x_i(y_i-\beta_0-\beta_1x_i)=0
$$

These equations yield the **normal equations**:

$$
\sum_{i=1}^{n}y_i=n\beta_0+\beta_1\sum_{i=1}^{n}x_i
$$

$$
\sum_{i=1}^{n}x_iy_i=\beta_0\sum_{i=1}^{n}x_i+\beta_1\sum_{i=1}^{n}x_i^2
$$

Solving these equations gives:

$$
\beta_1=\frac{\sum_{i=1}^{n}(x_i-\bar{x})(y_i-\bar{y})}{\sum_{i=1}^{n}(x_i-\bar{x})^2}
$$

and

$$
\beta_0=\bar{y}-\beta_1\bar{x}
$$

where $\bar{x}$ and $\bar{y}$ are the sample means of the $x$ and $y$ data, respectively.

## Geometric Interpretation

The least squares solution projects the vector of observed responses onto the space spanned by the predictors. In the case of simple linear regression, it finds the line that minimizes the sum of the squared vertical distances between the data points and the line.

## Extensions to Multiple Regression

The least squares method is not limited to simple linear regression. In multiple regression, where the model is

$$
y=X\beta+\varepsilon
$$

- $X$ is an $n\times p$ matrix of predictors,
- $\beta$ is a $p\times 1$ vector of coefficients.

The least squares solution is given by

$$
\hat{\beta}=(X^TX)^{-1}X^Ty
$$

provided that $X^TX$ is invertible.

## Intuitive Explanation

- **Error Minimization:**  
  The method minimizes the total error between the predicted and observed values by squaring the residuals. Squaring ensures that both positive and negative errors contribute equally to the total error.

- **Best-Fit Line:**  
  By minimizing the sum of squared errors, the method finds the line that best fits the data in the sense of least total deviation.

- **Projection Interpretation:**  
  In vector space, the observed data is projected onto the subspace spanned by the predictors. The coefficients obtained represent this projection and ensure the smallest possible error in the Euclidean sense.

## Python Code Example

Below is a Python example demonstrating how to perform linear regression using the least squares method with `numpy`.

```python
import numpy as np
import matplotlib.pyplot as plt

# Sample data
x = np.array([1, 2, 3, 4, 5])
y = np.array([2, 4, 5, 4, 5])

# Compute means
x_mean = np.mean(x)
y_mean = np.mean(y)

# Calculate coefficients using the normal equations
beta1 = np.sum((x - x_mean) * (y - y_mean)) / np.sum((x - x_mean)**2)
beta0 = y_mean - beta1 * x_mean

print("Estimated coefficients:")
print("beta0 (intercept):", beta0)
print("beta1 (slope):", beta1)

# Predicted values
y_pred = beta0 + beta1 * x

# Plot data and regression line
plt.scatter(x, y, color='blue', label='Data points')
plt.plot(x, y_pred, color='red', label='Least Squares Fit')
plt.xlabel('x')
plt.ylabel('y')
plt.title('Least Squares Linear Regression')
plt.legend()
plt.show()
