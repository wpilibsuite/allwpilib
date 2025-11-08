// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.trajectory.struct;

import org.wpilib.math.trajectory.TrapezoidProfile;
import org.wpilib.util.struct.Struct;
import java.nio.ByteBuffer;

public class TrapezoidProfileStateStruct implements Struct<TrapezoidProfile.State> {
  @Override
  public Class<TrapezoidProfile.State> getTypeClass() {
    return TrapezoidProfile.State.class;
  }

  @Override
  public String getTypeName() {
    return "TrapezoidProfileState";
  }

  @Override
  public int getSize() {
    return kSizeDouble * 2;
  }

  @Override
  public String getSchema() {
    return "double position;double velocity";
  }

  @Override
  public TrapezoidProfile.State unpack(ByteBuffer bb) {
    return new TrapezoidProfile.State(bb.getDouble(), bb.getDouble());
  }

  @Override
  public void pack(ByteBuffer bb, TrapezoidProfile.State value) {
    bb.putDouble(value.position);
    bb.putDouble(value.velocity);
  }
}
