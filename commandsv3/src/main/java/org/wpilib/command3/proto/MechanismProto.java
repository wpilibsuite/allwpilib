// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command3.proto;

import org.wpilib.command3.Mechanism;
import org.wpilib.command3.proto.ProtobufCommands.ProtobufMechanism;
import org.wpilib.util.protobuf.Protobuf;
import us.hebi.quickbuf.Descriptors;

public class MechanismProto implements Protobuf<Mechanism, ProtobufMechanism> {
  @Override
  public Class<Mechanism> getTypeClass() {
    return Mechanism.class;
  }

  @Override
  public Descriptors.Descriptor getDescriptor() {
    return ProtobufMechanism.getDescriptor();
  }

  @Override
  public ProtobufMechanism createMessage() {
    return ProtobufMechanism.newInstance();
  }

  @Override
  public Mechanism unpack(ProtobufMechanism msg) {
    throw new UnsupportedOperationException("Deserialization not supported");
  }

  @Override
  public void pack(ProtobufMechanism msg, Mechanism value) {
    msg.clear();
    msg.setName(value.getName());
  }
}
