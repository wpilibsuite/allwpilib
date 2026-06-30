// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.trajectory;

import java.util.Objects;
import org.wpilib.math.trajectory.struct.TrapezoidProfileStateStruct;
import org.wpilib.math.util.MathSharedStore;
import org.wpilib.util.struct.StructSerializable;

/**
 * A trapezoid-shaped velocity profile.
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
 * TrapezoidProfile profile = new TrapezoidProfile(constraints);
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
public class TrapezoidProfile {
  private final Constraints m_constraints;

  private ProfileTiming m_profile = new ProfileTiming();

  /** Profile constraints. */
  public static class Constraints {
    /** Maximum velocity. */
    public final double maxVelocity;

    /** Maximum acceleration. */
    public final double maxAcceleration;

    /**
     * Constructs constraints for a TrapezoidProfile.
     *
     * @param maxVelocity Maximum velocity, must be non-negative.
     * @param maxAcceleration Maximum acceleration, must be non-negative.
     */
    public Constraints(double maxVelocity, double maxAcceleration) {
      if (maxVelocity <= 0.0 || maxAcceleration <= 0.0) {
        throw new IllegalArgumentException("Constraints must be positive");
      }

      this.maxVelocity = maxVelocity;
      this.maxAcceleration = maxAcceleration;
      MathSharedStore.reportUsage("TrapezoidProfile", "");
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

  /** Profile Timing. */
  public static class ProfileTiming {
    /** The time the profile spends in the first leg. */
    public double t_1;

    /** The time the profile spends at the velocity limit. */
    public double t_2;

    /** The time the profile spends in the last leg. */
    public double t_3;

    /**
     * Constructs the timing object for a Trapezoid Profile.
     *
     * @param t_1 The time the profile spends on the first leg of the profile.
     * @param t_2 The time the profile spends at the velocity limit.
     * @param t_3 the time the profile spends on the last leg of the profile.
     */
    public ProfileTiming(double t_1, double t_2, double t_3) {
      if (t_1 < 0.0 || t_2 < 0.0 || t_3 < 0.0) {
        throw new IllegalArgumentException("Times must be non-negative");
      }
      this.t_1 = t_1;
      this.t_2 = t_2;
      this.t_3 = t_3;
    }

    /** Zero initializes the timing object for a Trapezoid Profile. */
    public ProfileTiming() {
      this.t_1 = 0.0;
      this.t_2 = 0.0;
      this.t_3 = 0.0;
    }

    @Override
    public boolean equals(Object other) {
      return other instanceof ProfileTiming rhs
          && this.t_1 == rhs.t_1
          && this.t_2 == rhs.t_2
          && this.t_3 == rhs.t_3;
    }

    @Override
    public int hashCode() {
      return Objects.hash(t_1, t_2, t_3);
    }
  }

  /**
   * Constructs a TrapezoidProfile.
   *
   * @param constraints The constraints on the profile, like maximum velocity.
   */
  public TrapezoidProfile(Constraints constraints) {
    m_constraints = constraints;
  }

  /**
   * Calculates the position and velocity for the profile at a time t where the current state is at
   * time t = 0.
   *
   * @param t How long to advance from the current state toward the desired state.
   * @param current The current state.
   * @param goal The desired state when the profile is complete.
   * @return The position and velocity of the profile at time t.
   */
  public State calculate(double t, State current, State goal) {
    State state = new State(current.position, current.velocity);
    State target = new State(goal.position, goal.velocity);

    double recoveryTime = adjustStates(state, target);
    double sign = getSign(state, target);
    m_profile = generateProfile(sign, state, target);

    // Set state back to the current one to ensure continuity.
    state = new State(current.position, current.velocity);

    // Make sure we add time to get to a valid state back onto the profile times.
    m_profile.t_1 += recoveryTime;

    double acceleration = sign * m_constraints.maxAcceleration;
    advanceState(
        Math.min(t, m_profile.t_1),
        recoveryTime > 0.0 && sign * state.velocity > 0.0 ? -acceleration : acceleration,
        state);

    if (t > m_profile.t_1) {
      t -= m_profile.t_1;
      advanceState(Math.min(t, m_profile.t_2), 0.0, state);

      if (t > m_profile.t_2) {
        t -= m_profile.t_2;
        advanceState(Math.min(t, m_profile.t_3), -acceleration, state);

        if (t > m_profile.t_3) {
          state = target;
        }
      }
    }

    return state;
  }

  /**
   * Returns the time to get between two states. This does not affect the internal variables, and as
   * a result, may be used for states not on the active trajectory.
   *
   * @param current The current state.
   * @param goal The goal state.
   * @return The time to transition between the two states while respecting profile constraints.
   */
  public double timeLeftUntil(State current, State goal) {
    State state = new State(current.position, current.velocity);
    State target = new State(goal.position, goal.velocity);

    double recoveryTime = adjustStates(state, target);
    double sign = getSign(state, target);
    ProfileTiming profile = generateProfile(sign, state, target);

    profile.t_1 += recoveryTime;

    return profile.t_1 + profile.t_2 + profile.t_3;
  }

  /**
   * Returns the total time the profile takes to reach the goal.
   *
   * @return The total time the profile takes to reach the goal, or zero if no goal was set.
   */
  public double totalTime() {
    return m_profile.t_1 + m_profile.t_2 + m_profile.t_3;
  }

  /**
   * Returns true if the profile has reached the goal.
   *
   * <p>The profile has reached the goal if the time since the profile started has exceeded the
   * profile's total time.
   *
   * @param t The time since the beginning of the profile.
   * @return True if the profile has reached the goal.
   */
  public boolean isFinished(double t) {
    return t >= totalTime();
  }

  /**
   * Adjusts the members of the state to reflect a constant acceleration over a specified time.
   *
   * @param time The time to advance the state by.
   * @param acceleration The acceleration to advance the state with.
   * @param state The state to advance.
   */
  private static void advanceState(double time, double acceleration, State state) {
    state.position += state.velocity * time + acceleration / 2.0 * time * time;
    state.velocity += acceleration * time;
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
   * @param current The current state to be adjusted.
   * @param goal The goal state state to be adjusted.
   * @return The time taken to make the current state valid.
   */
  private double adjustStates(State current, State goal) {
    if (Math.abs(goal.velocity) > m_constraints.maxVelocity) {
      goal.velocity = Math.copySign(m_constraints.maxVelocity, goal.velocity);
    }

    double recoveryTime = 0.0;
    double violationAmount = Math.abs(current.velocity) - m_constraints.maxVelocity;

    if (violationAmount > 0.0) {
      recoveryTime = violationAmount / m_constraints.maxAcceleration;
      // x = x_i + v_i t + at   (2)
      current.position +=
          current.velocity * recoveryTime
              + Math.copySign(m_constraints.maxAcceleration, -current.velocity)
                  * recoveryTime
                  * recoveryTime
                  / 2.0;
      // v = v_i + at   (1)
      current.velocity = Math.copySign(m_constraints.maxVelocity, current.velocity);
    }

    return recoveryTime;
  }

  /**
   * Returns the sign of the profile.
   *
   * <p>The current and goal states must be within the profile constraints for a valid sign.
   *
   * @param current The initial state, adjusted not to violate the constraints.
   * @param goal The goal state of the profile.
   * @return 1.0 if the profile direction is positive, -1.0 if it is not.
   */
  double getSign(State current, State goal) {
    double dx = goal.position - current.position;

    // Calculate threshold displacement
    // d = |v_t - v_i| * (v_t + v_i) / a_m   (9)
    double d =
        Math.abs(goal.velocity - current.velocity)
            / m_constraints.maxAcceleration
            * (current.velocity + goal.velocity)
            / 2.0;

    // As discussed in TrapezoidProfile.md the correct sign must be chosen when dx == d because
    // following a suboptimal profile may lead to "chattering". Additionally, if numerical precision
    // errors cause the calculated optimal sign to change throughout the profile, that may lead to
    // suboptimal states being calculated. To fix this we add a tolerance such that if |dx - d| <
    // epsilon we return the sign that would lead to the minimum profile being calculated. We do not
    // have control over the floating point precision error from previous calculations and as such
    // it is difficult to bound the possible error. 1e-12 should be good enough for FRC though.
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
   * @param current The valid current state.
   * @param goal The valid goal state.
   * @return The time for each section of the profile.
   */
  private ProfileTiming generateProfile(double sign, State current, State goal) {
    ProfileTiming profile = new ProfileTiming();

    double acceleration = sign * m_constraints.maxAcceleration;
    double velocityLimit = sign * m_constraints.maxVelocity;
    double dx = goal.position - current.position;

    // Calculate the peak velocity to compare to velocity constraint.
    // v_p = √(a * Δx + (v_t² + v_i²) / 2)   (9)
    double peakVelocity =
        sign
            * Math.sqrt(
                Math.max(
                    (goal.velocity * goal.velocity + current.velocity * current.velocity) / 2
                        + acceleration * dx,
                    0));

    // Handle the case where we hit maximum velocity.
    if (sign * peakVelocity > m_constraints.maxVelocity) {
      // t_1 = (v_l - v_i) / a   (13)
      profile.t_1 = (velocityLimit - current.velocity) / acceleration;
      // t_3 = (v_l - v_t) / a   (15)
      profile.t_3 = (velocityLimit - goal.velocity) / acceleration;

      // x_2 = Δx - x_1 - x_3   (12)
      // t_2 = x_2 / v_l   (14)
      profile.t_2 =
          (dx
                  - (2 * velocityLimit * velocityLimit
                          - (current.velocity * current.velocity + goal.velocity * goal.velocity))
                      / (2 * acceleration))
              / velocityLimit;
    } else {
      // t_1 = (v_p - v_i) / a   (13)
      profile.t_1 = (peakVelocity - current.velocity) / acceleration;
      // t_3 = (v_p - v_t) / a   (15)
      profile.t_3 = (peakVelocity - goal.velocity) / acceleration;
    }

    return profile;
  }
}
