// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.trajectory;

import java.util.Objects;
import org.wpilib.math.trajectory.struct.TrapezoidProfileStateStruct;
import org.wpilib.math.util.MathSharedStore;

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

  public ProfileTiming m_profile = new ProfileTiming();

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
      if (maxVelocity < 0.0 || maxAcceleration < 0.0) {
        throw new IllegalArgumentException("Constraints must be non-negative");
      }

      this.maxVelocity = maxVelocity;
      this.maxAcceleration = maxAcceleration;
      MathSharedStore.reportUsage("TrapezoidProfile", "");
    }
  }

  /** Profile state. */
  public static class State {
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
    /** The struct used to serialize this class. */
    // public static final TrapezoidProfileTimingStruct struct = new TrapezoidProfileTimingStruct();

    /** The time the profile spends in the first leg. */
    public double accelTime;

    /** The time the profile spends at the velocity limit. */
    public double cruiseTime;

    /** The time the profile spends in the last leg. */
    public double decelTime;

    /**
     * Constructs the timing object for a Trapezoid Profile.
     *
     * @param accelTime The time the profile spends on the first leg of the profile.
     * @param cruiseTime The time the profile spends at the velocity limit.
     * @param decelTime the time the profile spends on the last leg of the profile.
     */
    public ProfileTiming(double accelTime, double cruiseTime, double decelTime) {
      if (accelTime < 0.0 || cruiseTime < 0.0 || decelTime < 0.0) {
        throw new IllegalArgumentException("Times must be non-negative");
      }
      this.accelTime = accelTime;
      this.cruiseTime = cruiseTime;
      this.decelTime = decelTime;
    }

    /** Zero initializes the timing object for a Trapezoid Profile. */
    public ProfileTiming() {
      this.accelTime = 0.0;
      this.cruiseTime = 0.0;
      this.decelTime = 0.0;
    }

    @Override
    public boolean equals(Object other) {
      return other instanceof ProfileTiming rhs
          && this.accelTime == rhs.accelTime
          && this.cruiseTime == rhs.cruiseTime
          && this.decelTime == rhs.decelTime;
    }

    @Override
    public int hashCode() {
      return Objects.hash(accelTime, cruiseTime, decelTime);
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
  @SuppressWarnings("PMD.AvoidDeeplyNestedIfStmts")
  public State calculate(double t, State current, State goal) {
    State state = new State(current.position, current.velocity);
    State target = new State(goal.position, goal.velocity);

    double recoveryTime = adjustStates(state, target);
    double sign = getSign(state, target);
    m_profile = generateProfile(sign, state, target);

    // Set state back to the current one to ensure continuity.
    state = new State(current.position, current.velocity);

    // Make sure we add time to get to a valid state back onto the profile times.
    m_profile.accelTime += recoveryTime;

    double acceleration = sign * m_constraints.maxAcceleration;
    advanceState(
        Math.min(t, m_profile.accelTime),
        recoveryTime > 0.0 && sign * state.velocity > 0.0 ? -acceleration : acceleration,
        state);

    if (t > m_profile.accelTime) {
      t -= m_profile.accelTime;
      advanceState(Math.min(t, m_profile.cruiseTime), 0.0, state);

      if (t > m_profile.cruiseTime) {
        t -= m_profile.cruiseTime;
        advanceState(Math.min(t, m_profile.decelTime), -acceleration, state);

        if (t > m_profile.decelTime) {
          state = target;
        }
      }
    }

    return state;
  }

  /**
   * Returns the time left until a target distance in the profile is reached.
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

    profile.accelTime += recoveryTime;

    return profile.accelTime + profile.cruiseTime + profile.decelTime;
  }

  /**
   * Returns the total time the profile takes to reach the goal.
   *
   * @return The total time the profile takes to reach the goal, or zero if no goal was set.
   */
  public double totalTime() {
    return m_profile.accelTime + m_profile.cruiseTime + m_profile.decelTime;
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
      current.position +=
          current.velocity * recoveryTime
              + Math.copySign(m_constraints.maxAcceleration, -current.velocity)
                  * recoveryTime
                  * recoveryTime
                  / 2.0;
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
   * @return 1.0 if the profile direcetion is positive, -1.0 if it is not.
   */
  double getSign(State current, State goal) {
    double dx = goal.position - current.position;

    // Threshold distance is the distance to traverse between the initial and
    // final velocities with extremal acceleration.
    // v₂² − v₁² = 2ax
    // Because the acceleration has the sign of v₂ − v₁, we can factor v₂² − v₁²
    // and take the absolute value of the v₂ − v₁ term.
    // |v₂ − v₁|(v₂ + v₁) / 2a = x
    double thresholdDistance =
        Math.abs(goal.velocity - current.velocity)
            / m_constraints.maxAcceleration
            * (current.velocity + goal.velocity)
            / 2.0;

    // Make sure that we always choose the fastest feasible direction.
    return Math.copySign(
        1.0, dx == thresholdDistance ? goal.velocity + current.velocity : dx - thresholdDistance);
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
    double distance = goal.position - current.position;

    // x₁ + x₂ = Δx
    // vₚ² − v₁² = 2ax₁
    // vₚ² − v₂² = 2ax₂
    // Δx = x₁ + x₂ = (2vₚ² − (v₁² + v₂²)) / 2a
    // vₚ = √(aΔx + (v₁² + v₂²) / 2)
    double peakVelocity =
        sign
            * Math.sqrt(
                (goal.velocity * goal.velocity + current.velocity * current.velocity) / 2
                    + acceleration * distance);

    // Handle the case where we hit maximum velocity.
    if (sign * peakVelocity > m_constraints.maxVelocity) {
      profile.accelTime = (velocityLimit - current.velocity) / acceleration;
      profile.decelTime = (velocityLimit - goal.velocity) / acceleration;

      // Δx = x₁ + x₂ + x₃ = (2vₚ² − (v₁² + v₂²)) / 2a + x₃
      // x₃ = Δx - (2vₚ² − (v₁² + v₂²)) / 2a
      // cruiseTime = x₃ / vₚ
      profile.cruiseTime =
          (distance
                  - (2 * velocityLimit * velocityLimit
                          - (current.velocity * current.velocity + goal.velocity * goal.velocity))
                      / (2 * acceleration))
              / velocityLimit;
    } else {
      profile.accelTime = (peakVelocity - current.velocity) / acceleration;
      profile.decelTime = (peakVelocity - goal.velocity) / acceleration;
    }

    return profile;
  }
}
