// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.trajectory;

import static org.wpilib.units.Units.Seconds;

import io.avaje.jsonb.Json;
import java.util.Objects;
import org.wpilib.math.trajectory.proto.TrajectorySampleProto;
import org.wpilib.math.trajectory.struct.TrajectorySampleStruct;
import org.wpilib.units.measure.Time;
import org.wpilib.util.protobuf.ProtobufSerializable;
import org.wpilib.util.struct.StructSerializable;

/** Represents a single sample in a trajectory. */
@Json
public class TrajectorySample implements StructSerializable, ProtobufSerializable {
  /** The time of the sample relative to the trajectory start, in seconds. */
  @Json.Property("time")
  public double time;

  /** Base struct for serialization. */
  public static final TrajectorySampleStruct struct = new TrajectorySampleStruct();

  /** Base proto for serialization. */
  public static final TrajectorySampleProto proto = new TrajectorySampleProto();

  /**
   * Constructs a TrajectorySample.
   *
   * @param time The time of the sample relative to the trajectory start, in seconds.
   */
  @Json.Creator
  public TrajectorySample(double time) {
    this.time = time;
  }

  /**
   * Constructs a TrajectorySample.
   *
   * @param time The time of the sample relative to the trajectory start.
   */
  public TrajectorySample(Time time) {
    this(time.in(Seconds));
  }

  @Override
  public int hashCode() {
    return Objects.hash(time);
  }

  @Override
  public boolean equals(Object o) {
    if (this == o) {
      return true;
    }
    if (o == null || getClass() != o.getClass()) {
      return false;
    }

    TrajectorySample that = (TrajectorySample) o;
    return Double.compare(time, that.time) == 0;
  }
}
