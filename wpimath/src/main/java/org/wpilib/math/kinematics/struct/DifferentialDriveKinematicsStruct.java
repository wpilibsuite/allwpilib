// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics.struct;

import java.nio.ByteBuffer;
import org.wpilib.math.kinematics.DifferentialDriveKinematics;
import org.wpilib.util.struct.Struct;

public class DifferentialDriveKinematicsStruct implements Struct<DifferentialDriveKinematics> {
  @Override
  public Class<DifferentialDriveKinematics> getTypeClass() {
    return DifferentialDriveKinematics.class;
  }

  @Override
  public String getTypeName() {
    return "DifferentialDriveKinematics";
  }

  @Override
  public int getSize() {
    return kSizeDouble;
  }

  @Override
  public String getSchema() {
    return "double trackwidth";
  }

  @Override
  public DifferentialDriveKinematics unpack(ByteBuffer bb) {
    double trackwidth = bb.getDouble();
    return new DifferentialDriveKinematics(trackwidth);
  }

  @Override
  public void pack(ByteBuffer bb, DifferentialDriveKinematics value) {
    bb.putDouble(value.trackwidth);
  }
}
