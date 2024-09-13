from sympy import *
from sympy.logic.boolalg import *

init_printing()

U, A, B, t, x0, xf, v0, vf, c1, c2, v, V, kV, kA = symbols(
    "U, A, B t, x0, xf, v0, vf, C1, C2, v, V, kV, kA"
)

x = symbols("x", cls=Function)

# Exponential profiles are derived from a differential equation: ẍ - A * ẋ = B * U
diffeq = Eq(x(t).diff(t, t) - A * x(t).diff(t), B * U)

x = dsolve(diffeq).rhs
dx = x.diff(t)

x = x.subs(
    [
        (c1, solve(Eq(x.subs(t, 0), x0), c1)[0]),
        (c2, solve(Eq(dx.subs(t, 0), v0), c2)[0]),
    ]
)

print(f"General Solution: {x}")

# We need two specific solutions to this equation for an Exponential Profile:
# One that passes through (x0, v0) and has input U
# Another that passes through (xf, vf) and has input -U

# x1 is for the accelerate step
x1 = x.subs({x0: x0, v0: v0, U: U})

dx1 = x1.diff(t)
t1_eqn = solve(Eq(dx1, v), t)[0]
# x1 in phase space (input v, output x)
x1_ps = x1.subs(t, t1_eqn)


# x2 is for the decelerate step
x2 = x.subs({x0: xf, v0: vf, U: -U})

dx2 = x2.diff(t)
t2_eqn = solve(Eq(dx2, v), t)[0]
# x2 in phase space (input v, output x)
x2_ps = x2.subs(t, t2_eqn)

# The point at which we switch from input U to -U is the inflection point.
# In phase space, this is a point (x, v) where x1(v) = x2(v)
# For now, we will just solve for +U and assume inflection velocity is positive.
# The other possible solutions are -v_soln, and the solutions to v_equality.subs(U, -U)
equality = simplify(Eq(x1_ps, x2_ps).expand()).expand()
equality = Eq(equality.lhs - x0 + v0 / A - v / A, equality.rhs - x0 + v0 / A - v / A)
equality = Eq(
    equality.lhs
    - B * U * log(A * v / (A * vf - B * U) - B * U / (A * vf - B * U)) / A**2,
    equality.rhs
    - B * U * log(A * v / (A * vf - B * U) - B * U / (A * vf - B * U)) / A**2,
)
equality = Eq(equality.lhs / (-B * U / A / A), equality.rhs / (-B * U / A / A))
equality = Eq(exp(equality.lhs.simplify()), exp(equality.rhs.simplify()))
equality = Eq(
    equality.lhs * (A * v0 + B * U) * (A * vf - B * U),
    equality.rhs * (A * v0 + B * U) * (A * vf - B * U),
)
equality = Eq(-equality.lhs.expand() + equality.rhs, 0)

# This is a quadratic equation of the form ax^2 + c = 0
v_equality = equality

# solve, take positive result
v_soln = solve(v_equality, v)[0]

# With this information, we can calculate the inflection point (x, v)
# and calculate the times that x1 and x2 reach the inflection point
inflection_x = x1_ps.subs(v, v_soln)
inflection_t1 = t1_eqn.subs(v, v_soln)
inflection_t2 = t2_eqn.subs(v, v_soln)

# inflection_t2 < 0 because in order for the profile to get to
# the inflection point from the terminal state, it must go back in time.
totalTime = inflection_t1 - inflection_t2

print(f"x1: {expand(simplify(x1))}")
print(f"x2: {expand(simplify(x2))}")
print(f"dx1: {expand(simplify(dx1))}")
print(f"dx2: {expand(simplify(dx2))}")
print(f"t1: {expand(simplify(t1_eqn))}")
print(f"t2: {expand(simplify(t2_eqn))}")
print(f"x1 phase space: {expand(simplify(x1.subs(t, t1_eqn)))}")
print(f"x2 phase space: {expand(simplify(x2.subs(t, t2_eqn)))}")
print(f"vi equality: {v_equality}")


a, b, c, d = symbols("a, b, c, d")

expression = SOPform([a, b, c, d], minterms=[0, 4, 5, 8, 10, 12, 13, 14])
print(f"Truth Table Expression: {expression}")
