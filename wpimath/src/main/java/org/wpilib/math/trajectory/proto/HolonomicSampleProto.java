// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.trajectory.proto;

import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.kinematics.ChassisAccelerations;
import org.wpilib.math.kinematics.ChassisVelocities;
import org.wpilib.math.proto.ProtobufHolonomicSample;
import org.wpilib.math.trajectory.HolonomicSample;
import org.wpilib.util.protobuf.Protobuf;
import us.hebi.quickbuf.Descriptors.Descriptor;

public class HolonomicSampleProto implements Protobuf<HolonomicSample, ProtobufHolonomicSample> {
  @Override
  public Class<HolonomicSample> getTypeClass() {
    return HolonomicSample.class;
  }

  @Override
  public Descriptor getDescriptor() {
    return ProtobufHolonomicSample.getDescriptor();
  }

  @Override
  public ProtobufHolonomicSample createMessage() {
    return ProtobufHolonomicSample.newInstance();
  }

  @Override
  public HolonomicSample unpack(ProtobufHolonomicSample msg) {
    return new HolonomicSample(
        msg.getTime(),
        Pose2d.proto.unpack(msg.getPose()),
        ChassisVelocities.proto.unpack(msg.getVelocities()),
        ChassisAccelerations.proto.unpack(msg.getAccelerations()));
  }

  @Override
  public void pack(ProtobufHolonomicSample msg, HolonomicSample value) {
    msg.setTime(value.time);
    Pose2d.proto.pack(msg.getMutablePose(), value.pose);
    ChassisVelocities.proto.pack(msg.getMutableVelocities(), value.velocity);
    ChassisAccelerations.proto.pack(msg.getMutableAccelerations(), value.acceleration);
  }
}
