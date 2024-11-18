// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics.struct;

import org.wpilib.math.kinematics.DifferentialDriveKinematics;
import org.wpilib.util.struct.Struct;
import java.nio.ByteBuffer;

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
    return "double track_width";
  }

  @Override
  public DifferentialDriveKinematics unpack(ByteBuffer bb) {
    double trackWidth = bb.getDouble();
    return new DifferentialDriveKinematics(trackWidth);
  }

  @Override
  public void pack(ByteBuffer bb, DifferentialDriveKinematics value) {
    bb.putDouble(value.trackWidthMeters);
  }
}
