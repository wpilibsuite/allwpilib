// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.trajectory.struct;

import java.nio.ByteBuffer;
import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.kinematics.ChassisAccelerations;
import org.wpilib.math.kinematics.ChassisVelocities;
import org.wpilib.math.trajectory.DifferentialSample;
import org.wpilib.math.trajectory.HolonomicSample;
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
    return HolonomicSample.struct.getSize() + DOUBLE_SIZE + DOUBLE_SIZE;
  }

  @Override
  public String getSchema() {
    return HolonomicSample.struct.getSchema() + ";double leftVelocity;double rightVelocity";
  }

  @Override
  public Struct<?>[] getNested() {
    return new Struct<?>[] {HolonomicSample.struct};
  }

  @Override
  public DifferentialSample unpack(ByteBuffer bb) {
    double time = bb.getDouble();
    Pose2d pose = Pose2d.struct.unpack(bb);
    ChassisVelocities vel = ChassisVelocities.struct.unpack(bb);
    ChassisAccelerations accel = ChassisAccelerations.struct.unpack(bb);
    double leftVelocity = bb.getDouble();
    double rightVelocity = bb.getDouble();
    return new DifferentialSample(time, pose, vel, accel, leftVelocity, rightVelocity);
  }

  @Override
  public void pack(ByteBuffer bb, DifferentialSample value) {
    bb.putDouble(value.time);
    Pose2d.struct.pack(bb, value.pose);
    ChassisVelocities.struct.pack(bb, value.velocity);
    ChassisAccelerations.struct.pack(bb, value.acceleration);
    bb.putDouble(value.leftVelocity);
    bb.putDouble(value.rightVelocity);
  }
}
