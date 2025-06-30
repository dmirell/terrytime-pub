**Capital multiplier** affects how much times initial [[Trading cycle capital|trading capital]] will be multiplied after trading cycle completed.
Because [[Trading cycle profit|profit]] calculations works differently for different [[Commission calculation|commission calculation models]] capital multiplier formulas also different.

## Payment by volume received

Profit is:
$$D=C (\frac{B_{s}}{B_{b}}(1-f)^2 - 1)$$
So capital multiplier is:
$$
M = \frac{B_s}{B_b} \cdot (1 - f)^2
$$

## Payment by third currency

Final [[Quote asset|quote]] volume estimates as:
$$D=C((\frac{B_{s}}{B_{b}}(1-f))- f-1)$$
Capital multiplier is:
$$M=\frac{B_{s}}{B_{b}}(1-f)- f$$

## Common
$$
M \uparrow \iff B_s - B_b \uparrow \quad \text{and} \quad M \downarrow \iff B_s - B_b \downarrow
$$
