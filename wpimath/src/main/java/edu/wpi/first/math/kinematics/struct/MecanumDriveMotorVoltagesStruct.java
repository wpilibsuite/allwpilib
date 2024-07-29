// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.kinematics.struct;

import edu.wpi.first.math.kinematics.MecanumDriveMotorVoltages;
import edu.wpi.first.util.struct.Struct;
import java.nio.ByteBuffer;

public final class MecanumDriveMotorVoltagesStruct implements Struct<MecanumDriveMotorVoltages> {
  @Override
  public Class<MecanumDriveMotorVoltages> getTypeClass() {
    return MecanumDriveMotorVoltages.class;
  }

  @Override
  public String getTypeName() {
    return "MecanumDriveMotorVoltages";
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
  public MecanumDriveMotorVoltages unpack(ByteBuffer bb) {
    double front_left = bb.getDouble();
    double front_right = bb.getDouble();
    double rear_left = bb.getDouble();
    double rear_right = bb.getDouble();
    return new MecanumDriveMotorVoltages(front_left, front_right, rear_left, rear_right);
  }

  @Override
  public void pack(ByteBuffer bb, MecanumDriveMotorVoltages value) {
    bb.putDouble(value.frontLeftVoltage);
    bb.putDouble(value.frontRightVoltage);
    bb.putDouble(value.rearLeftVoltage);
    bb.putDouble(value.rearRightVoltage);
  }
}
