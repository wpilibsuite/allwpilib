// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.trajectory.proto;

import java.util.ArrayList;
import java.util.List;
import org.wpilib.math.proto.Trajectory.ProtobufHolonomicTrajectory;
import org.wpilib.math.trajectory.HolonomicTrajectory;
import org.wpilib.math.trajectory.TrajectorySample;
import org.wpilib.util.protobuf.Protobuf;
import us.hebi.quickbuf.Descriptors.Descriptor;
import us.hebi.quickbuf.RepeatedMessage;

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
    List<TrajectorySample> samples = new ArrayList<>();
    RepeatedMessage<org.wpilib.math.proto.Trajectory.ProtobufTrajectorySample> samplesMsg =
        msg.getSamples();
    for (int i = 0; i < samplesMsg.length(); i++) {
      samples.add(TrajectorySample.proto.unpack(samplesMsg.get(i)));
    }
    return new HolonomicTrajectory(samples);
  }

  @Override
  public void pack(ProtobufHolonomicTrajectory msg, HolonomicTrajectory value) {
    RepeatedMessage<org.wpilib.math.proto.Trajectory.ProtobufTrajectorySample> samplesMsg =
        msg.getMutableSamples();
    for (TrajectorySample sample : value.getSamples()) {
      TrajectorySample.proto.pack(samplesMsg.next(), sample);
    }
  }
}
