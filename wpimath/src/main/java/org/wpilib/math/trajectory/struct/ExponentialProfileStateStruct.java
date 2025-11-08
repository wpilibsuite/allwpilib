// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.trajectory.struct;

import org.wpilib.math.trajectory.ExponentialProfile;
import org.wpilib.util.struct.Struct;
import java.nio.ByteBuffer;

public class ExponentialProfileStateStruct implements Struct<ExponentialProfile.State> {
  @Override
  public Class<ExponentialProfile.State> getTypeClass() {
    return ExponentialProfile.State.class;
  }

  @Override
  public String getTypeName() {
    return "ExponentialProfileState";
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
  public ExponentialProfile.State unpack(ByteBuffer bb) {
    return new ExponentialProfile.State(bb.getDouble(), bb.getDouble());
  }

  @Override
  public void pack(ByteBuffer bb, ExponentialProfile.State value) {
    bb.putDouble(value.position);
    bb.putDouble(value.velocity);
  }
}
