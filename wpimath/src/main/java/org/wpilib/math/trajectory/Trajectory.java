// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.trajectory;

import static org.wpilib.units.Units.Seconds;

import io.avaje.jsonb.Json;
import java.util.Arrays;
import java.util.Comparator;
import java.util.List;
import org.wpilib.math.interpolation.InterpolatingTreeMap;
import org.wpilib.math.util.MathUtil;
import org.wpilib.units.measure.Time;

/**
 * Represents a trajectory consisting of a list of {@link TrajectorySample}s, kinematically
 * interpolating between them.
 *
 * @param <SampleType> The type of the samples in the trajectory.
 */
public abstract class Trajectory<SampleType extends TrajectorySample> {
  /** The samples this Trajectory is composed of. */
  protected final List<SampleType> samples;

  @Json.Ignore private final InterpolatingTreeMap<Double, SampleType> sampleMap;

  /** The total duration of the trajectory. */
  @Json.Ignore public final double duration;

  /**
   * Constructs a Trajectory.
   *
   * @param samples the samples of the trajectory. Order does not matter as they will be ordered
   *     internally.
   */
  @SuppressWarnings({"this-escape"})
  public Trajectory(SampleType[] samples) {
    this(Arrays.asList(samples));
  }

  /**
   * Constructs a Trajectory.
   *
   * @param samples the samples of the trajectory. Order does not matter as they will be ordered
   *     internally.
   */
  @SuppressWarnings({"this-escape"})
  public Trajectory(List<SampleType> samples) {
    this.samples = samples.stream().sorted(Comparator.comparingDouble(s -> s.time)).toList();

    this.sampleMap = new InterpolatingTreeMap<>(MathUtil::inverseLerp, this::interpolate);

    for (var sample : this.samples) {
      sampleMap.put(sample.time, sample);
    }

    this.duration = this.samples.isEmpty() ? 0.0 : this.samples.getLast().time;
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
    return sampleMap.get(time);
  }
}
