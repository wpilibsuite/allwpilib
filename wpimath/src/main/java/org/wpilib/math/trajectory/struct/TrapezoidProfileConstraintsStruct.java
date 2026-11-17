// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.trajectory.struct;

import java.nio.ByteBuffer;
import org.wpilib.math.trajectory.TrapezoidProfile;
import org.wpilib.util.struct.Struct;

public class TrapezoidProfileConstraintsStruct implements Struct<TrapezoidProfile.Constraints> {
  @Override
  public Class<TrapezoidProfile.Constraints> getTypeClass() {
    return TrapezoidProfile.Constraints.class;
  }

  @Override
  public String getTypeName() {
    return "TrapezoidProfileConstraints";
  }

  @Override
  public int getSize() {
    return DOUBLE_SIZE * 2;
  }

  @Override
  public String getSchema() {
    return "double maxVelocity;double maxAcceleration";
  }

  @Override
  public TrapezoidProfile.Constraints unpack(ByteBuffer bb) {
    return new TrapezoidProfile.Constraints(bb.getDouble(), bb.getDouble());
  }

  @Override
  public void pack(ByteBuffer bb, TrapezoidProfile.Constraints value) {
    bb.putDouble(value.maxVelocity);
    bb.putDouble(value.maxAcceleration);
  }
}
