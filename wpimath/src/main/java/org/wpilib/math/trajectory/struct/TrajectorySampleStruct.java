// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.trajectory.struct;

import java.nio.ByteBuffer;
import org.wpilib.math.trajectory.TrajectorySample;
import org.wpilib.util.struct.Struct;

public class TrajectorySampleStruct implements Struct<TrajectorySample> {
  @Override
  public Class<TrajectorySample> getTypeClass() {
    return TrajectorySample.class;
  }

  @Override
  public String getTypeName() {
    return "TrajectorySample.Base";
  }

  @Override
  public int getSize() {
    return DOUBLE_SIZE;
  }

  @Override
  public String getSchema() {
    return "double time";
  }

  @Override
  public TrajectorySample unpack(ByteBuffer bb) {
    double time = bb.getDouble();
    return new TrajectorySample(time);
  }

  @Override
  public void pack(ByteBuffer bb, TrajectorySample value) {
    bb.putDouble(value.time);
  }
}
