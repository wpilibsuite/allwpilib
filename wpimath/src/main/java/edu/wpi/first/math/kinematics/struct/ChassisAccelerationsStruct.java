// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.kinematics.struct;

import edu.wpi.first.math.kinematics.ChassisAccelerations;
import edu.wpi.first.util.struct.Struct;
import java.nio.ByteBuffer;

public class ChassisAccelerationsStruct implements Struct<ChassisAccelerations> {
  @Override
  public Class<ChassisAccelerations> getTypeClass() {
    return ChassisAccelerations.class;
  }

  @Override
  public String getTypeName() {
    return "ChassisAccelerations";
  }

  @Override
  public int getSize() {
    return kSizeDouble * 3;
  }

  @Override
  public String getSchema() {
    return "double ax;double ay;double alpha";
  }

  @Override
  public ChassisAccelerations unpack(ByteBuffer bb) {
    double ax = bb.getDouble();
    double ay = bb.getDouble();
    double alpha = bb.getDouble();
    return new ChassisAccelerations(ax, ay, alpha);
  }

  @Override
  public void pack(ByteBuffer bb, ChassisAccelerations value) {
    bb.putDouble(value.ax);
    bb.putDouble(value.ay);
    bb.putDouble(value.alpha);
  }
}
