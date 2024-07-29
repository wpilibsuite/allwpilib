// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.kinematics.struct;

import edu.wpi.first.math.kinematics.DifferentialDriveWheelSpeeds;
import edu.wpi.first.util.struct.Struct;
import java.nio.ByteBuffer;

public class DifferentialDriveWheelSpeedsStruct implements Struct<DifferentialDriveWheelSpeeds> {
  @Override
  public Class<DifferentialDriveWheelSpeeds> getTypeClass() {
    return DifferentialDriveWheelSpeeds.class;
  }

  @Override
  public String getTypeName() {
    return "DifferentialDriveWheelSpeeds";
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
  public DifferentialDriveWheelSpeeds unpack(ByteBuffer bb) {
    double left = bb.getDouble();
    double right = bb.getDouble();
    return new DifferentialDriveWheelSpeeds(left, right);
  }

  @Override
  public void pack(ByteBuffer bb, DifferentialDriveWheelSpeeds value) {
    bb.putDouble(value.leftMetersPerSecond);
    bb.putDouble(value.rightMetersPerSecond);
  }
}
