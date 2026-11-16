# Closed form Kalman gain for continuous Kalman filter with A = 0 and C = I

## Derivation

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

## Corner cases

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
