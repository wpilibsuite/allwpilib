# Algorithms

## Simple motor feedforward

### Derivation

For a simple DC motor with the model

```
  dx/dt = −kᵥ/kₐ x + 1/kₐ u - kₛ/kₐ sgn(x),
```

where

```
  A = −kᵥ/kₐ
  B = 1/kₐ
  c = -kₛ/kₐ sgn(x)
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
  uₖ = B_d⁺(xₖ₊₁ − A_d xₖ) − kₐ(-(kₛ/kₐ sgn(x)))
  uₖ = B_d⁺(xₖ₊₁ − A_d xₖ) + kₐ(kₛ/kₐ sgn(x))
  uₖ = B_d⁺(xₖ₊₁ − A_d xₖ) + kₛ sgn(x)
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
  uₖ = B_d⁺(xₖ₊₁ − A_d xₖ) + kₛ sgn(x)
  uₖ = (1/kᵥ)⁺(xₖ₊₁ − (0) xₖ) + kₛ sgn(x)
  uₖ = (1/kᵥ)⁺(xₖ₊₁) + kₛ sgn(x)
  uₖ = kᵥxₖ₊₁ + kₛ sgn(x)
  uₖ = kₛ sgn(x) + kᵥxₖ₊₁
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

## Elevator feedforward

### Derivation

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

## Closed form Kalman gain for continuous Kalman filter with A = 0 and C = I

### Derivation

Model is

```
  dx/dt = Ax + Bu
  y = Cx + Du
```

where A = 0, B = 0, C = I, and D = 0.

The optimal cost-to-go is the P that satisfies

```
  AᵀP + PA − PBR⁻¹BᵀP + Q = 0
```

Let A = Aᵀ and B = Cᵀ for state observers.

```
  AP + PAᵀ − PCᵀR⁻¹CP + Q = 0
```

Let A = 0, C = I.

```
  −PR⁻¹P + Q = 0
```

Solve for P. P, Q, and R are all diagonal, so this can be solved element-wise.

```
  −pr⁻¹p + q = 0
  −pr⁻¹p = −q
  pr⁻¹p = q
  p²r⁻¹ = q
  p² = qr
  p = √(qr)
```

Now solve for the Kalman gain.

```
  K = PCᵀ(CPCᵀ + R)⁻¹
  K = P(P + R)⁻¹
  k = p(p + r)⁻¹
  k = √(qr)(√(qr) + r)⁻¹
  k = √(qr)/(√(qr) + r)
```

Multiply by √(q/r)/√(q/r).

```
  k = q/(q + r √(q/r))
  k = q/(q + √(qr²/r))
  k = q/(q + √(qr))
```

### Corner cases

For q = 0 and r ≠ 0,

```
  k = 0/(0 + √0)
  k = 0/0
```

Apply L'Hôpital's rule to k with respect to q.

```
  k = 1/(1 + r/(2 √(qr)))
  k = 2 √(qr)/(2 √(qr) + r)
  k = 2 √(0)/(2 √0 + r)
  k = 0/r
  k = 0
```

For q ≠ 0 and r = 0,

```
  k = q / (q + √0)
  k = q / q
  k = 1
```

## Quaternion to Euler angle conversion

### Conventions

We'll use the extrinsic X-Y-Z rotation order for Euler angles. The direction of rotation is CCW looking into the positive axis. If you point your right thumb along the positive axis direction, your fingers curl in the direction of rotation.

The angles are a\_x around the X-axis, a\_y around the Y-axis, and a\_z around the Z-axis, with the following constraints:

```
  -π ≤ a_x ≤ π
  -π/2 ≤ a_y ≤ π/2
  -π ≤ a_z ≤ π
```

The coordinate system is right-handed. If you point your right thumb along the +Z axis, your fingers curl from the +X axis to the +Y axis.

The quaternion imaginary numbers are defined as follows:

```
  îĵ = k̂
  ĵk̂ = î
  k̂î = ĵ
  îĵ = -k̂
  k̂ĵ = -î
  îk̂ = -ĵ
  î² = ĵ² = k̂² = -1
```

### Quaternion representation of axis rotations

We will take it as given that a rotation by θ radians around a normalized vector v is represented with the quaternion cos(θ/2) + sin(θ/2) (v\_x î + v\_y ĵ + v\_z k̂).

### Derivation

For convenience, we'll define the following variables:

```
  c_x = cos(a_x/2)
  s_x = sin(a_x/2)
  c_y = cos(a_y/2)
  s_y = sin(a_y/2)
  c_z = cos(a_z/2)
  s_z = sin(a_z/2)
```

We can calculate the quaternion corresponding to a set of Euler angles by applying each rotation in sequence. Recall that quaternions are composed with left multiplication, like matrices.

```
  q = (cos(a_z/2) + sin(a_z/2) k̂)(cos(a_y/2) + sin(a_y/2) ĵ)(cos(a_x/2) + sin(a_x/2) î)
  q = (c_z + s_z k̂)(c_y + s_y ĵ)(c_x + s_x î)
  q = (c_y c_z - s_y s_z î + s_y c_z ĵ + c_y s_z k̂)(c_x + s_x î)
    = (c_x c_y c_z + s_x s_y s_z)
      + (s_x c_y c_z - c_x s_y s_z) î
      + (c_x s_y c_z + s_x c_y s_z) ĵ
      + (c_x c_y s_z - s_x s_y c_z) k̂
```

Letting q = q\_w + q\_x î + q\_y ĵ + q\_z k̂, we can extract the components of the quaternion:

```
  q_w = c_x c_y c_z + s_x s_y s_z
  q_x = s_x c_y c_z - c_x s_y s_z
  q_y = c_x s_y c_z + s_x c_y s_z
  q_z = c_x c_y s_z - s_x s_y c_z
```

### Solving for a\_y

Solving for sin(a\_y):

```
  sin(a_y) = 2 c_y s_y
  sin(a_y) = 2 (c_x² c_y s_y + s_x² c_y s_y)
  sin(a_y) = 2 (c_x² c_y s_y c_z² + c_x² c_y s_y s_z²
              + s_x² c_y s_y c_z² + s_x² c_y s_y s_z²)
  sin(a_y) = 2 (c_x² c_y s_y c_z² + s_x² c_y s_y s_z²
              + s_x² c_y s_y c_z² + c_x² c_y s_y s_z²)
  sin(a_y) = 2 (c_x² c_y s_y c_z² + c_x s_x c_y² c_z s_z
              + c_x s_x s_y² c_z s_z + s_x² c_y s_y s_z²
              - c_x s_x c_y² c_z s_z + s_x² c_y s_y c_z²
              + c_x² c_y s_y s_z² - c_x s_x s_y² c_z s_z)
  sin(a_y) = 2 ((c_x c_y c_z + s_x s_y s_z)(c_x s_y c_z + s_x c_y s_z)
              - (s_x c_y c_z - c_x s_y s_z)(c_x c_y s_z - s_x s_y c_z))
  sin(a_y) = 2 (q_w q_y - q_x q_z)
```

Then solving for a\_y:

```
  a_y = sin⁻¹(sin(a_y))
  a_y = sin⁻¹(2 (q_w q_y - q_x q_z))
```

### Solving for a\_x and a\_z

Solving for cos(a\_x) cos(a\_y):

```
  cos(a_x) cos(a_y) = (cos²(a_x/2) - sin²(a_x/2))(cos²(a_y/2) - sin²(a_y/2))
  cos(a_x) cos(a_y) = (c_x² - s_x²)(c_y² - s_y²)
  cos(a_x) cos(a_y) = c_x² c_y² - c_x² s_y² - s_x² c_y² + s_x² s_y²
  cos(a_x) cos(a_y) = c_x² (1 - s_y²) - c_x² s_y² - s_x² c_y² + s_x² (1 - c_y²)
  cos(a_x) cos(a_y) = c_x² - c_x² s_y² - c_x² s_y² - s_x² c_y² + s_x² - s_x² c_y²
  cos(a_x) cos(a_y) = c_x² + s_x² - 2 (c_x² s_y² + s_x² c_y²)
  cos(a_x) cos(a_y) = 1 - 2 (c_x² s_y² + s_x² c_y²)
  cos(a_x) cos(a_y) = 1 - 2 (c_x² s_y² c_z² + c_x² s_y² s_z²
                           + s_x² c_y² c_z² + s_x² c_y² s_z²)
  cos(a_x) cos(a_y) = 1 - 2 (s_x² c_y² c_z² + c_x² s_y² s_z²
                           + c_x² s_y² c_z² + s_x² c_y² s_z²)
  cos(a_x) cos(a_y) = 1 - 2 (s_x² c_y² c_z² - 2 c_x s_x c_y s_y c_z s_z + c_x² s_y² s_z²
                           + c_x² s_y² c_z² + 2 c_x s_x c_y s_y c_z s_z + s_x² c_y² s_z²)
  cos(a_x) cos(a_y) = 1 - 2 ((s_x c_y c_z - c_x s_y s_z)² + (c_x s_y c_z + s_x c_y s_z)²)
  cos(a_x) cos(a_y) = 1 - 2 (q_x² + q_y²)
```

Solving for sin(a\_x) cos(a\_y):

```
  sin(a_x) cos(a_y) = (2 cos(a_x/2) sin(a_x/2))(cos²(a_y/2) - sin²(a_y/2))
  sin(a_x) cos(a_y) = (2 c_x s_x)(c_y² - s_y²)
  sin(a_x) cos(a_y) = 2 (c_x s_x c_y² - c_x s_x s_y²)
  sin(a_x) cos(a_y) = 2 (c_x s_x c_y² c_z² + c_x s_x c_y² s_z²
                       - c_x s_x s_y² c_z² - c_x s_x s_y² s_z²)
  sin(a_x) cos(a_y) = 2 (c_s s_x c_y² c_z² - c_x s_x s_y² s_z²
                       - c_x s_x s_y² c_z² + c_x s_x c_y² s_z²)
  sin(a_x) cos(a_y) = 2 (c_x s_x c_y² c_z² - c_x² c_y s_y c_z s_z
                       + s_x² c_y s_y c_z s_z - c_x s_x s_y² s_z²
                       + c_x² c_y s_y c_z s_z - c_x s_x s_y² c_z²
                       + c_x s_x c_y² s_z² - s_x² c_y s_y c_z s_z)
  sin(a_x) cos(a_y) = 2 ((c_x c_y c_z + s_x s_y s_z)(s_x c_y c_z - c_x s_y s_z)
                       + (c_x s_y c_z + s_x c_y s_z)(c_x c_y s_z - s_x s_y c_z))
  sin(a_x) cos(a_y) = 2 (q_w q_x + q_y q_z)
```

Similarly, solving for cos(a\_z) cos(a\_y):

```
  cos(a_z) cos(a_y) = (cos²(a_z/2) - sin²(a_z/2))(cos²(a_y/2) - sin²(a_y/2))
  cos(a_z) cos(a_y) = (c_z² - s_z²)(c_y² - s_y²)
  cos(a_z) cos(a_y) = c_y² c_z² - s_y² c_z² - c_y² s_z² + s_y² s_z²
  cos(a_z) cos(a_y) = c_y² (1 - s_z²) - s_y² c_z² - c_y² s_z² + s_y² (1 - c_z²)
  cos(a_z) cos(a_y) = c_y² - c_y² s_z² - s_y² c_z² - c_y² s_z² + s_y² - s_y² c_z²
  cos(a_z) cos(a_y) = c_y² + s_y² - 2 (c_y² s_z² + s_y² c_z²)
  cos(a_z) cos(a_y) = 1 - 2 (c_y² s_z² + s_y² c_z²)
  cos(a_z) cos(a_y) = 1 - 2 (c_x² c_y² s_z² + s_x² c_y² s_z²
                           + c_x² s_y² c_z² + s_x² s_y² c_z²)
  cos(a_z) cos(a_y) = 1 - 2 (c_x² s_y² c_z² + s_x² c_y² s_z²
                           + c_x² c_y² s_z² + s_x² s_y² c_z²)
  cos(a_z) cos(a_y) = 1 - 2 (c_x² s_y² c_z² + 2 c_x s_x c_y s_y c_z s_z + s_x² c_y² s_z²
                           + c_x² c_y² s_z² - 2 c_x s_x c_y s_y c_z s_z + s_x² s_y² c_z²)
  cos(a_z) cos(a_y) = 1 - 2 ((c_x s_y c_z + s_x c_y s_z)² + (c_x c_y s_z - s_x s_y c_z)²)
  cos(a_z) cos(a_y) = 1 - 2 (q_y² + q_z²)
```

Similarly, solving for sin(a\_z) cos(a\_y):

```
  sin(a_z) cos(a_y) = (2 cos(a_z/2) sin(a_z/2))(cos²(a_y/2) - sin²(a_y/2))
  sin(a_z) cos(a_y) = (2 c_z s_z)(c_y² - s_y²)
  sin(a_z) cos(a_y) = 2 (c_y² c_z s_z - s_y² c_z s_z)
  sin(a_z) cos(a_y) = 2 (c_x² c_y² c_z s_z + s_x² c_y² c_z s_z
                       - c_x² s_y² c_z s_z - s_x² s_y² c_z s_z)
  sin(a_z) cos(a_y) = 2 (c_x² c_y² c_z s_z - s_x² s_y² c_z s_z
                       + s_x² c_y² c_z s_z - c_x² s_y² c_z s_z)
  sin(a_z) cos(a_y) = 2 (c_x² c_y² c_z s_z - c_x s_x c_y s_y c_z²
                       + c_x s_x c_y s_y s_z² - s_x² s_y² c_z s_z
                       + c_x s_x c_y s_y c_z² + s_x² c_y² c_z s_z
                       - c_x² s_y² c_z s_z - c_x s_x c_y s_y s_z²)
  sin(a_z) cos(a_y) = 2 ((c_x c_y c_z + s_x s_y s_z)(c_x c_y s_z - s_x s_y c_z)
                       + (s_x c_y c_z - c_x s_y s_z)(c_x s_y c_z + s_x c_y s_z))
  sin(a_z) cos(a_y) = 2 (q_w q_z + q_x q_y)
```

Solving for a\_x and a\_z:

```
  a_x = atan2(sin(a_x), cos(a_x))
  a_z = atan2(sin(a_z), cos(a_z))
```

If cos(a\_y) > 0:

```
  a_x = atan2(sin(a_x) cos(a_y), cos(a_x) cos(a_y))
  a_z = atan2(sin(a_z) cos(a_y), cos(a_z) cos(a_y))
  a_x = atan2(2 (q_w q_x + q_y q_z), 1 - 2 (q_x² + q_y²))
  a_z = atan2(2 (q_w q_z + q_x q_y), 1 - 2 (q_y² + q_z²))
```

Because -π/2 ≤ a\_y ≤ π/2, cos(a\_y) ≥ 0. Therefore, the only remaining case is cos(a\_y) = 0, whose only solutions in that range are a\_y = ±π/2.

```
  a_y = ±π/2
  a_y/2 = ±π/4
  cos(a_y/2) = √2/2
  c_y = √2/2
  sin(a_y/2) = ±√2/2
  s_y = ±√2/2
```

Plugging into the quaternion components:

```
  q_w = c_x c_y c_z + s_x s_y s_z
  q_x = s_x c_y c_z - c_x s_y s_z
  q_y = c_x s_y c_z + s_x c_y s_z
  q_z = c_x c_y s_z - s_x s_y c_z
  q_w = √2/2 c_x c_z ± √2/2 s_x s_z
  q_x = √2/2 s_x c_z ∓ √2/2 c_x s_z
  q_y = ±√2/2 c_x c_z + √2/2 s_x s_z
  q_z = √2/2 c_x s_z ∓ √2/2 s_x c_z
  q_w = √2/2 (c_x c_z ± s_x s_z)
  q_x = √2/2 (s_x c_z ∓ c_x s_z)
  q_y = √2/2 (± c_x c_z + s_x s_z)
  q_z = √2/2 (c_x s_z ∓ s_x c_z)
  q_w = √2/2 cos(a_z/2 ∓ a_x/2)
  q_x = √2/2 sin(a_x/2 ∓ a_z/2)
  q_y = √2/2 -cos(a_x/2 ∓ a_z/2)
  q_z = √2/2 sin(a_z/2 ∓ a_x/2)
```

In either case only the sum or the difference between a\_x and a\_z can be determined. We'll pick the solution where a\_x = 0.

```
  q_w = √2/2 cos(a_z/2 ∓ 0)
  q_w = √2/2 cos(a_z/2)
  cos(a_z/2) = √2 q_w
  q_z = √2/2 sin(a_z/2 ∓ 0)
  q_z = √2/2 sin(a_z/2)
  sin(a_z/2) = √2 q_z
  cos(a_z) = cos²(a_z/2) - sin²(a_z/2)
  cos(a_z) = (√2 q_w)² - (√2 q_z)²
  cos(a_z) = 2 q_w² - 2 q_z²
  cos(a_z) = 2 (q_w² - q_z²)
  sin(a_z) = 2 cos(a_z/2) sin(a_z/2)
  sin(a_z) = 2 (√2 q_w) (√2 q_z)
  sin(a_z) = 4 q_w q_z
  a_z = atan2(4 q_w q_z, 2 (q_w² - q_z²))
  a_z = atan2(2 q_w q_z, q_w² - q_z²)
```

### Determining if cos(a\_y) ≈ 0

When calculating a\_x:

```
  cos(a_y) ≈ 0
  cos²(a_y) ≈ 0
  cos²(a_x) cos²(a_y) + sin²(a_x) cos²(a_y) ≈ 0
  (cos(a_x) cos(a_y))² + (sin(a_x) cos(a_y))² ≈ 0
```

Note that this reuses the cos(a\_x) cos(a\_y) and sin(a\_x) cos(a\_y) terms needed to calculate a\_x.

When calculating a\_z:

```
  cos(a_y) ≈ 0
  cos²(a_y) ≈ 0
  cos²(a_y) cos²(a_z) + cos²(a_y) sin²(a_z) ≈ 0
  (cos(a_y) cos(a_z))² + (cos(a_y) sin(a_z))² ≈ 0
```

Note that this reuses the cos(a\_y) cos(a\_z) and cos(a\_y) sin(a\_z) terms needed to calculate a\_z.

## Quaternion Exponential

We will take it as given that a quaternion has scalar and vector components `𝑞 = s + 𝑣⃗`, with vector component 𝑣⃗ consisting of a unit vector and magnitude `𝑣⃗ = θ * v̂`.

```
𝑞 = s + 𝑣⃗

𝑣⃗ = θ * v̂

exp(𝑞) = exp(s + 𝑣⃗)
exp(𝑞) = exp(s) * exp(𝑣⃗)
exp(𝑞) = exp(s) * exp(θ * v̂)
```

Applying euler's identity:

```
exp(θ * v̂) = cos(θ) + sin(θ) * v̂
```

Gives us:
```
exp(𝑞) = exp(s) * [cos(θ) + sin(θ) * v̂]
```

Rearranging `𝑣⃗ = θ * v̂` we can solve for v̂: `v̂ = 𝑣⃗ / θ`

```
exp(𝑞) = exp(s) * [cos(θ) + sin(θ) / θ * 𝑣⃗]
```

## Quaternion Logarithm

We will take it as a given that for a given quaternion of the form `𝑞 = s + 𝑣⃗`, we can calculate the exponential: `exp(𝑞) = exp(s) * [cos(θ) + sin(θ) / θ * 𝑣⃗]` where `θ = ||𝑣⃗||`.

Additionally, `exp(log(𝑞)) = q` for a given value of `log(𝑞)`. There are multiple solutions to `log(𝑞)` caused by the imaginary axes in 𝑣⃗, discussed here: https://en.wikipedia.org/wiki/Complex_logarithm

We will demonstrate the principal solution of `log(𝑞)` satisfying `exp(log(𝑞)) = q`.
This being `log(𝑞) = log(||𝑞||) + atan2(θ, s) / θ * 𝑣⃗`, is the principal solution to `log(𝑞)` because the function `atan2(θ, s)` returns the principal value corresponding to its arguments.

Proof: `log(𝑞) = log(||𝑞||) + atan2(θ, s) / θ * 𝑣⃗` satisfies `exp(log(𝑞)) = q`.

```
exp(log(𝑞)) = exp(log(||𝑞||) + atan2(θ, s) / θ * 𝑣⃗)


exp(log(𝑞)) = exp(log(||𝑞||)) * exp(atan2(θ, s) / θ * 𝑣⃗)

Substitutions:
𝑣⃗ = θ * v̂:
exp(log(||𝑞||)) = ||𝑞||
exp(log(𝑞)) = ||𝑞|| * exp(atan2(θ, s) * v̂)

exp(log(𝑞)) = ||𝑞|| * [cos(atan2(θ, s)) + sin(atan2(θ, s)) * v̂]

Substitutions:
cos(atan2(θ, s)) = s / √(θ² + s²)
sin(atan2(θ, s)) = θ / √(θ² + s²)

exp(log(𝑞)) = ||𝑞|| * [s / √(θ² + s²) + θ / √(θ² + s²) * v̂]

√(θ² + s²) = ||𝑞||

exp(log(𝑞)) = ||𝑞|| * [s / ||𝑞|| + θ / ||𝑞|| * v̂]
exp(log(𝑞)) = s + θ * v̂

exp(log(𝑞)) = s + 𝑣⃗

exp(log(𝑞)) = 𝑞
```

## Unit Quaternion in SO(3) from Rotation Vector in 𝖘𝖔(3)

We will take it as a given that members of 𝖘𝖔(3) take the form `𝑣⃗ = θ * v̂`, representing a rotation θ around a unit axis v̂.

We additionally take it as a given that quaternions in SO(3) are of the form `𝑞 = cos(θ / 2) + sin(θ / 2) * v̂`, representing a rotation of θ around unit axis v̂.

```
θ = ||𝑣⃗||
v̂ = 𝑣⃗ / θ

𝑞 = cos(θ / 2) + sin(θ / 2) * v̂
𝑞 = cos(||𝑣⃗|| / 2) + sin(||𝑣⃗|| / 2) / ||𝑣⃗|| * 𝑣⃗
```

## Rotation vector in 𝖘𝖔(3) from Unit Quaternion in SO(3)

We will take it as a given that members of 𝖘𝖔(3) take the form  `𝑟⃗ = θ * r̂`, representing a rotation θ around a unit axis r̂.

We additionally take it as a given that quaternions in SO(3) are of the form `𝑞 = s + 𝑣⃗ = cos(θ / 2) + sin(θ / 2) * v̂`, representing a rotation of θ around unit axis v̂.

```
s + 𝑣⃗ = cos(θ / 2) + sin(θ / 2) * v̂
s = cos(θ / 2)
𝑣⃗ = sin(θ / 2) * v̂
||𝑣⃗|| = sin(θ / 2)

θ / 2 = atan2(||𝑣⃗||, s)
θ = 2 * atan2(||𝑣⃗||, s)

r̂ = 𝑣⃗ / ||𝑣⃗||

𝑟⃗ = θ * r̂
𝑟⃗ = 2 * atan2(||𝑣⃗||, s) / ||𝑣⃗|| * 𝑣⃗
```

## Closed form solution for an Exponential Motion Profile

### [Derivation of continuous-time model](wpimath/algorithms/ExponentialProfileModel.py)


### Heuristic for input direction in Exponential Profile

Demonstration: https://www.desmos.com/calculator/3jamollwrk

The fastest path possible for an exponential profile (and the placement of the inflection point) depend on boundary conditions.

Specifically, the placement (xf, vf) relative to the possible trajectories that cross through (x0, v0) decides this. There are two possible trajectories to take from the initial state. In the desmos demo these are colored Green and Purple, which arise from applying +input and -input from the initial state respectively. Red and Yellow trajectories arise from applying -input and +input respectively from terminal conditions.

In order to reach the terminal state from the initial state by following Green in the +v direction, the second step is following Red in the -v direction.
Likewise, Purple must be followed in the -v direction, and then Yellow must be followed in the +v direction.

The specific conditions surrounding this decision are fourfold:
- A: v0 >= 0
- B: vf >= 0
- C: vf >= x1_ps(vf, U)
- D: vf >= x1_ps(vf, -U)

Where x1_ps(v, U) follows the Green line, and x1_ps(v, -U) follows the Purple line.

This creates a decision table:
| v0>=0 | vf>=0 | vf>=x1_ps(vf,U) | vf>=x1_ps(vf,-U) | Output Sign |
|-------|-------|-----------------|------------------|------------:|
| False | False | False           | False            |          -1 |
| False | False | False           | True             |           1 |
| False | False | True            | False            |           1 |
| False | False | True            | True             |           1 |
| False | True  | False           | False            |          -1 |
| False | True  | False           | True             |          -1 |
| False | True  | True            | False            |           1 |
| False | True  | True            | True             |           1 |
| True  | False | False           | False            |          -1 |
| True  | False | False           | True             |           1 |
| True  | False | True            | False            |          -1 |
| True  | False | True            | True             |           1 |
| True  | True  | False           | False            |          -1 |
| True  | True  | False           | True             |          -1 |
| True  | True  | True            | False            |          -1 |
| True  | True  | True            | True             |           1 |

Which is equivalent to `-1 if (A & ~D) | (B & ~C) | (~C & ~D) else 1`.

## Trapezoid Profile

The fastest possible profile for the double integrator is one that applies the maximum allowed input in one direction and then the maximum allowed input in the other, also known as bang-bang. If there is an active maximum velocity constraint, this becomes bang-zero-bang. Notice that the plot of velocity versus time for this case resembles a trapezoid, thus the name.

We will utilize the subscript m to denote the maximum allowed acceleration (`a_m`) and velocity (`v_m`). To refer to the most extreme velocity achieved by the profile we will use the term peak velocity (`v_p`). Occuring when the acceleration of a profile switches sign, it will be either greater than or equal to, or less than or equal to both the initial and target velocity depending on the profile sign (more on this later).

First, let us derive the dynamics of the system. Our control input is acceleration, which as discussed earlier, will take the form of a piecewise constant over the time of the profile. Integrating constant acceleration with respect to time yields equation (1).
```
v = v_i + at   (1)
```
Integrating this once again gives equation (2).
```
x = x_i + v_i t + at²/2   (2)
```
We can solve (1) for `t` to get `t = (v - v_i)/a`. Substituting this into (2) and cleaning up the result yields
```
x = x_i + v_i((v - v_i)/a) + a((v - v_i) / a)²/2
x = x_i + (v_i v - v_i²)/a + (v² - 2vv_i + v_i²)/(2a)
(x - x_i)(2a) = v² - v_i²
2aΔx = v_t² - v_i²                                      (3)
```
This is the primary equation of motion we will use in this derivation. The subscripts of t and i denote target and initial respectively, and `Δx` denotes the displacement from an initial state to a target state.

### Determining the sign of the profile.

The sign of the profile can be defined as the sign of the acceleration of the first segment of the profile. If we separate the profile into segments based on the value of the input, the input applied for the first, second, and third sections can be found by mulitplying `a_m` by `s`, `0`, and `-s` respectively. From this, we can see that for profiles with a positive sign, the peak velocity would be greater than or equal to both the initial and the target velocity, and for profiles with a negative sign, the peak velocity would be less than or equal to the initial and target velocities.

The optimal sign of the profile can be determined by looking at the distance covered by the shortest profile that can connect the initial and target velocity while respecting the acceleration constraint. A profile that takes more time than this with a positive sign will either increase the displacement of the profile, or has a faster profile with a negative sign. Likewise, a profile with a negative sign will either decrease the displacement or has a faster profile with a positive sign. This minimum profile takes the form of a straight line in the velocity versus time plot and has an acceleration equal to `sign(v_t - v_i)a_m`. This threshold distance (`d`) is derived below.
```
2sign(v_t - v_i)a_m d = v_t² - v_i²
2sign(v_t - v_i)a_m d = (v_t - v_i)(v_t + v_i)
2sign(v_t - v_i)a_m d = sign(v_t - v_i)|v_t - v_i|(v_t + v_i)
2a_m d = |v_t - v_i|(v_t + v_i)
d = |v_t - v_i|(v_t + v_i)/(2a_m)                               (4)
```
Recall that occasionally there can be both a profile with a positive sign and a profile with a negative sign that successfully transition between the initial and target states. To understand this, imagine a profile where the initial and target velocities are above zero. A profile that has a negative sign and a time just a little bit over the minimum valid time will still end up with a positive displacement. Now consider if the profile had a positive sign. The same displacement would be possible to cover faster because the average velocity would be higher. Note that for the formulation discussed here, ambiguity only arises when both the initial and target velocities have the same signs.

While comparing with the threshold distance will handle the majority of these cases, solely relying on it cannot handle the case where the initial and target states make the minimum profile. While it is not common for two random states to give rise to this, it is relatively common when profiles are being generated from a reference on the final segment of a profile. If floating point error causes the state to be slightly above or below the threshold distance, the sign is properly determined and the next reference is guaranteed to be correct (within floating point tolerances); however, in the case it is equal and the wrong sign is chosen, a reference for a new, longer profile may be generated. This can lead to choatic input sign changes and prevent the profile from coming to rest. This can be avoided by preferring the negative sign when both state velocities are below zero, and a positive sign otherwise. Because the scenario with different initial signs has one valid profile, meaning either sign will lead to valid solutions within floating point tolerance, this preference can be simplified to only check the sign of the target velocity.

### Determining the peak velocity

In order to find the peak velocity (`v_p`), let us first define `a = sa_m` and that the profile displacement (`Δx`) be separated into segments based on the value of the input. Let the subscripts 1, 2, and 3, indicate the first section, the optional second section, and the third section respectively.
```
Δx = x_1 + x_2 + x_3   (5)
```
To determine the if the profile has an active velocity constraint, we must first calculate the peak velocity as if it didn't. To start, we substitute in `x_2 = 0`.
```
Δx = x_1 + x_3   (6)
```
where
```
2ax_1 = v_p² - v_i²
x_1 = (v_p² - v_i²)/(2a)
```
and
```
-2ax_3 = v_t² - v_p²
2ax_3 = v_p² - v_t²
x_3 = (v_p² - v_t²)/(2a)
```
Substituting these into (6) yields
```
(v_p² - v_i²)/(2a) + (v_p² - v_t²)/(2a) = Δx
(2v_p² - (v_t² + v_i²))/(2a) = Δx
2v_p² - (v_t² + v_i²) = 2aΔx
2v_p² = 2aΔx + v_t² + v_i²
v_p² = aΔx + (v_t² + v_i²)/2
v_p = √(aΔx + (v_t² + v_i²)/2)                 (7)
```
For the case where v_p exceeds the the velocity limit, letting `v_l = s v_m` means the values of `x_1` and `x_3` can be found by substituting `v_p = v_l`
```
x_1 = (v_l² - v_i²) / (2 a)   (8)
x_3 = (v_l² - v_t²) / (2 a)   (9)
```
which can be used to find x_2 by rearranging (5) to get
```
x_1 + x_2 + x_3 = Δx
x_2 = Δx - x_1 - x_3   (10)
```
