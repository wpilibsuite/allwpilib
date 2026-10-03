// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.trajectory.struct;

import java.nio.ByteBuffer;
import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.kinematics.ChassisAccelerations;
import org.wpilib.math.kinematics.ChassisVelocities;
import org.wpilib.math.trajectory.HolonomicSample;
import org.wpilib.util.struct.Struct;

public class HolonomicSampleStruct implements Struct<HolonomicSample> {
  @Override
  public Class<HolonomicSample> getTypeClass() {
    return HolonomicSample.class;
  }

  @Override
  public String getTypeName() {
    return "HolonomicSample";
  }

  @Override
  public int getSize() {
    return DOUBLE_SIZE
        + Pose2d.struct.getSize()
        + ChassisVelocities.struct.getSize()
        + ChassisAccelerations.struct.getSize();
  }

  @Override
  public String getSchema() {
    return "double time;Pose2d pose;ChassisVelocities velocity;ChassisAccelerations acceleration";
  }

  @Override
  public Struct<?>[] getNested() {
    return new Struct<?>[] {Pose2d.struct, ChassisVelocities.struct, ChassisAccelerations.struct};
  }

  @Override
  public HolonomicSample unpack(ByteBuffer bb) {
    double time = bb.getDouble();
    Pose2d pose = Pose2d.struct.unpack(bb);
    ChassisVelocities vel = ChassisVelocities.struct.unpack(bb);
    ChassisAccelerations accel = ChassisAccelerations.struct.unpack(bb);
    return new HolonomicSample(time, pose, vel, accel);
  }

  @Override
  public void pack(ByteBuffer bb, HolonomicSample value) {
    bb.putDouble(value.time);
    Pose2d.struct.pack(bb, value.pose);
    ChassisVelocities.struct.pack(bb, value.velocity);
    ChassisAccelerations.struct.pack(bb, value.acceleration);
  }
}
