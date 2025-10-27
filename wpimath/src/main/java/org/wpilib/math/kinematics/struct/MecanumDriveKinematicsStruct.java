// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.kinematics.struct;

import edu.wpi.first.math.geometry.Translation2d;
import edu.wpi.first.math.kinematics.MecanumDriveKinematics;
import edu.wpi.first.util.struct.Struct;
import java.nio.ByteBuffer;

public class MecanumDriveKinematicsStruct implements Struct<MecanumDriveKinematics> {
  @Override
  public Class<MecanumDriveKinematics> getTypeClass() {
    return MecanumDriveKinematics.class;
  }

  @Override
  public String getTypeName() {
    return "MecanumDriveKinematics";
  }

  @Override
  public int getSize() {
    return 4 * Translation2d.struct.getSize();
  }

  @Override
  public String getSchema() {
    return "Translation2d front_left;Translation2d front_right;Translation2d rear_left;"
        + "Translation2d rear_right";
  }

  @Override
  public Struct<?>[] getNested() {
    return new Struct<?>[] {Translation2d.struct};
  }

  @Override
  public MecanumDriveKinematics unpack(ByteBuffer bb) {
    Translation2d frontLeft = Translation2d.struct.unpack(bb);
    Translation2d frontRight = Translation2d.struct.unpack(bb);
    Translation2d rearLeft = Translation2d.struct.unpack(bb);
    Translation2d rearRight = Translation2d.struct.unpack(bb);
    return new MecanumDriveKinematics(frontLeft, frontRight, rearLeft, rearRight);
  }

  @Override
  public void pack(ByteBuffer bb, MecanumDriveKinematics value) {
    Translation2d.struct.pack(bb, value.getFrontLeft());
    Translation2d.struct.pack(bb, value.getFrontRight());
    Translation2d.struct.pack(bb, value.getRearLeft());
    Translation2d.struct.pack(bb, value.getRearRight());
  }
}
