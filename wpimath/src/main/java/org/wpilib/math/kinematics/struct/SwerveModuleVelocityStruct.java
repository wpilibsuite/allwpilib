// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics.struct;

import java.nio.ByteBuffer;
import org.wpilib.math.geometry.Rotation2d;
import org.wpilib.math.kinematics.SwerveModuleVelocity;
import org.wpilib.util.struct.Struct;

public class SwerveModuleVelocityStruct implements Struct<SwerveModuleVelocity> {
  @Override
  public Class<SwerveModuleVelocity> getTypeClass() {
    return SwerveModuleVelocity.class;
  }

  @Override
  public String getTypeName() {
    return "SwerveModuleVelocity";
  }

  @Override
  public int getSize() {
    return kSizeDouble + Rotation2d.struct.getSize();
  }

  @Override
  public String getSchema() {
    return "double velocity;Rotation2d angle";
  }

  @Override
  public Struct<?>[] getNested() {
    return new Struct<?>[] {Rotation2d.struct};
  }

  @Override
  public SwerveModuleVelocity unpack(ByteBuffer bb) {
    double velocity = bb.getDouble();
    Rotation2d angle = Rotation2d.struct.unpack(bb);
    return new SwerveModuleVelocity(velocity, angle);
  }

  @Override
  public void pack(ByteBuffer bb, SwerveModuleVelocity value) {
    bb.putDouble(value.velocity);
    Rotation2d.struct.pack(bb, value.angle);
  }
}
