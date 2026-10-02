# Elevator Feedforward

## Derivation

For an elevator with the model

```
  dx/dt = −kᵥ/kₐ x + 1/kₐ u - kg/kₐ - kₛ/kₐ sgn(x)
```

where

```
  A = −kᵥ/kₐ
  B = 1/kₐ
  c = -(kg/kₐ + kₛ/kₐ sgn(x))
  A_d = eᴬᵀ
  B_d = A⁻¹(eᴬᵀ - I)B
  dx/dt = Ax + Bu + c
```

Discretize the affine model.

```
  dx/dt = Ax + Bu + c
  dx/dt = Ax + B(u + B⁺c)
  xₖ₊₁ = eᴬᵀxₖ + A⁻¹(eᴬᵀ - I)B(uₖ + B⁺cₖ)
  xₖ₊₁ = A_d xₖ + B_d (uₖ + B⁺cₖ)
  xₖ₊₁ = A_d xₖ + B_d uₖ + B_d B⁺cₖ
```

Solve for uₖ.

```
  B_d uₖ = xₖ₊₁ − A_d xₖ − B_d B⁺cₖ
  uₖ = B_d⁺(xₖ₊₁ − A_d xₖ − B_d B⁺cₖ)
  uₖ = B_d⁺(xₖ₊₁ − A_d xₖ) − B⁺cₖ
```

Substitute in B assuming sgn(x) is a constant for the duration of the step.

```
  uₖ = B_d⁺(xₖ₊₁ − A_d xₖ) − kₐ(-(kg/kₐ + kₛ/kₐ sgn(x)))
  uₖ = B_d⁺(xₖ₊₁ − A_d xₖ) + kₐ(kg/kₐ + kₛ/kₐ sgn(x))
  uₖ = B_d⁺(xₖ₊₁ − A_d xₖ) + kg + kₛ sgn(x)
```

Simplify the model when kₐ = 0.

Simplify A.

```
  A = −kᵥ/kₐ
```

As kₐ approaches zero, A approaches -∞.

```
  A = −∞
```

Simplify A_d.

```
  A_d = eᴬᵀ
  A_d = exp(−∞)
  A_d = 0
```

Simplify B_d.

```
  B_d = A⁻¹(eᴬᵀ - I)B
  B_d = A⁻¹((0) - I)B
  B_d = A⁻¹(-I)B
  B_d = -A⁻¹B
  B_d = -(−kᵥ/kₐ)⁻¹(1/kₐ)
  B_d = (kᵥ/kₐ)⁻¹(1/kₐ)
  B_d = kₐ/kᵥ(1/kₐ)
  B_d = 1/kᵥ
```

Substitute these into the feedforward equation.

```
  uₖ = B_d⁺(xₖ₊₁ − A_d xₖ) + kg + kₛ sgn(x)
  uₖ = (1/kᵥ)⁺(xₖ₊₁ − (0) xₖ) + kg + kₛ sgn(x)
  uₖ = (1/kᵥ)⁺(xₖ₊₁) + kg + kₛ sgn(x)
  uₖ = kᵥxₖ₊₁ + kg + kₛ sgn(x)
  uₖ = kₛ sgn(x) + kg + kᵥxₖ₊₁
```

Simplify the model when kₐ ≠ 0

```
  uₖ = B_d⁺(xₖ₊₁ − A_d xₖ)
```

where

```
  A = −kᵥ/kₐ
  B = 1/kₐ
  A_d = eᴬᵀ
  B_d = A⁻¹(eᴬᵀ - I)B
```

When kᵥ = 0, A = 0 and B_d has a singularity. We can eliminate the singularity using the matrix exponential discretization method.

```
 [A  B]
 [0  0]T   [A_d  B_d]
e        = [ 0    I ]

 [0  B]
 [0  0]T   [1  BT]
e        = [0   1]

A_d = 1
B_d = BT
```
