// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics.struct;

import java.nio.ByteBuffer;
import org.wpilib.math.kinematics.DifferentialDriveWheelVelocities;
import org.wpilib.util.struct.Struct;

public class DifferentialDriveWheelVelocitiesStruct
    implements Struct<DifferentialDriveWheelVelocities> {
  @Override
  public Class<DifferentialDriveWheelVelocities> getTypeClass() {
    return DifferentialDriveWheelVelocities.class;
  }

  @Override
  public String getTypeName() {
    return "DifferentialDriveWheelVelocities";
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
  public DifferentialDriveWheelVelocities unpack(ByteBuffer bb) {
    double left = bb.getDouble();
    double right = bb.getDouble();
    return new DifferentialDriveWheelVelocities(left, right);
  }

  @Override
  public void pack(ByteBuffer bb, DifferentialDriveWheelVelocities value) {
    bb.putDouble(value.left);
    bb.putDouble(value.right);
  }
}
