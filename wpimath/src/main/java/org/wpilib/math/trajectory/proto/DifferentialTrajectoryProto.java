// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.trajectory.proto;

import java.util.ArrayList;
import java.util.List;
import org.wpilib.math.proto.Trajectory.ProtobufDifferentialTrajectory;
import org.wpilib.math.trajectory.DifferentialSample;
import org.wpilib.math.trajectory.DifferentialTrajectory;
import org.wpilib.util.protobuf.Protobuf;
import us.hebi.quickbuf.Descriptors.Descriptor;
import us.hebi.quickbuf.RepeatedMessage;

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
    List<DifferentialSample> samples = new ArrayList<>();
    RepeatedMessage<org.wpilib.math.proto.Trajectory.ProtobufDifferentialSample> samplesMsg =
        msg.getSamples();
    for (int i = 0; i < samplesMsg.length(); i++) {
      samples.add(DifferentialSample.proto.unpack(samplesMsg.get(i)));
    }
    return new DifferentialTrajectory(samples.toArray(DifferentialSample[]::new));
  }

  @Override
  public void pack(ProtobufDifferentialTrajectory msg, DifferentialTrajectory value) {
    RepeatedMessage<org.wpilib.math.proto.Trajectory.ProtobufDifferentialSample> samplesMsg =
        msg.getMutableSamples();
    for (DifferentialSample sample : value.getSamples()) {
      DifferentialSample.proto.pack(samplesMsg.next(), sample);
    }
  }
}
