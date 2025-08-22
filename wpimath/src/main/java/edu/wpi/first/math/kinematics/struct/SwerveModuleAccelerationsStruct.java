// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.kinematics.struct;

import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.kinematics.SwerveModuleAccelerations;
import edu.wpi.first.util.struct.Struct;
import java.nio.ByteBuffer;

public class SwerveModuleAccelerationsStruct implements Struct<SwerveModuleAccelerations> {
  @Override
  public Class<SwerveModuleAccelerations> getTypeClass() {
    return SwerveModuleAccelerations.class;
  }

  @Override
  public String getTypeName() {
    return "SwerveModuleAccelerations";
  }

  @Override
  public int getSize() {
    return kSizeDouble * 2;
  }

  @Override
  public String getSchema() {
    return "double acceleration;double angle";
  }

  @Override
  public SwerveModuleAccelerations unpack(ByteBuffer bb) {
    double acceleration = bb.getDouble();
    double angle = bb.getDouble();
    return new SwerveModuleAccelerations(acceleration, new Rotation2d(angle));
  }

  @Override
  public void pack(ByteBuffer bb, SwerveModuleAccelerations value) {
    bb.putDouble(value.acceleration);
    bb.putDouble(value.angle.getRadians());
  }
}
