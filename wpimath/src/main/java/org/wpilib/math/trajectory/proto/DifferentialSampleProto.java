// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.trajectory.proto;

import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.kinematics.ChassisAccelerations;
import org.wpilib.math.kinematics.ChassisVelocities;
import org.wpilib.math.proto.ProtobufDifferentialSample;
import org.wpilib.math.trajectory.DifferentialSample;
import org.wpilib.util.protobuf.Protobuf;
import us.hebi.quickbuf.Descriptors.Descriptor;

public class DifferentialSampleProto
    implements Protobuf<DifferentialSample, ProtobufDifferentialSample> {
  @Override
  public Class<DifferentialSample> getTypeClass() {
    return DifferentialSample.class;
  }

  @Override
  public Descriptor getDescriptor() {
    return ProtobufDifferentialSample.getDescriptor();
  }

  @Override
  public ProtobufDifferentialSample createMessage() {
    return ProtobufDifferentialSample.newInstance();
  }

  @Override
  public DifferentialSample unpack(ProtobufDifferentialSample msg) {
    return new DifferentialSample(
        msg.getTime(),
        Pose2d.proto.unpack(msg.getPose()),
        ChassisVelocities.proto.unpack(msg.getVelocities()),
        ChassisAccelerations.proto.unpack(msg.getAccelerations()),
        msg.getLeftVelocity(),
        msg.getRightVelocity());
  }

  @Override
  public void pack(ProtobufDifferentialSample msg, DifferentialSample value) {
    msg.setTime(value.time);
    Pose2d.proto.pack(msg.getMutablePose(), value.pose);
    ChassisVelocities.proto.pack(msg.getMutableVelocities(), value.velocity);
    ChassisAccelerations.proto.pack(msg.getMutableAccelerations(), value.acceleration);
    msg.setLeftVelocity(value.leftVelocity);
    msg.setRightVelocity(value.rightVelocity);
  }
}
