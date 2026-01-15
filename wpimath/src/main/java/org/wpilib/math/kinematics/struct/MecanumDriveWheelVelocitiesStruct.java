// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics.struct;

import java.nio.ByteBuffer;
import org.wpilib.math.kinematics.MecanumDriveWheelVelocities;
import org.wpilib.util.struct.Struct;

public class MecanumDriveWheelVelocitiesStruct implements Struct<MecanumDriveWheelVelocities> {
  @Override
  public Class<MecanumDriveWheelVelocities> getTypeClass() {
    return MecanumDriveWheelVelocities.class;
  }

  @Override
  public String getTypeName() {
    return "MecanumDriveWheelVelocities";
  }

  @Override
  public int getSize() {
    return kSizeDouble * 4;
  }

  @Override
  public String getSchema() {
    return "double front_left;double front_right;double rear_left;double rear_right";
  }

  @Override
  public MecanumDriveWheelVelocities unpack(ByteBuffer bb) {
    double frontLeft = bb.getDouble();
    double frontRight = bb.getDouble();
    double rearLeft = bb.getDouble();
    double rearRight = bb.getDouble();
    return new MecanumDriveWheelVelocities(frontLeft, frontRight, rearLeft, rearRight);
  }

  @Override
  public void pack(ByteBuffer bb, MecanumDriveWheelVelocities value) {
    bb.putDouble(value.frontLeft);
    bb.putDouble(value.frontRight);
    bb.putDouble(value.rearLeft);
    bb.putDouble(value.rearRight);
  }
}
