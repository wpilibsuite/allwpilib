// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.trajectory;

import java.util.Objects;

/**
 * A exponential curve-shaped velocity profile.
 *
 * <p>While this class can be used for a profiled movement from start to finish, the intended usage
 * is to filter a reference's dynamics based on state-space model dynamics. To compute the reference
 * obeying this constraint, do the following.
 *
 * <p>Initialization:
 *
 * <pre><code>
 * ExponentialProfile.Constraints constraints =
 *   new ExponentialProfile.Constraints(kMaxV, kMaxA);
 * ExponentialProfile.State previousProfiledReference =
 *   new ExponentialProfile.State(initialReference, 0.0);
 * ExponentialProfile profile = new ExponentialProfile(constraints);
 * </code></pre>
 *
 * <p>Run on update:
 *
 * <pre><code>
 * previousProfiledReference =
 * profile.calculate(timeSincePreviousUpdate, previousProfiledReference, unprofiledReference);
 * </code></pre>
 *
 * <p>where `unprofiledReference` is free to change between calls. Note that when the unprofiled
 * reference is within the constraints, `calculate()` returns the unprofiled reference unchanged.
 *
 * <p>Otherwise, a timer can be started to provide monotonic values for `calculate()` and to
 * determine when the profile has completed via `isFinished()`.
 */
public class ExponentialProfile {
  private final Constraints m_constraints;

  public static class ProfileTiming {
    public final double inflectionTime;
    public final double totalTime;

    protected ProfileTiming(double inflectionTime, double totalTime) {
      this.inflectionTime = inflectionTime;
      this.totalTime = totalTime;
    }

    public boolean isFinished(double t) {
      return t > inflectionTime;
    }
  }

  public static class Constraints {
    public final double maxInput;

    public final double A;
    public final double B;

    /**
     * Construct constraints for an ExponentialProfile.
     *
     * @param maxInput maximum unsigned input voltage
     * @param A The State-Space 1x1 system matrix.
     * @param B The State-Space 1x1 input matrix.
     */
    private Constraints(double maxInput, double A, double B) {
      this.maxInput = maxInput;
      this.A = A;
      this.B = B;
    }

    /**
     * Computes the max achievable velocity for an Exponential Profile.
     *
     * @return The seady-state velocity achieved by this profile.
     */
    public double maxVelocity() {
      return -maxInput * B / A;
    }

    /**
     * Construct constraints for an ExponentialProfile from characteristics.
     *
     * @param maxInput maximum unsigned input voltage
     * @param kV The velocity gain.
     * @param kA The acceleration gain.
     */
    public static Constraints fromCharacteristics(double maxInput, double kV, double kA) {
      return new Constraints(maxInput, -kV / kA, 1.0 / kA);
    }

    /**
     * Construct constraints for an ExponentialProfile from State-Space parameters.
     *
     * @param maxInput maximum unsigned input voltage
     * @param A The State-Space 1x1 system matrix.
     * @param B The State-Space 1x1 input matrix.
     */
    public static Constraints fromStateSpace(double maxInput, double A, double B) {
      return new Constraints(maxInput, A, B);
    }
  }

  public static class State {
    public final double position;

    public final double velocity;

    /**
     * Construct a state within an exponential profile
     *
     * @param position The position at this state.
     * @param velocity The velocity at this state.
     */
    public State(double position, double velocity) {
      this.position = position;
      this.velocity = velocity;
    }

    @Override
    public boolean equals(Object other) {
      if (other instanceof State) {
        State rhs = (State) other;
        return this.position == rhs.position && this.velocity == rhs.velocity;
      } else {
        return false;
      }
    }

    @Override
    public int hashCode() {
      return Objects.hash(position, velocity);
    }
  }

  /**
   * Construct an ExponentialProfile.
   *
   * @param constraints The constraints on the profile, like maximum input.
   */
  public ExponentialProfile(Constraints constraints) {
    m_constraints = constraints;
  }

  /**
   * Calculate the correct input for the profile at a time t where the current state is at time t =
   * 0.
   *
   * @param t The time since the beginning of the profile.
   * @param current The current state.
   * @param goal The desired state when the profile is complete.
   * @return The input applied at time t.
   */
  public double calculateInput(double t, State current, State goal) {
    var direction = shouldFlipInput(current, goal) ? -1 : 1;
    var u = direction * m_constraints.maxInput;

    var inflectionPoint = calculateInflectionPoint(current, goal, u);
    var timing = calculateProfileTiming(current, inflectionPoint, goal, u);

    if (t < 0) {
      return 0;
    } else if (t < timing.inflectionTime) {
      return direction * m_constraints.maxInput;
    } else if (t < timing.totalTime) {
      return -direction * m_constraints.maxInput;
    } else {
      return 0;
    }
  }

  /**
   * Calculate the correct position and velocity for the profile at a time t where the current state
   * is at time t = 0.
   *
   * @param t The time since the beginning of the profile.
   * @param current The current state.
   * @param goal The desired state when the profile is complete.
   * @return The position and velocity of the profile at time t.
   */
  public State calculate(double t, State current, State goal) {
    var direction = shouldFlipInput(current, goal) ? -1 : 1;
    var u = direction * m_constraints.maxInput;

    var inflectionPoint = calculateInflectionPoint(current, goal, u);
    var timing = calculateProfileTiming(current, inflectionPoint, goal, u);

    if (t < 0) {
      return current;
    } else if (t < timing.inflectionTime) {
      return new State(
          computeDistanceFromTime(t, u, current), computeVelocityFromTime(t, u, current));
    } else if (t < timing.totalTime) {
      return new State(
          computeDistanceFromTime(t - timing.totalTime, -u, goal),
          computeVelocityFromTime(t - timing.totalTime, -u, goal));
    } else {
      return goal;
    }
  }

  /**
   * Calculate the point after which the fastest way to reach the goal state is to apply input in
   * the opposite direction.
   *
   * @param current The current state.
   * @param goal The desired state when the profile is complete.
   * @return The position and velocity of the profile at the inflection point.
   */
  public State calculateInflectionPoint(State current, State goal) {
    var direction = shouldFlipInput(current, goal) ? -1 : 1;
    var u = direction * m_constraints.maxInput;

    return calculateInflectionPoint(current, goal, u);
  }

  /**
   * Calculate the point after which the fastest way to reach the goal state is to apply input in
   * the opposite direction.
   *
   * @param current The current state.
   * @param goal The desired state when the profile is complete.
   * @param input The signed input applied to this profile from the current state.
   * @return The position and velocity of the profile at the inflection point.
   */
  private State calculateInflectionPoint(State current, State goal, double input) {
    var u = input;

    if (current.equals(goal)) {
      return current;
    }

    var inflectionVelocity = solveForInflectionVelocity(u, current, goal);
    var inflectionPosition = computeDistanceFromVelocity(inflectionVelocity, -u, goal);

    return new State(inflectionPosition, inflectionVelocity);
  }

  /**
   * Calculate the time it will take for this profile to reach the goal state.
   *
   * @param current The current state.
   * @param goal The desired state when the profile is complete.
   * @return The total duration of this profile.
   */
  public double timeLeftUntil(State current, State goal) {
    var timing = calculateProfileTiming(current, goal);

    return timing.totalTime;
  }

  /**
   * Calculate the time it will take for this profile to reach the inflection point, and the time it
   * will take for this profile to reach the goal state.
   *
   * @param current The current state.
   * @param goal The desired state when the profile is complete.
   * @return The timing information for this profile.
   */
  public ProfileTiming calculateProfileTiming(State current, State goal) {
    var direction = shouldFlipInput(current, goal) ? -1 : 1;
    var u = direction * m_constraints.maxInput;

    var inflectionPoint = calculateInflectionPoint(current, goal, u);
    return calculateProfileTiming(current, inflectionPoint, goal, u);
  }

  /**
   * Calculate the time it will take for this profile to reach the inflection point, and the time it
   * will take for this profile to reach the goal state.
   *
   * @param current The current state.
   * @param inflectionPoint The inflection point of this profile.
   * @param goal The desired state when the profile is complete.
   * @param input The signed input applied to this profile from the current state.
   * @return The timing information for this profile.
   */
  private ProfileTiming calculateProfileTiming(
      State current, State inflectionPoint, State goal, double input) {
    var u = input;

    double inflectionT_forward;

    // We need to handle 4 cases here:
    // - Approaching -maxVelocity from below
    // - Approaching -maxVelocity from above
    // - Approaching maxVelocity from below
    // - Approaching maxVelocity from above
    // For cases 1 and 3, we want to subtract epsilon from the inflection point velocity
    // For cases 2 and 4, we want to add epsilon to the inflection point velocity
    double epsilon = 1e-9;
    if (Math.abs(Math.signum(input) * m_constraints.maxVelocity() - inflectionPoint.velocity)
        < epsilon) {
      double solvableV = inflectionPoint.velocity;
      if (Math.abs(current.velocity) > m_constraints.maxVelocity()) {
        solvableV += Math.signum(u) * epsilon;
      } else {
        solvableV -= Math.signum(u) * epsilon;
      }

      var t_to_solvable_v = computeTimeFromVelocity(solvableV, u, current.velocity);
      var x_at_solvable_v = computeDistanceFromVelocity(solvableV, u, current);

      inflectionT_forward =
          t_to_solvable_v
              + Math.signum(input)
                  * (inflectionPoint.position - x_at_solvable_v)
                  / m_constraints.maxVelocity();
    } else {
      inflectionT_forward = computeTimeFromVelocity(inflectionPoint.velocity, u, current.velocity);
    }

    var inflectionT_backward = computeTimeFromVelocity(inflectionPoint.velocity, -u, goal.velocity);

    return new ProfileTiming(inflectionT_forward, inflectionT_forward - inflectionT_backward);
  }

  /**
   * Calculate the position reached after t seconds when applying an input from the initial state.
   *
   * @param t The time since the initial state.
   * @param input The signed input applied to this profile from the initial state.
   * @param initial The initial state.
   * @return The distance travelled by this profile.
   */
  private double computeDistanceFromTime(double t, double input, State initial) {
    var A = m_constraints.A;
    var B = m_constraints.B;
    var u = input;

    return initial.position
        + (-B * u * t + (initial.velocity + B * u / A) * (Math.exp(A * t) - 1)) / A;
  }

  /**
   * Calculate the velocity reached after t seconds when applying an input from the initial state.
   *
   * @param t The time since the initial state.
   * @param input The signed input applied to this profile from the initial state.
   * @param initial The initial state.
   * @return The distance travelled by this profile.
   */
  private double computeVelocityFromTime(double t, double input, State initial) {
    var A = m_constraints.A;
    var B = m_constraints.B;
    var u = input;

    return (initial.velocity + B * u / A) * Math.exp(A * t) - B * u / A;
  }

  /**
   * Calculate the time require to reach a specified velocity the initial velocity.
   *
   * @param velocity The goal velocity.
   * @param input The signed input applied to this profile from the initial state.
   * @param initial The initial velocity.
   * @return The time required to reach the goal velocity.
   */
  private double computeTimeFromVelocity(double velocity, double input, double initial) {
    var A = m_constraints.A;
    var B = m_constraints.B;
    var u = input;

    return Math.log((A * velocity + B * u) / (A * initial + B * u)) / A;
  }

  /**
   * Calculate the distance reached at the same time as the given velocity when applying the given
   * input from the initial state.
   *
   * @param velocity The velocity reached by this profile
   * @param input The signed input applied to this profile from the initial state.
   * @param initial The initial state.
   * @return The distance reached when the given velocity is reached.
   */
  private double computeDistanceFromVelocity(double velocity, double input, State initial) {
    var A = m_constraints.A;
    var B = m_constraints.B;
    var u = input;

    return initial.position
        + (velocity - initial.velocity) / A
        - B * u / (A * A) * Math.log((A * velocity + B * u) / (A * initial.velocity + B * u));
  }

  /**
   * Calculate the velocity at which input should be reversed in order to reach the goal state from
   * the current state.
   *
   * @param input The signed input applied to this profile from the current state.
   * @param current The current state.
   * @param goal The goal state.
   * @return The inflection velocity.
   */
  private double solveForInflectionVelocity(double input, State current, State goal) {
    var A = m_constraints.A;
    var B = m_constraints.B;
    var u = input;

    var U_dir = Math.signum(u);

    var position_delta = goal.position - current.position;
    var velocity_delta = goal.velocity - current.velocity;

    var scalar = (A * current.velocity + B * u) * (A * goal.velocity - B * u);
    var power = -A / B / u * (A * position_delta - velocity_delta);

    var a = -A * A;
    var c = (B * B) * (u * u) + scalar * Math.exp(power);

    if (-1e-9 < c && c < 0) {
      // Numerical stability issue - the heuristic gets it right but c is around -1e-13
      return 0;
    }

    return U_dir * Math.sqrt(-c / a);
  }

  /**
   * Returns true if the profile inverted.
   *
   * <p>The profile is inverted if we should first apply negative input in order to reach the goal
   * state.
   *
   * @param current The initial state (usually the current state).
   * @param goal The desired state when the profile is complete.
   */
  private boolean shouldFlipInput(State current, State goal) {
    var A = m_constraints.A;
    var B = m_constraints.B;
    var u = m_constraints.maxInput;

    var x0 = current.position;
    var xf = goal.position;
    var v0 = current.velocity;
    var vf = goal.velocity;

    var x_forward = computeDistanceFromVelocity(vf, u, current);
    var x_reverse = computeDistanceFromVelocity(vf, -u, current);

    if (v0 >= m_constraints.maxVelocity()) {
      return xf < x_reverse;
    }

    if (v0 <= -m_constraints.maxVelocity()) {
      return xf < x_forward;
    }

    var a = v0 >= 0;
    var b = vf >= 0;
    var c = xf >= x_forward;
    var d = xf >= x_reverse;

    return (a && !d) || (b && !c) || (!c && !d);
  }
}
