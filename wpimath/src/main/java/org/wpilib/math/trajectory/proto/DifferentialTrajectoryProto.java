// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.trajectory.proto;

import org.wpilib.math.proto.ProtobufDifferentialTrajectory;
import org.wpilib.math.trajectory.DifferentialSample;
import org.wpilib.math.trajectory.DifferentialTrajectory;
import org.wpilib.util.protobuf.Protobuf;
import us.hebi.quickbuf.Descriptors.Descriptor;

public class DifferentialTrajectoryProto
    implements Protobuf<DifferentialTrajectory, ProtobufDifferentialTrajectory> {
  @Override
  public Class<DifferentialTrajectory> getTypeClass() {
    return DifferentialTrajectory.class;
  }

  @Override
  public Descriptor getDescriptor() {
    return ProtobufDifferentialTrajectory.getDescriptor();
  }

  @Override
  public ProtobufDifferentialTrajectory createMessage() {
    return ProtobufDifferentialTrajectory.newInstance();
  }

  @Override
  public DifferentialTrajectory unpack(ProtobufDifferentialTrajectory msg) {
    return new DifferentialTrajectory(
        Protobuf.unpackList(msg.getSamples(), DifferentialSample.proto));
  }

  @Override
  public void pack(ProtobufDifferentialTrajectory msg, DifferentialTrajectory value) {
    Protobuf.packList(msg.getMutableSamples(), value.getSamples(), DifferentialSample.proto);
  }
}
