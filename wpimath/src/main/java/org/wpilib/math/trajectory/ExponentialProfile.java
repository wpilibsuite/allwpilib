// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.trajectory;

import java.util.List;
import java.util.Objects;
import org.wpilib.math.trajectory.struct.ExponentialProfileStateStruct;
import org.wpilib.util.struct.StructSerializable;

/**
 * A trajectory that follows an exponential curve-shaped velocity profile.
 *
 * <p>While this class can be used for a profiled movement from start to finish, the intended usage
 * is to filter a reference's dynamics based on state-space model dynamics. To compute the reference
 * obeying this constraint, do the following.
 *
 * <p>Initialization:
 *
 * <pre><code>
 * ExponentialProfile.Constraints constraints =
 *   ExponentialProfile.Constraints.fromCharacteristics(kMaxV, kV, kA);
 * ExponentialProfile.State previousProfiledReference =
 *   new ExponentialProfile.State(initialReference, 0.0);
 * </code></pre>
 *
 * <p>Run on update:
 *
 * <pre><code>
 * var sample =
 *   ExponentialProfile.generate(constraints, previousProfiledReference, unprofiledReference)
 *     .sampleAt(timeSincePreviousUpdate);
 * previousProfiledReference = new ExponentialProfile.State(sample.position, sample.velocity);
 * </code></pre>
 *
 * <p>where `unprofiledReference` is free to change between calls. Note that when the unprofiled
 * reference is within the constraints, sampling returns the unprofiled reference unchanged.
 *
 * <p>Otherwise, a timer can be started to provide monotonic values for `sampleAt()` and to
 * determine when the profile has completed by comparing against `duration()`.
 */
public final class ExponentialProfile extends Trajectory<ExponentialProfileSample> {
  private final Constraints m_constraints;

  /** Profile constraints. */
  public static final class Constraints {
    /** Maximum unsigned input voltage. */
    public final double maxInput;

    /** The State-Space 1x1 system matrix. */
    public final double A;

    /** The State-Space 1x1 input matrix. */
    public final double B;

    /**
     * Constructs constraints for an ExponentialProfile.
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
     * @return The steady-state velocity achieved by this profile.
     */
    public double maxVelocity() {
      return -maxInput * B / A;
    }

    /**
     * Constructs constraints for an ExponentialProfile from characteristics.
     *
     * @param maxInput maximum unsigned input voltage
     * @param kV The velocity gain.
     * @param kA The acceleration gain.
     * @return The Constraints object.
     */
    public static Constraints fromCharacteristics(double maxInput, double kV, double kA) {
      return new Constraints(maxInput, -kV / kA, 1.0 / kA);
    }

    /**
     * Constructs constraints for an ExponentialProfile from State-Space parameters.
     *
     * @param maxInput maximum unsigned input voltage
     * @param A The State-Space 1x1 system matrix.
     * @param B The State-Space 1x1 input matrix.
     * @return The Constraints object.
     */
    public static Constraints fromStateSpace(double maxInput, double A, double B) {
      return new Constraints(maxInput, A, B);
    }
  }

  /** Profile state. */
  public static class State implements StructSerializable {
    /** The struct that serializes this class. */
    public static final ExponentialProfileStateStruct struct = new ExponentialProfileStateStruct();

    /** The position at this state. */
    public double position;

    /** The velocity at this state. */
    public double velocity;

    /** Default constructor. */
    public State() {}

    /**
     * Constructs a state within an exponential profile.
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
      return other instanceof State rhs
          && this.position == rhs.position
          && this.velocity == rhs.velocity;
    }

    @Override
    public int hashCode() {
      return Objects.hash(position, velocity);
    }
  }

  /**
   * ExponentialProfile timings.
   *
   * @param inflectionTime The time at which the profile reaches the inflection point.
   * @param totalTime The duration of the profile.
   */
  private record Timing(double inflectionTime, double totalTime) {}

  /**
   * Constructs an ExponentialProfile from its phase transition samples.
   *
   * @param constraints The constraints on the profile.
   * @param samples The samples of the trajectory.
   * @throws IllegalArgumentException if the list of samples is empty.
   */
  private ExponentialProfile(Constraints constraints, List<ExponentialProfileSample> samples) {
    super(samples);
    m_constraints = constraints;
  }

  /**
   * Interpolates between two samples along the profile.
   *
   * <p>The profile's samples are its phase transitions, so the input is constant between
   * consecutive samples. The start sample's state is integrated forward using its input, so the
   * result lies exactly on the profile.
   *
   * @param start The starting sample.
   * @param end The ending sample.
   * @param t The interpolation parameter between 0 and 1.
   * @return The interpolated sample.
   */
  @Override
  public ExponentialProfileSample interpolate(
      ExponentialProfileSample start, ExponentialProfileSample end, double t) {
    if (t <= 0.0) {
      return start;
    } else if (t >= 1.0) {
      return end;
    }

    double dt = t * (end.time - start.time);
    double u = start.input;

    var initial = new State(start.position, start.velocity);
    double position = computeDistanceFromTime(m_constraints, dt, u, initial);
    double velocity = computeVelocityFromTime(m_constraints, dt, u, initial);

    return new ExponentialProfileSample(
        start.time + dt, position, velocity, accelerationAt(m_constraints, velocity, u), u);
  }

  /**
   * Generates a profile from the current state to the goal state.
   *
   * <p>The trajectory starts at the current state at t = 0 and ends at the goal state once the
   * profile completes. Sampling past the end of the trajectory returns the goal state.
   *
   * @param constraints The constraints on the profile, like maximum input.
   * @param current The current state.
   * @param goal The desired state when the profile is complete.
   * @return The trajectory from the current state to the goal state.
   */
  public static ExponentialProfile generate(Constraints constraints, State current, State goal) {
    double direction = shouldFlipInput(constraints, current, goal) ? -1.0 : 1.0;
    double u = direction * constraints.maxInput;

    State inflectionPoint = calculateInflectionPoint(constraints, current, goal, u);
    Timing timing = calculateTiming(constraints, current, inflectionPoint, goal, u);

    // Floating point error can make segment durations slightly negative, which would place samples
    // before the start of the trajectory or out of order, so they're clamped.
    double inflectionTime = Math.max(timing.inflectionTime(), 0.0);
    double totalTime = Math.max(timing.totalTime(), inflectionTime);

    // The input that holds the goal velocity at steady state (0 = Av + Bu)
    double goalInput = -constraints.A * goal.velocity / constraints.B;

    // The samples are the profile's phase transitions: the start, the inflection point, and the end
    // of the profile. Each sample's input is the input of the segment that starts at it. The
    // sampled trajectory starts at the current state, regardless of validity.
    //
    // The inflection point is computed backward from the goal, so integrating forward from it
    // along the last segment reaches the goal.
    var samples =
        List.of(
            new ExponentialProfileSample(
                0.0,
                current.position,
                current.velocity,
                accelerationAt(constraints, current.velocity, u),
                u),
            new ExponentialProfileSample(
                inflectionTime,
                inflectionPoint.position,
                inflectionPoint.velocity,
                accelerationAt(constraints, inflectionPoint.velocity, -u),
                -u),
            new ExponentialProfileSample(totalTime, goal.position, goal.velocity, 0.0, goalInput));

    return new ExponentialProfile(constraints, samples);
  }

  /**
   * Returns the acceleration of the system at the given velocity and input.
   *
   * @param constraints The constraints on the profile.
   * @param velocity The velocity.
   * @param input The input.
   * @return The acceleration.
   */
  private static double accelerationAt(Constraints constraints, double velocity, double input) {
    // dv/dt = Av + Bu
    return constraints.A * velocity + constraints.B * input;
  }

  /**
   * Calculates the point after which the fastest way to reach the goal state is to apply input in
   * the opposite direction.
   *
   * @param constraints The constraints on the profile.
   * @param current The current state.
   * @param goal The desired state when the profile is complete.
   * @param input The signed input applied to this profile from the current state.
   * @return The position and velocity of the profile at the inflection point.
   */
  private static State calculateInflectionPoint(
      Constraints constraints, State current, State goal, double input) {
    var u = input;

    if (current.equals(goal)) {
      return current;
    }

    var inflectionVelocity = solveForInflectionVelocity(constraints, u, current, goal);
    var inflectionPosition = computeDistanceFromVelocity(constraints, inflectionVelocity, -u, goal);

    return new State(inflectionPosition, inflectionVelocity);
  }

  /**
   * Calculates the time it will take for this profile to reach the inflection point, and the time
   * it will take for this profile to reach the goal state.
   *
   * @param constraints The constraints on the profile.
   * @param current The current state.
   * @param inflectionPoint The inflection point of this profile.
   * @param goal The desired state when the profile is complete.
   * @param input The signed input applied to this profile from the current state.
   * @return The timing information for this profile.
   */
  private static Timing calculateTiming(
      Constraints constraints, State current, State inflectionPoint, State goal, double input) {
    var u = input;

    double inflectionT_forward;

    // We need to handle 5 cases here:
    //
    // - Approaching -maxVelocity from below
    // - Approaching -maxVelocity from above
    // - Approaching maxVelocity from below
    // - Approaching maxVelocity from above
    // - At +-maxVelocity
    //
    // For cases 1 and 3, we want to subtract epsilon from the inflection point velocity.
    // For cases 2 and 4, we want to add epsilon to the inflection point velocity.
    // For case 5, we have reached inflection point velocity.
    double epsilon = 1e-9;
    if (Math.abs(Math.signum(input) * constraints.maxVelocity() - inflectionPoint.velocity)
        < epsilon) {
      double solvableV = inflectionPoint.velocity;
      double t_to_solvable_v;
      double x_at_solvable_v;
      if (Math.abs(current.velocity - inflectionPoint.velocity) < epsilon) {
        t_to_solvable_v = 0;
        x_at_solvable_v = current.position;
      } else {
        if (Math.abs(current.velocity) > constraints.maxVelocity()) {
          solvableV += Math.signum(u) * epsilon;
        } else {
          solvableV -= Math.signum(u) * epsilon;
        }

        t_to_solvable_v = computeTimeFromVelocity(constraints, solvableV, u, current.velocity);
        x_at_solvable_v = computeDistanceFromVelocity(constraints, solvableV, u, current);
      }

      inflectionT_forward =
          t_to_solvable_v
              + Math.signum(input)
                  * (inflectionPoint.position - x_at_solvable_v)
                  / constraints.maxVelocity();
    } else {
      inflectionT_forward =
          computeTimeFromVelocity(constraints, inflectionPoint.velocity, u, current.velocity);
    }

    var inflectionT_backward =
        computeTimeFromVelocity(constraints, inflectionPoint.velocity, -u, goal.velocity);

    return new Timing(inflectionT_forward, inflectionT_forward - inflectionT_backward);
  }

  /**
   * Calculates the position reached after t seconds when applying an input from the initial state.
   *
   * @param constraints The constraints on the profile.
   * @param t The time since the initial state.
   * @param input The signed input applied to this profile from the initial state.
   * @param initial The initial state.
   * @return The distance travelled by this profile.
   */
  private static double computeDistanceFromTime(
      Constraints constraints, double t, double input, State initial) {
    var A = constraints.A;
    var B = constraints.B;
    var u = input;

    return initial.position
        + (-B * u * t + (initial.velocity + B * u / A) * (Math.exp(A * t) - 1)) / A;
  }

  /**
   * Calculates the velocity reached after t seconds when applying an input from the initial state.
   *
   * @param constraints The constraints on the profile.
   * @param t The time since the initial state.
   * @param input The signed input applied to this profile from the initial state.
   * @param initial The initial state.
   * @return The distance travelled by this profile.
   */
  private static double computeVelocityFromTime(
      Constraints constraints, double t, double input, State initial) {
    var A = constraints.A;
    var B = constraints.B;
    var u = input;

    return (initial.velocity + B * u / A) * Math.exp(A * t) - B * u / A;
  }

  /**
   * Calculates the time required to reach a specified velocity given the initial velocity.
   *
   * @param constraints The constraints on the profile.
   * @param velocity The goal velocity.
   * @param input The signed input applied to this profile from the initial state.
   * @param initial The initial velocity.
   * @return The time required to reach the goal velocity.
   */
  private static double computeTimeFromVelocity(
      Constraints constraints, double velocity, double input, double initial) {
    var A = constraints.A;
    var B = constraints.B;
    var u = input;

    return Math.log((A * velocity + B * u) / (A * initial + B * u)) / A;
  }

  /**
   * Calculates the distance reached at the same time as the given velocity when applying the given
   * input from the initial state.
   *
   * @param constraints The constraints on the profile.
   * @param velocity The velocity reached by this profile
   * @param input The signed input applied to this profile from the initial state.
   * @param initial The initial state.
   * @return The distance reached when the given velocity is reached.
   */
  private static double computeDistanceFromVelocity(
      Constraints constraints, double velocity, double input, State initial) {
    var A = constraints.A;
    var B = constraints.B;
    var u = input;

    return initial.position
        + (velocity - initial.velocity) / A
        - B * u / (A * A) * Math.log((A * velocity + B * u) / (A * initial.velocity + B * u));
  }

  /**
   * Calculates the velocity at which input should be reversed in order to reach the goal state from
   * the current state.
   *
   * @param constraints The constraints on the profile.
   * @param input The signed input applied to this profile from the current state.
   * @param current The current state.
   * @param goal The goal state.
   * @return The inflection velocity.
   */
  private static double solveForInflectionVelocity(
      Constraints constraints, double input, State current, State goal) {
    var A = constraints.A;
    var B = constraints.B;
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
   * Returns true if the profile should be inverted.
   *
   * <p>The profile is inverted if we should first apply negative input in order to reach the goal
   * state.
   *
   * @param constraints The constraints on the profile.
   * @param current The initial state (usually the current state).
   * @param goal The desired state when the profile is complete.
   */
  @SuppressWarnings("UnnecessaryParentheses")
  private static boolean shouldFlipInput(Constraints constraints, State current, State goal) {
    var u = constraints.maxInput;

    var xf = goal.position;
    var v0 = current.velocity;
    var vf = goal.velocity;

    var x_forward = computeDistanceFromVelocity(constraints, vf, u, current);
    var x_reverse = computeDistanceFromVelocity(constraints, vf, -u, current);

    if (v0 >= constraints.maxVelocity()) {
      return xf < x_reverse;
    }

    if (v0 <= -constraints.maxVelocity()) {
      return xf < x_forward;
    }

    var a = v0 >= 0;
    var b = vf >= 0;
    var c = xf >= x_forward;
    var d = xf >= x_reverse;

    return (a && !d) || (b && !c) || (!c && !d);
  }
}
