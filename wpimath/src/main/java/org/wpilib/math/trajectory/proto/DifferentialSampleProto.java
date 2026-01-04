// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.trajectory.proto;

import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.kinematics.ChassisAccelerations;
import org.wpilib.math.kinematics.ChassisSpeeds;
import org.wpilib.math.proto.Trajectory.ProtobufDifferentialSample;
import org.wpilib.math.trajectory.DifferentialSample;
import org.wpilib.units.Units;
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
        msg.getTimestamp(),
        Pose2d.proto.unpack(msg.getPose()),
        ChassisSpeeds.proto.unpack(msg.getVelocities()),
        ChassisAccelerations.proto.unpack(msg.getAccelerations()),
        msg.getLeftVelocity(),
        msg.getRightVelocity());
  }

  @Override
  public void pack(ProtobufDifferentialSample msg, DifferentialSample value) {
    msg.setTimestamp(value.timestamp);
    Pose2d.proto.pack(msg.getMutablePose(), value.pose);
    ChassisSpeeds.proto.pack(msg.getMutableVelocities(), value.velocity);
    ChassisAccelerations.proto.pack(msg.getMutableAccelerations(), value.acceleration);
    msg.setLeftVelocity(value.leftSpeed);
    msg.setRightVelocity(value.rightSpeed);
  }
}
