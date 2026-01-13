// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.trajectory;

import static org.wpilib.units.Units.Seconds;

import com.fasterxml.jackson.annotation.JsonIgnore;
import java.util.Arrays;
import java.util.Comparator;
import java.util.List;
import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.geometry.Transform2d;
import org.wpilib.math.interpolation.InterpolatingTreeMap;
import org.wpilib.math.util.MathUtil;
import org.wpilib.units.measure.Time;

/**
 * Represents a trajectory consisting of a list of {@link TrajectorySample}s, kinematically
 * interpolating between them.
 */
public abstract class Trajectory<SampleType extends TrajectorySample> {
  /** The samples this Trajectory is composed of. */
  protected final SampleType[] samples;

  private final InterpolatingTreeMap<Double, SampleType> sampleMap;

  /** The total duration of the trajectory. */
  @JsonIgnore public final double duration;

  /**
   * Constructs a Trajectory.
   *
   * @param samples the samples of the trajectory. Order does not matter as they will be ordered
   *     internally.
   */
  @SuppressWarnings({"this-escape"})
  public Trajectory(SampleType[] samples) {
    this.samples =
        Arrays.stream(samples)
            .sorted(Comparator.comparingDouble(s -> s.timestamp))
            .toArray(size -> Arrays.copyOf(samples, size));

    this.sampleMap = new InterpolatingTreeMap<>(MathUtil::inverseLerp, this::interpolate);

    for (var sample : this.samples) {
      sampleMap.put(sample.timestamp, sample);
    }

    this.duration = this.samples.length > 0 ? this.samples[this.samples.length - 1].timestamp : 0.0;
  }

  /**
   * Gets the samples of the trajectory.
   *
   * @return the samples of the trajectory as an unmodifiable list.
   */
  public List<SampleType> getSamples() {
    return List.of(samples);
  }

  /**
   * Interpolates between two samples. This method must be implemented by subclasses to provide
   * drivetrain-specific interpolation logic.
   *
   * @param start The starting sample.
   * @param end The ending sample.
   * @param t The interpolation parameter between 0 and 1.
   * @return The interpolated sample.
   */
  public abstract SampleType interpolate(SampleType start, SampleType end, double t);

  /** Gets the first sample in the trajectory. */
  public SampleType start() {
    return samples[0];
  }

  /** Gets the last sample in the trajectory. */
  public SampleType end() {
    return samples[samples.length - 1];
  }

  /** Gets the sample at the given timestamp. */
  public SampleType sampleAt(Time timestamp) {
    return sampleAt(timestamp.in(Seconds));
  }

  /**
   * Gets the sample at the given timestamp.
   *
   * @param timestamp timestamp in seconds
   */
  public SampleType sampleAt(double timestamp) {
    return sampleMap.get(timestamp);
  }

  /**
   * Transforms all poses in the trajectory by the given transform. This is useful for converting a
   * robot-relative trajectory into a field-relative trajectory. This works with respect to the
   * first pose in the trajectory.
   *
   * @param transform The transform to transform the trajectory by.
   * @return The transformed trajectory.
   */
  public abstract Trajectory<SampleType> transformBy(Transform2d transform);

  /**
   * Concatenates this trajectory with another trajectory. If the other trajectory is empty, this
   * trajectory is returned. To work correctly, the other trajectory should start at the end of this
   * trajectory.
   *
   * @param other the other trajectory to concatenate with this one.
   * @return a new trajectory that is the concatenation of this trajectory and the other trajectory.
   */
  public abstract Trajectory<SampleType> concatenate(Trajectory<SampleType> other);

  /**
   * Returns a new trajectory that is relative to the given pose. This is useful for converting a
   * field-relative trajectory into a robot-relative trajectory. The returned trajectory will have
   * the same timestamps, velocities, and accelerations as the original trajectory, but the poses
   * will be relative to the given pose.
   *
   * @param other the pose to which the trajectory should be relative. This is typically the robot's
   *     starting pose.
   * @return a new trajectory that is relative to the given pose.
   */
  public abstract Trajectory<SampleType> relativeTo(Pose2d other);
}
