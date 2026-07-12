// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.trajectory.proto;

import org.wpilib.math.proto.ProtobufTrajectorySample;
import org.wpilib.math.trajectory.TrajectorySample;
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
    return new TrajectorySample(msg.getTime());
  }

  @Override
  public void pack(ProtobufTrajectorySample msg, TrajectorySample value) {
    msg.setTime(value.time);
  }
}
