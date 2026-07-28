// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.trajectory.proto;

import org.wpilib.math.proto.ProtobufHolonomicTrajectory;
import org.wpilib.math.trajectory.HolonomicSample;
import org.wpilib.math.trajectory.HolonomicTrajectory;
import org.wpilib.util.protobuf.Protobuf;
import us.hebi.quickbuf.Descriptors.Descriptor;

public class HolonomicTrajectoryProto
    implements Protobuf<HolonomicTrajectory, ProtobufHolonomicTrajectory> {
  @Override
  public Class<HolonomicTrajectory> getTypeClass() {
    return HolonomicTrajectory.class;
  }

  @Override
  public Descriptor getDescriptor() {
    return ProtobufHolonomicTrajectory.getDescriptor();
  }

  @Override
  public ProtobufHolonomicTrajectory createMessage() {
    return ProtobufHolonomicTrajectory.newInstance();
  }

  @Override
  public HolonomicTrajectory unpack(ProtobufHolonomicTrajectory msg) {
    return new HolonomicTrajectory(Protobuf.unpackList(msg.getSamples(), HolonomicSample.proto));
  }

  @Override
  public void pack(ProtobufHolonomicTrajectory msg, HolonomicTrajectory value) {
    Protobuf.packList(msg.getMutableSamples(), value.getSamples(), HolonomicSample.proto);
  }
}
