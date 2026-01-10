// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.trajectory.struct;

import java.nio.ByteBuffer;
import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.kinematics.ChassisAccelerations;
import org.wpilib.math.kinematics.ChassisSpeeds;
import org.wpilib.math.trajectory.TrajectorySample;
import org.wpilib.util.struct.Struct;

public class TrajectorySampleStruct implements Struct<TrajectorySample> {
  @Override
  public Class<TrajectorySample> getTypeClass() {
    return TrajectorySample.class;
  }

  @Override
  public String getTypeName() {
    return "TrajectorySample.Base";
  }

  @Override
  public int getSize() {
    return kSizeDouble
        + Pose2d.struct.getSize()
        + ChassisSpeeds.struct.getSize()
        + ChassisAccelerations.struct.getSize();
  }

  @Override
  public String getSchema() {
    return "double timestamp;Pose2d pose;ChassisSpeeds vel;ChassisAccelerations accel";
  }

  @Override
  public TrajectorySample unpack(ByteBuffer bb) {
    double timestamp = bb.getDouble();
    Pose2d pose = Pose2d.struct.unpack(bb);
    ChassisSpeeds vel = ChassisSpeeds.struct.unpack(bb);
    ChassisAccelerations accel = ChassisAccelerations.struct.unpack(bb);
    return new TrajectorySample(timestamp, pose, vel, accel);
  }

  @Override
  public void pack(ByteBuffer bb, TrajectorySample value) {
    bb.putDouble(value.timestamp);
    Pose2d.struct.pack(bb, value.pose);
    ChassisSpeeds.struct.pack(bb, value.velocity);
    ChassisAccelerations.struct.pack(bb, value.acceleration);
  }
}
