// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.trajectory;

import static org.wpilib.units.Units.Seconds;

import java.util.Objects;
import org.wpilib.units.measure.Time;

/** Represents a single sample in a trapezoid profile trajectory. */
public class TrapezoidProfileSample implements TrajectorySample {
  /** The time of the sample relative to the profile start, in seconds. */
  public final double time;

  /** The position at this sample. */
  public final double position;

  /** The velocity at this sample. */
  public final double velocity;

  /** The acceleration at this sample. */
  public final double acceleration;

  /**
   * Constructs a TrapezoidProfileSample.
   *
   * @param time The time of the sample relative to the profile start, in seconds.
   * @param position The position at this sample.
   * @param velocity The velocity at this sample.
   * @param acceleration The acceleration at this sample.
   */
  public TrapezoidProfileSample(
      double time, double position, double velocity, double acceleration) {
    this.time = time;
    this.position = position;
    this.velocity = velocity;
    this.acceleration = acceleration;
  }

  /**
   * Constructs a TrapezoidProfileSample.
   *
   * @param time The time of the sample relative to the profile start.
   * @param position The position at this sample.
   * @param velocity The velocity at this sample.
   * @param acceleration The acceleration at this sample.
   */
  public TrapezoidProfileSample(Time time, double position, double velocity, double acceleration) {
    this(time.in(Seconds), position, velocity, acceleration);
  }

  @Override
  public double getTime() {
    return time;
  }

  @Override
  public int hashCode() {
    return Objects.hash(time, position, velocity, acceleration);
  }

  @Override
  public boolean equals(Object o) {
    if (this == o) {
      return true;
    }
    if (o == null || getClass() != o.getClass()) {
      return false;
    }

    TrapezoidProfileSample that = (TrapezoidProfileSample) o;
    return Double.compare(time, that.time) == 0
        && Double.compare(position, that.position) == 0
        && Double.compare(velocity, that.velocity) == 0
        && Double.compare(acceleration, that.acceleration) == 0;
  }
}
