// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.trajectory.struct;

import static org.wpilib.units.Units.Seconds;

import java.nio.ByteBuffer;
import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.kinematics.ChassisAccelerations;
import org.wpilib.math.kinematics.ChassisSpeeds;
import org.wpilib.math.trajectory.DifferentialSample;
import org.wpilib.math.trajectory.TrajectorySample;
import org.wpilib.util.struct.Struct;

public class DifferentialSampleStruct implements Struct<DifferentialSample> {
  @Override
  public Class<DifferentialSample> getTypeClass() {
    return DifferentialSample.class;
  }

  @Override
  public String getTypeName() {
    return "DifferentialSample";
  }

  @Override
  public int getSize() {
    return TrajectorySample.struct.getSize() + kSizeDouble + kSizeDouble;
  }

  @Override
  public String getSchema() {
    return TrajectorySample.struct.getSchema() + ";double leftSpeed;double rightSpeed";
  }

  @Override
  public DifferentialSample unpack(ByteBuffer bb) {
    TrajectorySample.Base base = TrajectorySample.struct.unpack(bb);
    double leftSpeed = bb.getDouble();
    double rightSpeed = bb.getDouble();
    return new DifferentialSample(base, leftSpeed, rightSpeed);
  }

  @Override
  public void pack(ByteBuffer bb, DifferentialSample value) {
    bb.putDouble(value.timestamp.in(Seconds));
    Pose2d.struct.pack(bb, value.pose);
    ChassisSpeeds.struct.pack(bb, value.velocity);
    ChassisAccelerations.struct.pack(bb, value.acceleration);
    bb.putDouble(value.leftSpeed);
    bb.putDouble(value.rightSpeed);
  }
}
