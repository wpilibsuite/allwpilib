# Trapezoid Profile

## [Interactive Desmos Graph](https://www.desmos.com/calculator/hcj8m3jycn)

### Overview

The fastest possible profile for the double integrator is one that applies the maximum allowed input in one direction and then the maximum allowed input in the other, also known as bang-bang.
 If there is an active maximum velocity constraint, this becomes bang-zero-bang.
 Notice that the plot of velocity versus time for this case can resemble a trapezoid, thus the name.

We will utilize the subscript m to denote the maximum allowed acceleration magnitude (`a_m`) and velocity magnitude (`v_m`).
 To refer to the most extreme velocity achieved by a profile without an active velocity constraint we will use the term peak velocity (`v_p`).
 Occurring when the acceleration of a profile switches sign, it will be either greater than or equal to, or less than or equal to both the initial and target velocity depending on the profile sign (more on this later).

First, let us derive the dynamics of the system.
 Our control input is acceleration, which as discussed earlier, will take the form of a piecewise constant over the time of the profile.
 Integrating constant acceleration with respect to time yields equation (1).
```
v = v_i + at   (1)
```
Integrating this once again gives equation (2).
```
x = x_i + v_i t + at²/2   (2)
```
We can solve (1) for `t` to get `t = (v - v_i)/a`.
 Substituting this into (2) and cleaning up the result yields equation (3).
```
x = x_i + v_i((v - v_i)/a) + a((v - v_i)/a)²/2
x = x_i + (v_i v - v_i²)/a + (v² - 2vv_i + v_i²)/(2a)
x = x_i + (v² + v_i²)/(2a) + (-v_i²)/a + vv_i/a - vv_i/a
x = x_i + (v² - v_i²)/(2a)
x - x_i = (v² - v_i²)/(2a)
(x - x_i)(2a) = v² - v_i²
2aΔx = v_t² - v_i²                                          (3)
```
This is the primary equation of motion we will use in this derivation.
 The subscripts of t and i denote target and initial respectively, and `Δx` denotes the displacement from an initial state to a target state.

### The sign of the profile

The sign of the profile can be defined as the sign of the acceleration of the first segment of the profile.
 If we separate the profile into segments based on the value of the input, the input applied for the first, second, and third sections can be found by multiplying `a_m` by `s`, `0`, and `-s` respectively.
 From this, we can see that for profiles with a positive sign, the peak velocity would be greater than or equal to both the initial and the target velocity, and for profiles with a negative sign, the peak velocity would be less than or equal to the initial and target velocities.
 We will cover how to determine the sign of the profile later in the documentation.

![PositiveProfileExample.png](PositiveProfileExample.png)

Positive profile sign.

![NegativeProfileExample.png](NegativeProfileExample.png)

Negative profile sign.

### Determining the peak velocity

In order to find the peak velocity (`v_p`), let us first define `a = sa_m` and that the profile displacement (`Δx`) be separated into segments based on the value of the input.
 Let the subscripts 1, 2, and 3, indicate the first section, the optional second section, and the third section respectively.
```
Δx = x_1 + x_2 + x_3   (4)
```
To determine the if the profile has an active velocity constraint, we must first calculate the peak velocity as if it didn't.
 To start, we substitute in `x_2 = 0`.
```
Δx = x_1 + x_3   (5)
```
where
```
2ax_1 = v_p² - v_i²
x_1 = (v_p² - v_i²)/(2a)   (6)
```
and
```
-2ax_3 = v_t² - v_p²
2ax_3 = v_p² - v_t²
x_3 = (v_p² - v_t²)/(2a)   (7)
```
Substituting these into (5) yields
```
(v_p² - v_i²)/(2a) + (v_p² - v_t²)/(2a) = Δx
(2v_p² - (v_t² + v_i²))/(2a) = Δx
2v_p² - (v_t² + v_i²) = 2aΔx
2v_p² = 2aΔx + v_t² + v_i²
v_p² = aΔx + (v_t² + v_i²)/2
v_p = s√(aΔx + (v_t² + v_i²)/2)                 (8)
```

### Determining the sign of the profile

The optimal sign of the profile can be determined by looking at the distance covered by the shortest profile that can connect the initial and target velocities while respecting the acceleration constraint.
 This minimum profile takes the form of a straight line in the velocity versus time plot with an acceleration equal to `sign(v_t - v_i)a_m`.
 The sign of the optimal profile for all displacements greater than that of the minimum profile is positive, and the sign of optimal profile for all displacements less than that of the minimum profile is negative.
 This threshold distance (`d`) is derived below.
```
2sign(v_t - v_i)a_m d = v_t² - v_i²
2sign(v_t - v_i)a_m d = (v_t - v_i)(v_t + v_i)
2sign(v_t - v_i)a_m d = sign(v_t - v_i)|v_t - v_i|(v_t + v_i)
2a_m d = |v_t - v_i|(v_t + v_i)
d = |v_t - v_i|(v_t + v_i)/(2a_m)                               (9)
```

![FinalPositionAsAFunctionOfTime.png](FinalPositionAsAFunctionOfTime.png)

Final position as a function of time for initial and target velocities of 0.25 and when `a_m = 1`.
 Blue is the threshold displacement, red is for a profile with a positive sign, and black is for a profile with a negative sign.

Notice how in the above figure, the displacement as a function of time for a profile with a negative sign first increases, then decreases, eventually crossing the threshold displacement.
 This means there are up to three unique profiles that can connect the initial and target states while respecting the constraints.
 Determining which one to follow is straight forward; in this case, it is the profile with the positive sign as it would take the least amount of time.
 This can be understood intuitively by the fact that, in this case, increasing the average velocity of the profile would get to a positive displacement faster than a lower average velocity.
 Likewise, the negative sign would be faster for negative displacements if the initial and target velocity were both -0.25.
 If the initial and target velocities are not of the same nonzero sign, then both displacement as a function of time curves will diverge from the threshold distance line.
 This leads to a simple way to determine the optimal sign of the profile: if the target displacement is greater than `d`, the sign is positive, and if it is less than `d`, the sign is negative.

This naturally leads to the question: what is the sign if the target displacement is equal to `d`?
 Obviously we would like the profile to take the form of the minimum profile, but it does make the implementation cumbersome to create an entirely separate code path for that case.
 To convince you that correctly determining the sign is important, we will calculate the peak velocity of a profile where `Δx = d`.
 Let `Δx = d = 0.375`, `v_i = 1`, `v_t = 2`, `a_m = 4`.
 First with `s = 1`:
```
v_p = (1)√((4)(0.375) + ((2)² + (1)²)/2)
v_p = √(1.5 + 2.5)
v_p = √(4)
v_p = 2
```
Next with `s = -1`:
```
v_p = (-1)√((-4)(0.375) + ((2)² + (1)²)/2)
v_p = -√(-1.5 + 2.5)
v_p = -√(1)
v_p = -1
```
While both of these would result in valid profiles, the second one would be considerably slower.
 Additionally, like with the architecture used in WPILib, calculating a profile with the same target state using the previous setpoint as the initial state could lead to an inconsistent profile sign being determined, and erratic behavior as a result.
 Recall that when the initial and target velocities have the same nonzero sign, the optimal sign matches the signs of the initial and target velocities.
 The same is true here, and because they are the same, we only need to check the sign of either the initial velocity or the target velocity.
 In all other cases, the calculations are identical regardless of sign when the target displacement equals the threshold displacement.

### Calculating characteristics of the profile

For the case where v_p exceeds the the velocity limit, letting `v_l = sv_m` means the values of `x_1` and `x_3` can be found by substituting `v_p = v_l` into (6) and (7).
```
x_1 = (v_l² - v_i²)/(2a)   (10)
x_3 = (v_l² - v_t²)/(2a)   (11)
```
which can be used to find x_2 by rearranging (4) to get
```
x_1 + x_2 + x_3 = Δx
x_2 = Δx - x_1 - x_3   (12)
```

Equations for the times of the segments are shown below, with `v_l = v_p` in the cases where v_p does not exceed the velocity limit.
```
t_1 = (v_l - v_i)/a   (13)
t_2 = x_2/v_l         (14)
t_3 = (v_l - v_t)/a   (15)
```
