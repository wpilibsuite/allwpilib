// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics.struct;

import java.nio.ByteBuffer;
import org.wpilib.math.kinematics.ChassisSpeeds;
import org.wpilib.util.struct.Struct;

public class ChassisSpeedsStruct implements Struct<ChassisSpeeds> {
  @Override
  public Class<ChassisSpeeds> getTypeClass() {
    return ChassisSpeeds.class;
  }

  @Override
  public String getTypeName() {
    return "ChassisSpeeds";
  }

  @Override
  public int getSize() {
    return kSizeDouble * 3;
  }

  @Override
  public String getSchema() {
    return "double vx;double vy;double omega";
  }

  @Override
  public ChassisSpeeds unpack(ByteBuffer bb) {
    double vx = bb.getDouble();
    double vy = bb.getDouble();
    double omega = bb.getDouble();
    return new ChassisSpeeds(vx, vy, omega);
  }

  @Override
  public void pack(ByteBuffer bb, ChassisSpeeds value) {
    bb.putDouble(value.vx);
    bb.putDouble(value.vy);
    bb.putDouble(value.omega);
  }
}
