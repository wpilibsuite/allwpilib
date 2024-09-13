# OLS derivations

## Simple/drivetrain

Here's the ODE for a drivetrain.
```
dx/dt = -Kv/Ka x + 1/Ka u - Ks/Ka sgn(x)
```

### OLS setup

Let `α = -Kv/Ka`, `β = 1/Ka`, and `γ = -Ks/Ka`.
```
dx/dt = αx + βu + γ sgn(x)
```

### Feedforward gains

Divide the OLS terms by each other to obtain `Ks`, `Kv`, and `Ka`.
```
Ks = -γ/β
Kv = -α/β
Ka = 1/β
```

## Elevator

Here's the ODE for an elevator.
```
dx/dt = -Kv/Ka x + 1/Ka u - Ks/Ka sgn(x) - Kg/Ka
```

### OLS setup

Let `α = -Kv/Ka`, `β = 1/Ka`, `γ = -Ks/Ka`, and `δ = -Kg/Ka`.
```
dx/dt = αx + βu + γ sgn(x) + δ
```

### Feedforward gains

Divide the OLS terms by each other to obtain `Ks`, `Kv`, `Ka`, and `Kg`.
```
Ks = -γ/β
Kv = -α/β
Ka = 1/β
Kg = −δ/β
```

## Arm

Here's the ODE for an arm:
```
dx/dt = -Kv/Ka x + 1/Ka u - Ks/Ka sgn(x) - Kg/Ka cos(angle)
```

If the arm encoder doesn't read zero degrees when the arm is horizontal, the fit
for `Kg` will be wrong. An angle offset should be added to the model like so.
```
dx/dt = -Kv/Ka x + 1/Ka u - Ks/Ka sgn(x) - Kg/Ka cos(angle + offset)
```

Use a trig identity to split the cosine into two terms.
```
dx/dt = -Kv/Ka x + 1/Ka u - Ks/Ka sgn(x) - Kg/Ka (cos(angle) cos(offset) - sin(angle) sin(offset))
dx/dt = -Kv/Ka x + 1/Ka u - Ks/Ka sgn(x) - Kg/Ka cos(angle) cos(offset) + Kg/Ka sin(angle) sin(offset)
```

Reorder multiplicands so the offset trig is absorbed by the OLS terms.
```
dx/dt = -Kv/Ka x + 1/Ka u - Ks/Ka sgn(x) - Kg/Ka cos(offset) cos(angle) + Kg/Ka sin(offset) sin(angle)
```

### OLS setup

Let `α = -Kv/Ka`, `β = 1/Ka`, `γ = -Ks/Ka`, `δ = -Kg/Ka cos(offset)`, and `ε = Kg/Ka sin(offset)`.
```
dx/dt = αx + βu + γ sgn(x) + δ cos(angle) + ε sin(angle)
```

### Feedforward gains: Ks, Kv, Ka

Divide the OLS terms by each other to obtain `Ks`, `Kv`, and `Ka`.
```
Ks = -γ/β
Kv = -α/β
Ka = 1/β
```

### Feedforward gains: Kg

Take the sum of squares of the OLS terms containing the angle offset. The angle
offset trig functions will form a trig identity that cancels out. Then, just
solve for `Kg`.
```
δ²+ε² = (-Kg/Ka cos(offset))² + (Kg/Ka sin(offset))²
δ²+ε² = (-Kg/Ka)² cos²(offset) + (Kg/Ka)² sin²(offset)
δ²+ε² = (Kg/Ka)² cos²(offset) + (Kg/Ka)² sin²(offset)
δ²+ε² = (Kg/Ka)² (cos²(offset) + sin²(offset))
δ²+ε² = (Kg/Ka)² (1)
δ²+ε² = (Kg/Ka)²
√(δ²+ε²) = Kg/Ka
hypot(δ, ε) = Kg/Ka
hypot(δ, ε) = Kg β
Kg = hypot(δ, ε)/β
```

### Feedforward gains: offset

Divide ε by δ, combine the trig functions into `tan(offset)`, then use `atan2()`
to preserve the angle quadrant. Maintaining the proper negative signs in the
numerator and denominator are important for obtaining the correct result.
```
δ = -Kg/Ka cos(offset)
ε = Kg/Ka sin(offset)
sin(offset)/-cos(offset) = ε/δ
sin(offset)/cos(offset) = ε/-δ
tan(offset) = ε/-δ
offset = atan2(ε, -δ)
```
