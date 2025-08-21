// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.kinematics.struct;

import edu.wpi.first.math.kinematics.DifferentialDriveWheelAccelerations;
import edu.wpi.first.util.struct.Struct;
import java.nio.ByteBuffer;

public class DifferentialDriveWheelAccelerationsStruct implements Struct<DifferentialDriveWheelAccelerations> {
  @Override
  public Class<DifferentialDriveWheelAccelerations> getTypeClass() {
    return DifferentialDriveWheelAccelerations.class;
  }

  @Override
  public String getTypeName() {
    return "DifferentialDriveWheelAccelerations";
  }

  @Override
  public int getSize() {
    return kSizeDouble * 2;
  }

  @Override
  public String getSchema() {
    return "double left;double right";
  }

  @Override
  public DifferentialDriveWheelAccelerations unpack(ByteBuffer bb) {
    double left = bb.getDouble();
    double right = bb.getDouble();
    return new DifferentialDriveWheelAccelerations(left, right);
  }

  @Override
  public void pack(ByteBuffer bb, DifferentialDriveWheelAccelerations value) {
    bb.putDouble(value.left);
    bb.putDouble(value.right);
  }
}
