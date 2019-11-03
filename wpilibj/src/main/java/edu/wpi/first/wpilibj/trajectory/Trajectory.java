/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.trajectory;

import java.util.List;
import java.util.Objects;
import java.util.stream.Collectors;

import com.fasterxml.jackson.annotation.JsonProperty;

import edu.wpi.first.wpilibj.geometry.Pose2d;

/**
 * Represents a time-parameterized trajectory. The trajectory contains of
 * various States that represent the pose, curvature, time elapsed, velocity,
 * and acceleration at that point.
 */
public class Trajectory {
  private final double m_totalTimeSeconds;
  private final List<State> m_states;

  /**
   * Constructs a trajectory from a vector of states.
   *
   * @param states A vector of states.
   */
  public Trajectory(final List<State> states) {
    m_states = states;
    m_totalTimeSeconds = m_states.get(m_states.size() - 1).timeSeconds;
  }

  /**
   * Linearly interpolates between two values.
   *
   * @param startValue The start value.
   * @param endValue   The end value.
   * @param t          The fraction for interpolation.
   * @return The interpolated value.
   */
  @SuppressWarnings("ParameterName")
  private static double lerp(double startValue, double endValue, double t) {
    return startValue + (endValue - startValue) * t;
  }

  /**
   * Linearly interpolates between two poses.
   *
   * @param startValue The start pose.
   * @param endValue   The end pose.
   * @param t          The fraction for interpolation.
   * @return The interpolated pose.
   */
  @SuppressWarnings("ParameterName")
  private static Pose2d lerp(Pose2d startValue, Pose2d endValue, double t) {
    return startValue.plus((endValue.minus(startValue)).times(t));
  }

  /**
   * Returns the overall duration of the trajectory.
   *
   * @return The duration of the trajectory.
   */
  public double getTotalTimeSeconds() {
    return m_totalTimeSeconds;
  }

  /**
   * Return the states of the trajectory.
   *
   * @return The states of the trajectory.
   */
  public List<State> getStates() {
    return m_states;
  }

  /**
   * Sample the trajectory at a point in time.
   *
   * @param timeSeconds The point in time since the beginning of the trajectory to sample.
   * @return The state at that point in time.
   */
  public State sample(double timeSeconds) {
    if (timeSeconds <= m_states.get(0).timeSeconds) {
      return m_states.get(0);
    }
    if (timeSeconds >= m_totalTimeSeconds) {
      return m_states.get(m_states.size() - 1);
    }

    // To get the element that we want, we will use a binary search algorithm
    // instead of iterating over a for-loop. A binary search is O(std::log(n))
    // whereas searching using a loop is O(n).

    // This starts at 1 because we use the previous state later on for
    // interpolation.
    int low = 1;
    int high = m_states.size() - 1;

    while (low != high) {
      int mid = (low + high) / 2;
      if (m_states.get(mid).timeSeconds < timeSeconds) {
        // This index and everything under it are less than the requested
        // timestamp. Therefore, we can discard them.
        low = mid + 1;
      } else {
        // t is at least as large as the element at this index. This means that
        // anything after it cannot be what we are looking for.
        high = mid;
      }
    }

    // High and Low should be the same.

    // The sample's timestamp is now greater than or equal to the requested
    // timestamp. If it is greater, we need to interpolate between the
    // previous state and the current state to get the exact state that we
    // want.
    final State sample = m_states.get(low);
    final State prevSample = m_states.get(low - 1);

    // If the difference in states is negligible, then we are spot on!
    if (Math.abs(sample.timeSeconds - prevSample.timeSeconds) < 1E-9) {
      return sample;
    }
    // Interpolate between the two states for the state that we want.
    return prevSample.interpolate(sample,
        (timeSeconds - prevSample.timeSeconds) / (sample.timeSeconds - prevSample.timeSeconds));
  }

  /**
   * Represents a time-parameterized trajectory. The trajectory contains of
   * various States that represent the pose, curvature, time elapsed, velocity,
   * and acceleration at that point.
   */
  @SuppressWarnings("MemberName")
  public static class State {
    // The time elapsed since the beginning of the trajectory.
    @JsonProperty("time")
    public double timeSeconds;

    // The speed at that point of the trajectory.
    @JsonProperty("velocity")
    public double velocityMetersPerSecond;

    // The acceleration at that point of the trajectory.
    @JsonProperty("acceleration")
    public double accelerationMetersPerSecondSq;

    // The pose at that point of the trajectory.
    @JsonProperty("pose")
    public Pose2d poseMeters;

    // The curvature at that point of the trajectory.
    @JsonProperty("curvature")
    public double curvatureRadPerMeter;

    public State() {
      poseMeters = new Pose2d();
    }

    /**
     * Constructs a State with the specified parameters.
     *
     * @param timeSeconds                   The time elapsed since the beginning of the trajectory.
     * @param velocityMetersPerSecond       The speed at that point of the trajectory.
     * @param accelerationMetersPerSecondSq The acceleration at that point of the trajectory.
     * @param poseMeters                    The pose at that point of the trajectory.
     * @param curvatureRadPerMeter          The curvature at that point of the trajectory.
     */
    public State(double timeSeconds, double velocityMetersPerSecond,
                 double accelerationMetersPerSecondSq, Pose2d poseMeters,
                 double curvatureRadPerMeter) {
      this.timeSeconds = timeSeconds;
      this.velocityMetersPerSecond = velocityMetersPerSecond;
      this.accelerationMetersPerSecondSq = accelerationMetersPerSecondSq;
      this.poseMeters = poseMeters;
      this.curvatureRadPerMeter = curvatureRadPerMeter;
    }

    /**
     * Interpolates between two States.
     *
     * @param endValue The end value for the interpolation.
     * @param i        The interpolant (fraction).
     * @return The interpolated state.
     */
    @SuppressWarnings("ParameterName")
    State interpolate(State endValue, double i) {
      // Find the new t value.
      final double newT = lerp(timeSeconds, endValue.timeSeconds, i);

      // Find the delta time between the current state and the interpolated state.
      final double deltaT = newT - timeSeconds;

      // If delta time is negative, flip the order of interpolation.
      if (deltaT < 0) {
        return endValue.interpolate(this, 1 - i);
      }

      // Check whether the robot is reversing at this stage.
      final boolean reversing = velocityMetersPerSecond < 0
          || Math.abs(velocityMetersPerSecond) < 1E-9 && accelerationMetersPerSecondSq < 0;

      // Calculate the new velocity
      // v_f = v_0 + at
      final double newV = velocityMetersPerSecond + (accelerationMetersPerSecondSq * deltaT);

      // Calculate the change in position.
      // delta_s = v_0 t + 0.5 at^2
      final double newS = (velocityMetersPerSecond * deltaT
          + 0.5 * accelerationMetersPerSecondSq * Math.pow(deltaT, 2)) * (reversing ? -1.0 : 1.0);

      // Return the new state. To find the new position for the new state, we need
      // to interpolate between the two endpoint poses. The fraction for
      // interpolation is the change in position (delta s) divided by the total
      // distance between the two endpoints.
      final double interpolationFrac = newS
          / endValue.poseMeters.getTranslation().getDistance(poseMeters.getTranslation());

      return new State(
          newT, newV, accelerationMetersPerSecondSq,
          lerp(poseMeters, endValue.poseMeters, interpolationFrac),
          lerp(curvatureRadPerMeter, endValue.curvatureRadPerMeter, interpolationFrac)
      );
    }

    @Override
    public String toString() {
      return String.format(
        "State(Sec: %.2f, Vel m/s: %.2f, Accel m/s/s: %.2f, Pose: %s, Curvature: %.2f)",
        timeSeconds, velocityMetersPerSecond, accelerationMetersPerSecondSq,
        poseMeters, curvatureRadPerMeter);
    }

    @Override
    public boolean equals(Object obj) {
      if (this == obj) {
        return true;
      }
      if (!(obj instanceof State)) {
        return false;
      }
      State state = (State) obj;
      return Double.compare(state.timeSeconds, timeSeconds) == 0
              && Double.compare(state.velocityMetersPerSecond, velocityMetersPerSecond) == 0
              && Double.compare(state.accelerationMetersPerSecondSq,
                accelerationMetersPerSecondSq) == 0
              && Double.compare(state.curvatureRadPerMeter, curvatureRadPerMeter) == 0
              && Objects.equals(poseMeters, state.poseMeters);
    }

    @Override
    public int hashCode() {
      return Objects.hash(timeSeconds, velocityMetersPerSecond,
              accelerationMetersPerSecondSq, poseMeters, curvatureRadPerMeter);
    }
  }

  @Override
  public String toString() {
    String stateList = m_states.stream().map(State::toString).collect(Collectors.joining(", \n"));
    return String.format("Trajectory - Seconds: %.2f, States:\n%s", m_totalTimeSeconds, stateList);
  }
}
