// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.trajectory;

import com.fasterxml.jackson.annotation.JsonProperty;
import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.geometry.Transform2d;
import org.wpilib.math.trajectory.proto.TrajectoryProto;
import org.wpilib.math.trajectory.proto.TrajectoryStateProto;
import edu.wpi.first.util.protobuf.ProtobufSerializable;
import java.util.ArrayList;
import java.util.List;
import java.util.Objects;
import java.util.stream.Collectors;

/**
 * Represents a time-parameterized trajectory. The trajectory contains of various States that
 * represent the pose, curvature, time elapsed, velocity, and acceleration at that point.
 */
public class Trajectory implements ProtobufSerializable {
  /** Trajectory protobuf for serialization. */
  public static final TrajectoryProto proto = new TrajectoryProto();

  private final double m_totalTime;
  private final List<State> m_states;

  /** Constructs an empty trajectory. */
  public Trajectory() {
    m_states = new ArrayList<>();
    m_totalTime = 0.0;
  }

  /**
   * Constructs a trajectory from a vector of states.
   *
   * @param states A vector of states.
   * @throws IllegalArgumentException if the vector of states is empty.
   */
  public Trajectory(final List<State> states) {
    m_states = states;

    if (m_states.isEmpty()) {
      throw new IllegalArgumentException("Trajectory manually created with no states.");
    }

    m_totalTime = m_states.get(m_states.size() - 1).time;
  }

  /**
   * Linearly interpolates between two values.
   *
   * @param startValue The start value.
   * @param endValue The end value.
   * @param t The fraction for interpolation.
   * @return The interpolated value.
   */
  private static double lerp(double startValue, double endValue, double t) {
    return startValue + (endValue - startValue) * t;
  }

  /**
   * Linearly interpolates between two poses.
   *
   * @param startValue The start pose.
   * @param endValue The end pose.
   * @param t The fraction for interpolation.
   * @return The interpolated pose.
   */
  private static Pose2d lerp(Pose2d startValue, Pose2d endValue, double t) {
    return startValue.plus((endValue.minus(startValue)).times(t));
  }

  /**
   * Returns the initial pose of the trajectory.
   *
   * @return The initial pose of the trajectory.
   */
  public Pose2d getInitialPose() {
    return sample(0).pose;
  }

  /**
   * Returns the overall duration of the trajectory.
   *
   * @return The duration of the trajectory in seconds.
   */
  public double getTotalTime() {
    return m_totalTime;
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
   * @param time The point in time since the beginning of the trajectory to sample in seconds.
   * @return The state at that point in time.
   * @throws IllegalStateException if the trajectory has no states.
   */
  public State sample(double time) {
    if (m_states.isEmpty()) {
      throw new IllegalStateException("Trajectory cannot be sampled if it has no states.");
    }

    if (time <= m_states.get(0).time) {
      return m_states.get(0);
    }
    if (time >= m_totalTime) {
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
      if (m_states.get(mid).time < time) {
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
    if (Math.abs(sample.time - prevSample.time) < 1E-9) {
      return sample;
    }
    // Interpolate between the two states for the state that we want.
    return prevSample.interpolate(
        sample, (time - prevSample.time) / (sample.time - prevSample.time));
  }

  /**
   * Transforms all poses in the trajectory by the given transform. This is useful for converting a
   * robot-relative trajectory into a field-relative trajectory. This works with respect to the
   * first pose in the trajectory.
   *
   * @param transform The transform to transform the trajectory by.
   * @return The transformed trajectory.
   */
  public Trajectory transformBy(Transform2d transform) {
    var firstState = m_states.get(0);
    var firstPose = firstState.pose;

    // Calculate the transformed first pose.
    var newFirstPose = firstPose.plus(transform);
    List<State> newStates = new ArrayList<>();

    newStates.add(
        new State(
            firstState.time,
            firstState.velocity,
            firstState.acceleration,
            newFirstPose,
            firstState.curvature));

    for (int i = 1; i < m_states.size(); i++) {
      var state = m_states.get(i);
      // We are transforming relative to the coordinate frame of the new initial pose.
      newStates.add(
          new State(
              state.time,
              state.velocity,
              state.acceleration,
              newFirstPose.plus(state.pose.minus(firstPose)),
              state.curvature));
    }

    return new Trajectory(newStates);
  }

  /**
   * Transforms all poses in the trajectory so that they are relative to the given pose. This is
   * useful for converting a field-relative trajectory into a robot-relative trajectory.
   *
   * @param pose The pose that is the origin of the coordinate frame that the current trajectory
   *     will be transformed into.
   * @return The transformed trajectory.
   */
  public Trajectory relativeTo(Pose2d pose) {
    return new Trajectory(
        m_states.stream()
            .map(
                state ->
                    new State(
                        state.time,
                        state.velocity,
                        state.acceleration,
                        state.pose.relativeTo(pose),
                        state.curvature))
            .collect(Collectors.toList()));
  }

  /**
   * Concatenates another trajectory to the current trajectory. The user is responsible for making
   * sure that the end pose of this trajectory and the start pose of the other trajectory match (if
   * that is the desired behavior).
   *
   * @param other The trajectory to concatenate.
   * @return The concatenated trajectory.
   */
  public Trajectory concatenate(Trajectory other) {
    // If this is a default constructed trajectory with no states, then we can
    // simply return the rhs trajectory.
    if (m_states.isEmpty()) {
      return other;
    }

    // Deep copy the current states.
    List<State> states =
        m_states.stream()
            .map(
                state ->
                    new State(
                        state.time,
                        state.velocity,
                        state.acceleration,
                        state.pose,
                        state.curvature))
            .collect(Collectors.toList());

    // Here we omit the first state of the other trajectory because we don't want
    // two time points with different states. Sample() will automatically
    // interpolate between the end of this trajectory and the second state of the
    // other trajectory.
    for (int i = 1; i < other.getStates().size(); ++i) {
      var s = other.getStates().get(i);
      states.add(new State(s.time + m_totalTime, s.velocity, s.acceleration, s.pose, s.curvature));
    }
    return new Trajectory(states);
  }

  /**
   * Represents a time-parameterized trajectory. The trajectory contains of various States that
   * represent the pose, curvature, time elapsed, velocity, and acceleration at that point.
   */
  public static class State implements ProtobufSerializable {
    /** Trajectory.State protobuf for serialization. */
    public static final TrajectoryStateProto proto = new TrajectoryStateProto();

    /** The time elapsed since the beginning of the trajectory in seconds. */
    @JsonProperty("time")
    public double time;

    /** The speed at that point of the trajectory in meters per second. */
    @JsonProperty("velocity")
    public double velocity;

    /** The acceleration at that point of the trajectory in m/s². */
    @JsonProperty("acceleration")
    public double acceleration;

    /** The pose at that point of the trajectory. */
    @JsonProperty("pose")
    public Pose2d pose;

    /** The curvature at that point of the trajectory in rad/m. */
    @JsonProperty("curvature")
    public double curvature;

    /** Default constructor. */
    public State() {
      pose = Pose2d.kZero;
    }

    /**
     * Constructs a State with the specified parameters.
     *
     * @param time The time elapsed since the beginning of the trajectory in seconds.
     * @param velocity The speed at that point of the trajectory in m/s.
     * @param acceleration The acceleration at that point of the trajectory in m/s².
     * @param pose The pose at that point of the trajectory.
     * @param curvature The curvature at that point of the trajectory in rad/m.
     */
    public State(double time, double velocity, double acceleration, Pose2d pose, double curvature) {
      this.time = time;
      this.velocity = velocity;
      this.acceleration = acceleration;
      this.pose = pose;
      this.curvature = curvature;
    }

    /**
     * Interpolates between two States.
     *
     * @param endValue The end value for the interpolation.
     * @param i The interpolant (fraction).
     * @return The interpolated state.
     */
    State interpolate(State endValue, double i) {
      // Find the new t value.
      final double newT = lerp(time, endValue.time, i);

      // Find the delta time between the current state and the interpolated state.
      final double deltaT = newT - time;

      // If delta time is negative, flip the order of interpolation.
      if (deltaT < 0) {
        return endValue.interpolate(this, 1 - i);
      }

      // Check whether the robot is reversing at this stage.
      final boolean reversing = velocity < 0 || Math.abs(velocity) < 1E-9 && acceleration < 0;

      // Calculate the new velocity
      // v_f = v_0 + at
      final double newV = velocity + (acceleration * deltaT);

      // Calculate the change in position.
      // delta_s = v_0 t + 0.5at²
      final double newS =
          (velocity * deltaT + 0.5 * acceleration * Math.pow(deltaT, 2)) * (reversing ? -1.0 : 1.0);

      // Return the new state. To find the new position for the new state, we need
      // to interpolate between the two endpoint poses. The fraction for
      // interpolation is the change in position (delta s) divided by the total
      // distance between the two endpoints.
      final double interpolationFrac =
          newS / endValue.pose.getTranslation().getDistance(pose.getTranslation());

      return new State(
          newT,
          newV,
          acceleration,
          lerp(pose, endValue.pose, interpolationFrac),
          lerp(curvature, endValue.curvature, interpolationFrac));
    }

    @Override
    public String toString() {
      return String.format(
          "State(Sec: %.2f, Vel m/s: %.2f, Accel m/s/s: %.2f, Pose: %s, Curvature: %.2f)",
          time, velocity, acceleration, pose, curvature);
    }

    @Override
    public boolean equals(Object obj) {
      return obj instanceof State state
          && Double.compare(state.time, time) == 0
          && Double.compare(state.velocity, velocity) == 0
          && Double.compare(state.acceleration, acceleration) == 0
          && Double.compare(state.curvature, curvature) == 0
          && Objects.equals(pose, state.pose);
    }

    @Override
    public int hashCode() {
      return Objects.hash(time, velocity, acceleration, pose, curvature);
    }
  }

  @Override
  public String toString() {
    String stateList = m_states.stream().map(State::toString).collect(Collectors.joining(", \n"));
    return String.format("Trajectory - Seconds: %.2f, States:\n%s", m_totalTime, stateList);
  }

  @Override
  public int hashCode() {
    return m_states.hashCode();
  }

  @Override
  public boolean equals(Object obj) {
    return obj instanceof Trajectory other && m_states.equals(other.getStates());
  }
}
