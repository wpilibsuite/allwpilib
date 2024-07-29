// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.kinematics.struct;

import edu.wpi.first.math.kinematics.ChassisSpeeds;
import edu.wpi.first.util.struct.Struct;
import java.nio.ByteBuffer;

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
    bb.putDouble(value.vxMetersPerSecond);
    bb.putDouble(value.vyMetersPerSecond);
    bb.putDouble(value.omegaRadiansPerSecond);
  }
}
