// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.trajectory.proto;

import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.proto.Trajectory.ProtobufTrajectoryState;
import org.wpilib.math.trajectory.Trajectory;
import org.wpilib.util.protobuf.Protobuf;
import us.hebi.quickbuf.Descriptors.Descriptor;

public class TrajectoryStateProto implements Protobuf<Trajectory.State, ProtobufTrajectoryState> {
  @Override
  public Class<Trajectory.State> getTypeClass() {
    return Trajectory.State.class;
  }

  @Override
  public Descriptor getDescriptor() {
    return ProtobufTrajectoryState.getDescriptor();
  }

  @Override
  public ProtobufTrajectoryState createMessage() {
    return ProtobufTrajectoryState.newInstance();
  }

  @Override
  public Trajectory.State unpack(ProtobufTrajectoryState msg) {
    return new Trajectory.State(
        msg.getTime(),
        msg.getVelocity(),
        msg.getAcceleration(),
        Pose2d.proto.unpack(msg.getPose()),
        msg.getCurvature());
  }

  @Override
  public void pack(ProtobufTrajectoryState msg, Trajectory.State value) {
    msg.setTime(value.time);
    msg.setVelocity(value.velocity);
    msg.setAcceleration(value.acceleration);
    Pose2d.proto.pack(msg.getMutablePose(), value.pose);
    msg.setCurvature(value.curvature);
  }
}
