// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics.struct;

import java.nio.ByteBuffer;
import org.wpilib.math.geometry.Rotation2d;
import org.wpilib.math.kinematics.SwerveModuleAccelerations;
import org.wpilib.util.struct.Struct;

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
    return kSizeDouble + Rotation2d.struct.getSize();
  }

  @Override
  public String getSchema() {
    return "double acceleration;Rotation2d angle";
  }

  @Override
  public Struct<?>[] getNested() {
    return new Struct<?>[] {Rotation2d.struct};
  }

  @Override
  public SwerveModuleAccelerations unpack(ByteBuffer bb) {
    double acceleration = bb.getDouble();
    Rotation2d angle = Rotation2d.struct.unpack(bb);
    return new SwerveModuleAccelerations(acceleration, angle);
  }

  @Override
  public void pack(ByteBuffer bb, SwerveModuleAccelerations value) {
    bb.putDouble(value.acceleration);
    Rotation2d.struct.pack(bb, value.angle);
  }
}
