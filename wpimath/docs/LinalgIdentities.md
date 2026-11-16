# Linear Algebra Identities

## Left Divide

`X = A\B` is a left divide of `AX = B` via `X = A⁻¹B`. It's called a left divide because the left argument is inverted.

A left divide in software looks like:
```
X = solve(A, B)
```

## Right Divide

`X = A/B` is a right divide of `XB = A` via `X = AB⁻¹`. It's called a right divide because the right argument is inverted.

A right divide in software looks like:
```
X = solve(Bᵀ, Aᵀ)ᵀ
```

## Converting Right Divide to Left Divide

```
  X = A/B
  XB = A
  BᵀXᵀ = Aᵀ
  Xᵀ = Bᵀ\Aᵀ
  X = (Bᵀ\Aᵀ)ᵀ
```
Thus,
```
  A/B = (Bᵀ\Aᵀ)ᵀ
```

## Converting Left Divide to Right Divide

```
  X = A\B
  AX = B
  XᵀAᵀ = Bᵀ
  Xᵀ = Bᵀ/Aᵀ
  X = (Bᵀ/Aᵀ)ᵀ
```
Thus,
```
  A\B = (Bᵀ/Aᵀ)ᵀ
```
