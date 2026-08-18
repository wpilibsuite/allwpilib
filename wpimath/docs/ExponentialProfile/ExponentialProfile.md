# Exponential Profile

## Closed Form Solution for an Exponential Motion Profile

See [this script](ExponentialProfileModel.py) for the derivation of the continuous-time model.

### Heuristic for Input Direction in Exponential Profile

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
