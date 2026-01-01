// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.controller.proto;

import org.wpilib.math.controller.ArmFeedforward;
import org.wpilib.math.proto.ProtobufController.ProtobufArmFeedforward;
import org.wpilib.util.protobuf.Protobuf;
import us.hebi.quickbuf.Descriptors.Descriptor;

public class ArmFeedforwardProto implements Protobuf<ArmFeedforward, ProtobufArmFeedforward> {
  @Override
  public Class<ArmFeedforward> getTypeClass() {
    return ArmFeedforward.class;
  }

  @Override
  public Descriptor getDescriptor() {
    return ProtobufArmFeedforward.getDescriptor();
  }

  @Override
  public ProtobufArmFeedforward createMessage() {
    return ProtobufArmFeedforward.newInstance();
  }

  @Override
  public ArmFeedforward unpack(ProtobufArmFeedforward msg) {
    return new ArmFeedforward(msg.getKs(), msg.getKg(), msg.getKv(), msg.getKa(), msg.getDt());
  }

  @Override
  public void pack(ProtobufArmFeedforward msg, ArmFeedforward value) {
    msg.setKs(value.getKs());
    msg.setKg(value.getKg());
    msg.setKv(value.getKv());
    msg.setKa(value.getKa());
    msg.setDt(value.getDt());
  }
}
