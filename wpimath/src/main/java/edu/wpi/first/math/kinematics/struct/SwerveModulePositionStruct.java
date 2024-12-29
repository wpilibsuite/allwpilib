// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.kinematics.struct;

import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.kinematics.SwerveModulePosition;
import edu.wpi.first.util.struct.Struct;
import java.nio.ByteBuffer;

public class SwerveModulePositionStruct implements Struct<SwerveModulePosition> {
  @Override
  public Class<SwerveModulePosition> getTypeClass() {
    return SwerveModulePosition.class;
  }

  @Override
  public String getTypeName() {
    return "SwerveModulePosition";
  }

  @Override
  public int getSize() {
    return kSizeDouble + Rotation2d.struct.getSize();
  }

  @Override
  public String getSchema() {
    return "double distance;Rotation2d angle";
  }

  @Override
  public Struct<?>[] getNested() {
    return new Struct<?>[] {Rotation2d.struct};
  }

  @Override
  public SwerveModulePosition unpack(ByteBuffer bb) {
    double distance = bb.getDouble();
    Rotation2d angle = Rotation2d.struct.unpack(bb);
    return new SwerveModulePosition(distance, angle);
  }

  @Override
  public void pack(ByteBuffer bb, SwerveModulePosition value) {
    bb.putDouble(value.distanceMeters);
    Rotation2d.struct.pack(bb, value.angle);
  }
}
