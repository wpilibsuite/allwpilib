# Algorithms

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
