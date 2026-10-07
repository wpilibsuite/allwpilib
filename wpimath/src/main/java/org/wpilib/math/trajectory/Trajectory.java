// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.trajectory;

import static org.wpilib.units.Units.Seconds;

import java.util.Arrays;
import java.util.Comparator;
import java.util.List;
import org.wpilib.units.measure.Time;
import org.wpilib.util.collections.Search;

/**
 * Represents a trajectory consisting of a list of {@link TrajectorySample}s, kinematically
 * interpolating between them.
 *
 * @param <SampleType> The type of the samples in the trajectory.
 */
public abstract class Trajectory<SampleType extends TrajectorySample> {
  /** The samples this Trajectory is composed of. */
  protected final List<SampleType> samples;

  /**
   * Constructs a Trajectory.
   *
   * @param samples the samples of the trajectory. Order does not matter as they will be ordered
   *     internally.
   * @throws IllegalArgumentException if there are no samples or the earliest sample's time is not
   *     zero.
   */
  public Trajectory(SampleType[] samples) {
    this(Arrays.asList(samples));
  }

  /**
   * Constructs a Trajectory.
   *
   * @param samples the samples of the trajectory. Order does not matter as they will be ordered
   *     internally.
   * @throws IllegalArgumentException if there are no samples or the earliest sample's time is not
   *     zero.
   */
  public Trajectory(List<SampleType> samples) {
    if (samples.isEmpty()) {
      throw new IllegalArgumentException("Trajectory manually initialized with no samples.");
    }

    this.samples =
        samples.stream().sorted(Comparator.comparingDouble(TrajectorySample::getTime)).toList();

    if (this.samples.getFirst().getTime() != 0.0) {
      throw new IllegalArgumentException(
          "Trajectory sample times must be relative to the trajectory start "
              + "(the first sample must have time 0).");
    }
  }

  /**
   * Gets the samples of the trajectory.
   *
   * @return the samples of the trajectory as an unmodifiable list.
   */
  public List<SampleType> getSamples() {
    return samples;
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

  /**
   * Gets the total duration of the trajectory, in seconds.
   *
   * @return the duration of the trajectory in seconds.
   */
  public double duration() {
    return end().getTime();
  }

  /**
   * Gets the first sample in the trajectory.
   *
   * @return the first sample in the trajectory.
   */
  public SampleType start() {
    return samples.getFirst();
  }

  /**
   * Gets the last sample in the trajectory.
   *
   * @return the last sample in the trajectory.
   */
  public SampleType end() {
    return samples.getLast();
  }

  /**
   * Gets the sample at the given time.
   *
   * @param time the time since the beginning of the trajectory to sample.
   * @return the sample at that point in time.
   */
  public SampleType sampleAt(Time time) {
    return sampleAt(time.in(Seconds));
  }

  /**
   * Gets the sample at the given time.
   *
   * @param time the time since the beginning of the trajectory to sample, in seconds.
   * @return the sample at that point in time.
   */
  public SampleType sampleAt(double time) {
    if (time < 0.0) {
      return start();
    }
    if (time > duration()) {
      return end();
    }

    int index = Search.binarySearch(samples, time, TrajectorySample::getTime);
    if (index >= 0) {
      // Exact match, no interpolation needed
      return samples.get(index);
    }

    // The insertion point is the first sample after the requested time. The early returns above
    // guarantee that it is neither the first sample nor past the last one.
    var upper = samples.get(-index - 1);
    var lower = samples.get(-index - 2);
    double param = (time - lower.getTime()) / (upper.getTime() - lower.getTime());
    return interpolate(lower, upper, param);
  }
}
