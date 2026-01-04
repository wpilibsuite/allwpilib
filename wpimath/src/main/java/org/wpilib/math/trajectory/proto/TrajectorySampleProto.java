// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.trajectory.proto;

import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.kinematics.ChassisAccelerations;
import org.wpilib.math.kinematics.ChassisSpeeds;
import org.wpilib.math.proto.Trajectory.ProtobufTrajectorySample;
import org.wpilib.math.trajectory.TrajectorySample;
import org.wpilib.units.Units;
import org.wpilib.util.protobuf.Protobuf;
import us.hebi.quickbuf.Descriptors.Descriptor;

public class TrajectorySampleProto implements Protobuf<TrajectorySample, ProtobufTrajectorySample> {
  @Override
  public Class<TrajectorySample> getTypeClass() {
    return TrajectorySample.class;
  }

  @Override
  public Descriptor getDescriptor() {
    return ProtobufTrajectorySample.getDescriptor();
  }

  @Override
  public ProtobufTrajectorySample createMessage() {
    return ProtobufTrajectorySample.newInstance();
  }

  @Override
  public TrajectorySample unpack(ProtobufTrajectorySample msg) {
    return new TrajectorySample(
        Units.Seconds.of(msg.getTimestamp()),
        Pose2d.proto.unpack(msg.getPose()),
        ChassisSpeeds.proto.unpack(msg.getVelocities()),
        ChassisAccelerations.proto.unpack(msg.getAccelerations()));
  }

  @Override
  public void pack(ProtobufTrajectorySample msg, TrajectorySample value) {
    msg.setTimestamp(value.timestamp.in(Units.Seconds));
    Pose2d.proto.pack(msg.getMutablePose(), value.pose);
    ChassisSpeeds.proto.pack(msg.getMutableVelocities(), value.velocity);
    ChassisAccelerations.proto.pack(msg.getMutableAccelerations(), value.acceleration);
  }
}
