In this article will be calculated which [[commission calculation]] better to chose and in which cases.
To determine that let's take [[capital multiplier]] formulas for each of case:
1. Payment by volume received:
$$
M_1 = \frac{B_s}{B_b} \cdot (1 - f_1)^2
$$
2. Payment by third currency:
$$M_2=\frac{B_{s}}{B_{b}}(1-f_2)- f_2$$
To consider buy and sell price the same and focus only on which commission type is more profitable, let's claim:
$$x=\frac{B_s}{B_b},$$
Now let's see when $M_1 > M_2$ :
The initial inequality:
$$
M_1 > M_2 \Rightarrow x(1 - f_1)^2 > x(1 - f_2) - f_2 \quad | \quad x>0, \quad 0 \le f_1, f_2 \le 1
$$

1. Simplify the inequality. For $x > 0$, we can divide both sides by $x$:
$$
(1 - f_1)^2 > (1 - f_2) - \frac{f_2}{x}
$$

2. Rewrite this inequality as:
$$
(1 - f_1)^2 - (1 - f_2) > - \frac{f_2}{x}
$$

3. Multiply both sides by $-1$:
$$
\frac{f_2}{x} > (1 - f_2) - (1 - f_1)^2
$$

4. From this inequality, we can express $x$ as:
$$
x < \frac{f_2}{(1 - f_2) - (1 - f_1)^2}
$$

5. Therefore, for $M_1 > M_2$ to hold, $x$ must be less than the following limit:
$$
x < \frac{f_2}{(1 - f_2) - (1 - f_1)^2}
$$

