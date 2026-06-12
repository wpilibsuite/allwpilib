// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.trajectory.proto;

import org.wpilib.math.proto.ProtobufTrajectory;
import org.wpilib.math.trajectory.Trajectory;
import org.wpilib.util.protobuf.Protobuf;
import us.hebi.quickbuf.Descriptors.Descriptor;

public class TrajectoryProto implements Protobuf<Trajectory, ProtobufTrajectory> {
  @Override
  public Class<Trajectory> getTypeClass() {
    return Trajectory.class;
  }

  @Override
  public Descriptor getDescriptor() {
    return ProtobufTrajectory.getDescriptor();
  }

  @Override
  public ProtobufTrajectory createMessage() {
    return ProtobufTrajectory.newInstance();
  }

  @Override
  public Trajectory unpack(ProtobufTrajectory msg) {
    return new Trajectory(Protobuf.unpackList(msg.getStates(), Trajectory.State.proto));
  }

  @Override
  public void pack(ProtobufTrajectory msg, Trajectory value) {
    Protobuf.packList(msg.getMutableStates(), value.getStates(), Trajectory.State.proto);
  }
}
