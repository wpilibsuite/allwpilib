// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.kinematics.struct;

import edu.wpi.first.math.kinematics.DifferentialDriveWheelPositions;
import edu.wpi.first.util.struct.Struct;
import java.nio.ByteBuffer;

public class DifferentialDriveWheelPositionsStruct
    implements Struct<DifferentialDriveWheelPositions> {
  @Override
  public Class<DifferentialDriveWheelPositions> getTypeClass() {
    return DifferentialDriveWheelPositions.class;
  }

  @Override
  public String getTypeName() {
    return "DifferentialDriveWheelPositions";
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
  public DifferentialDriveWheelPositions unpack(ByteBuffer bb) {
    double left = bb.getDouble();
    double right = bb.getDouble();
    return new DifferentialDriveWheelPositions(left, right);
  }

  @Override
  public void pack(ByteBuffer bb, DifferentialDriveWheelPositions value) {
    bb.putDouble(value.leftMeters);
    bb.putDouble(value.rightMeters);
  }
}
