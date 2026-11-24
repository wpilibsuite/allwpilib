// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.trajectory;

import java.util.List;
import java.util.Objects;
import org.wpilib.math.trajectory.struct.TrapezoidProfileConstraintsStruct;
import org.wpilib.math.trajectory.struct.TrapezoidProfileStateStruct;
import org.wpilib.util.UsageReporting;
import org.wpilib.util.struct.StructSerializable;

/**
 * A trajectory that follows a trapezoid-shaped velocity profile.
 *
 * <p>While this class can be used for a profiled movement from start to finish, the intended usage
 * is to filter a reference's dynamics based on trapezoidal velocity constraints. To compute the
 * reference obeying this constraint, do the following.
 *
 * <p>Initialization:
 *
 * <pre><code>
 * TrapezoidProfile.Constraints constraints =
 *   new TrapezoidProfile.Constraints(kMaxV, kMaxA);
 * TrapezoidProfile.State previousProfiledReference =
 *   new TrapezoidProfile.State(initialReference, 0.0);
 * </code></pre>
 *
 * <p>Run on update:
 *
 * <pre><code>
 * var sample =
 *   TrapezoidProfile.generate(constraints, previousProfiledReference, unprofiledReference)
 *     .sampleAt(timeSincePreviousUpdate);
 * previousProfiledReference = new TrapezoidProfile.State(sample.position, sample.velocity);
 * </code></pre>
 *
 * <p>where `unprofiledReference` is free to change between calls. Note that when the unprofiled
 * reference is within the constraints, sampling returns the unprofiled reference unchanged.
 *
 * <p>Otherwise, a timer can be started to provide monotonic values for `sampleAt()` and to
 * determine when the profile has completed by comparing against `duration()`.
 */
public final class TrapezoidProfile extends Trajectory<TrapezoidProfileSample> {
  /** Profile constraints. */
  public static class Constraints implements StructSerializable {
    /** The struct used to serialize this class. */
    public static final TrapezoidProfileConstraintsStruct struct =
        new TrapezoidProfileConstraintsStruct();

    /** Maximum velocity. */
    public final double maxVelocity;

    /** Maximum acceleration. */
    public final double maxAcceleration;

    /**
     * Constructs constraints for a TrapezoidProfile.
     *
     * @param maxVelocity Maximum velocity, must be positive.
     * @param maxAcceleration Maximum acceleration, must be positive.
     */
    public Constraints(double maxVelocity, double maxAcceleration) {
      if (maxVelocity <= 0.0 || maxAcceleration <= 0.0) {
        throw new IllegalArgumentException("Constraints must be positive");
      }

      this.maxVelocity = maxVelocity;
      this.maxAcceleration = maxAcceleration;
      UsageReporting.reportUsage("TrapezoidProfile", "");
    }
  }

  /** Profile state. */
  public static class State implements StructSerializable {
    /** The struct used to serialize this class. */
    public static final TrapezoidProfileStateStruct struct = new TrapezoidProfileStateStruct();

    /** The position at this state. */
    public double position;

    /** The velocity at this state. */
    public double velocity;

    /** Default constructor. */
    public State() {}

    /**
     * Constructs constraints for a Trapezoid Profile.
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
   * TrapezoidProfile timings.
   *
   * @param t_1 The time the profile spends in the first segment.
   * @param t_2 The time the profile spends at the velocity limit.
   * @param t_3 The time the profile spends in the last segment.
   */
  @SuppressWarnings("checkstyle:MemberName")
  private record Timing(double t_1, double t_2, double t_3) {
    /**
     * Returns the duration of the profile.
     *
     * @return The duration of the profile.
     */
    double duration() {
      return t_1 + t_2 + t_3;
    }
  }

  /**
   * Constructs a TrapezoidProfile from its phase transition samples.
   *
   * @param samples The samples of the trajectory.
   * @throws IllegalArgumentException if the list of samples is empty.
   */
  private TrapezoidProfile(List<TrapezoidProfileSample> samples) {
    super(samples);
  }

  /**
   * Interpolates between two samples along the profile.
   *
   * <p>The profile's samples are its phase transitions, so the acceleration is constant between
   * consecutive samples. The start sample's state is integrated forward using its acceleration, so
   * the result lies exactly on the profile.
   *
   * @param start The starting sample.
   * @param end The ending sample.
   * @param t The interpolation parameter between 0 and 1.
   * @return The interpolated sample.
   */
  @Override
  public TrapezoidProfileSample interpolate(
      TrapezoidProfileSample start, TrapezoidProfileSample end, double t) {
    if (t <= 0.0) {
      return start;
    } else if (t >= 1.0) {
      return end;
    }

    double dt = t * (end.time - start.time);

    // v = v_i + at   (1)
    double velocity = start.velocity + start.acceleration * dt;
    // x = x_i + v_i t + at² / 2   (2)
    double position = start.position + start.velocity * dt + start.acceleration / 2.0 * dt * dt;

    return new TrapezoidProfileSample(start.time + dt, position, velocity, start.acceleration);
  }

  /**
   * Generates a profile from the current state to the goal state.
   *
   * <p>The trajectory starts at the current state at t = 0 and ends at the goal state once the
   * profile completes. Sampling past the end of the trajectory returns the goal state.
   *
   * @param constraints The constraints on the profile, like maximum velocity.
   * @param current The current state.
   * @param goal The desired state when the profile is complete.
   * @return The trajectory from the current state to the goal state.
   */
  public static TrapezoidProfile generate(Constraints constraints, State current, State goal) {
    State state = new State(current.position, current.velocity);
    State target = new State(goal.position, goal.velocity);

    // Adjust states so that they are within the constraints and get the time required for the
    // current state to return to a valid state.
    double recoveryTime = adjustStates(constraints, state, target);
    double sign = getSign(constraints, state, target);
    Timing timing = generateTiming(sign, constraints, state, target);

    // In the case that the sign of the profile and the sign of the acceleration are identical, the
    // recovery can be treated as an extension of the first segment. In the case that they differ,
    // the recovered state will have a velocity of v_l and the above calculated t_1 will be zero. To
    // handle this, the first segment's acceleration is flipped to ensure proper recovery.
    //
    // Floating point error can make segment durations slightly negative, which would place samples
    // before the start of the trajectory, so they're clamped to zero.
    timing =
        new Timing(
            Math.max(timing.t_1() + recoveryTime, 0.0),
            Math.max(timing.t_2(), 0.0),
            Math.max(timing.t_3(), 0.0));

    double acceleration = sign * constraints.maxAcceleration;
    double firstLegAcceleration =
        recoveryTime > 0.0 && current.velocity * sign > 0.0 ? -acceleration : acceleration;
    double lastLegAcceleration = -acceleration;

    // The samples are the profile's phase transitions: the start, the end of the first segment, the
    // end of the constant velocity segment, and the end of the profile. Each sample's acceleration
    // is the acceleration of the segment that starts at it. The sampled trajectory starts at the
    // current state, regardless of validity.
    double[] segmentDurations = {timing.t_1(), timing.t_2(), timing.t_3()};
    double[] segmentAccelerations = {firstLegAcceleration, 0.0, lastLegAcceleration};

    var samples = new TrapezoidProfileSample[4];

    double time = 0.0;
    double position = current.position;
    double velocity = current.velocity;
    for (int i = 0; i < segmentDurations.length; ++i) {
      samples[i] =
          new TrapezoidProfileSample(
              time,
              position,
              velocity,
              accelerationAt(time, timing, firstLegAcceleration, lastLegAcceleration));

      double dt = segmentDurations[i];
      double a = segmentAccelerations[i];
      double v_i = velocity;
      // v = v_i + at   (1)
      velocity = v_i + a * dt;
      // x = x_i + v_i t + at² / 2   (2)
      position += v_i * dt + a / 2.0 * dt * dt;
      time += dt;
    }

    // Use the goal state directly for the last sample to avoid accumulating floating point error
    samples[3] =
        new TrapezoidProfileSample(timing.duration(), target.position, target.velocity, 0.0);

    return new TrapezoidProfile(List.of(samples));
  }

  /**
   * Returns the acceleration of a profile at the given time.
   *
   * @param time The time relative to the profile start.
   * @param timing The time spent in each segment of the profile.
   * @param firstLegAcceleration The acceleration during the first segment of the profile.
   * @param lastLegAcceleration The acceleration during the last segment of the profile.
   * @return The acceleration of the profile at the given time.
   */
  private static double accelerationAt(
      double time, Timing timing, double firstLegAcceleration, double lastLegAcceleration) {
    if (time < timing.t_1()) {
      return firstLegAcceleration;
    } else if (time < timing.t_1() + timing.t_2()) {
      return 0.0;
    } else if (time < timing.duration()) {
      return lastLegAcceleration;
    } else {
      return 0.0;
    }
  }

  /**
   * Adjusts the profile states to be within the constraints and returns the time needed to bring
   * the current state back within the constraints.
   *
   * <p>In order to smoothly return to a state within the constraints, the current state is modified
   * to be the result of accelerating towards a valid velocity at the maximum acceleration. This
   * method returns the time this transition takes. By contrast, the goal velocity is simply clamped
   * to the valid region.
   *
   * @param constraints The constraints on the profile, like maximum velocity.
   * @param current The current state to be adjusted.
   * @param goal The goal state state to be adjusted.
   * @return The time taken to make the current state valid.
   */
  private static double adjustStates(Constraints constraints, State current, State goal) {
    if (Math.abs(goal.velocity) > constraints.maxVelocity) {
      goal.velocity = Math.copySign(constraints.maxVelocity, goal.velocity);
    }

    double recoveryTime = 0.0;
    double violationAmount = Math.abs(current.velocity) - constraints.maxVelocity;

    if (violationAmount > 0.0) {
      recoveryTime = violationAmount / constraints.maxAcceleration;
      // x = x_i + v_i t + at² / 2   (2)
      current.position +=
          current.velocity * recoveryTime
              + Math.copySign(constraints.maxAcceleration, -current.velocity)
                  * recoveryTime
                  * recoveryTime
                  / 2.0;
      // The closest valid velocity will have the magnitude of the max velocity.
      current.velocity = Math.copySign(constraints.maxVelocity, current.velocity);
    }

    return recoveryTime;
  }

  /**
   * Returns the sign of the profile.
   *
   * <p>The current and goal states must be within the profile constraints for a valid sign.
   *
   * @param constraints The constraints on the profile, like maximum velocity.
   * @param current The initial state, adjusted not to violate the constraints.
   * @param goal The goal state of the profile.
   * @return 1.0 if the profile direction is positive, -1.0 if it is not.
   */
  private static double getSign(Constraints constraints, State current, State goal) {
    double dx = goal.position - current.position;

    // Calculate threshold displacement
    // d = |v_t - v_i|(v_t + v_i) / (2 a_m)   (9)
    double d =
        Math.abs(goal.velocity - current.velocity)
            * (goal.velocity + current.velocity)
            / (2.0 * constraints.maxAcceleration);

    // As discussed in TrapezoidProfile.md, the correct sign must be chosen when dx == d because
    // following a suboptimal profile may lead to "chattering". Additionally, if numerical precision
    // errors cause the calculated optimal sign to change throughout the profile, that may lead to
    // suboptimal states being calculated. To fix this, we add a tolerance such that if |dx - d| <
    // epsilon, we return the sign that would lead to the minimum profile being calculated. We do
    // not have control over the floating point precision error from previous calculations, and as
    // such, it is difficult to bound the possible error. 1e-12 should be good enough for FRC
    // though.
    if (Math.abs(dx - d) < 1e-12) {
      return Math.copySign(1.0, goal.velocity);
    } else {
      if (dx > d) {
        return 1.0;
      } else {
        return -1.0;
      }
    }
  }

  /**
   * Generates profile timings from valid current and goal states.
   *
   * <p>Returns the time for each section of the profile from current and goal states with valid
   * velocities.
   *
   * @param sign The sign of the profile to generate.
   * @param constraints The constraints on the profile, like maximum velocity.
   * @param current The valid current state.
   * @param goal The valid goal state.
   * @return The time for each section of the profile.
   */
  private static Timing generateTiming(
      double sign, Constraints constraints, State current, State goal) {
    double acceleration = sign * constraints.maxAcceleration;
    double velocityLimit = sign * constraints.maxVelocity;
    double dx = goal.position - current.position;

    // Calculate the peak velocity to compare to velocity constraint.
    // v_p = √(aΔx + (v_t² + v_i²) / 2)   (8)
    double peakVelocity =
        sign
            * Math.sqrt(
                Math.max(
                    acceleration * dx
                        + (goal.velocity * goal.velocity + current.velocity * current.velocity) / 2,
                    0));

    // Handle the case where we hit maximum velocity.
    if (sign * peakVelocity > constraints.maxVelocity) {
      // t_1 = (v_l - v_i) / a   (13)
      double t_1 = (velocityLimit - current.velocity) / acceleration;
      // t_3 = (v_l - v_t) / a   (15)
      double t_3 = (velocityLimit - goal.velocity) / acceleration;

      // x_1 = (v_p² - v_i²) / (2a)   (6)
      // Substitute v_p for v_l because this is the velocity constrained case.
      // x_1 = (v_l² - v_i²) / (2a)
      double x_1 =
          (velocityLimit * velocityLimit - current.velocity * current.velocity)
              / (2 * acceleration);

      // x_3 = (v_p² - v_t²) / (2a)   (7)
      // Substitute v_p for v_l because this is the velocity constrained case.
      // x_3 = (v_l² - v_t²) / (2a)
      double x_3 =
          (velocityLimit * velocityLimit - goal.velocity * goal.velocity) / (2 * acceleration);

      // x_2 = Δx - x_1 - x_3   (12)
      double x_2 = dx - x_1 - x_3;

      // t_2 = x_2 / v_l   (14)
      double t_2 = x_2 / velocityLimit;

      return new Timing(t_1, t_2, t_3);
    } else {
      // t_1 = (v_p - v_i) / a   (13)
      double t_1 = (peakVelocity - current.velocity) / acceleration;
      // t_3 = (v_p - v_t) / a   (15)
      double t_3 = (peakVelocity - goal.velocity) / acceleration;

      return new Timing(t_1, 0.0, t_3);
    }
  }
}
