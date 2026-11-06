// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.fields.proto;

import org.wpilib.fields.Fields;
import org.wpilib.fields.proto.detail.ProtobufFieldId;
import org.wpilib.util.protobuf.Protobuf;
import us.hebi.quickbuf.Descriptors.Descriptor;

public class FieldIdProto implements Protobuf<Fields, ProtobufFieldId> {
  @Override
  public Class<Fields> getTypeClass() {
    return Fields.class;
  }

  @Override
  public Descriptor getDescriptor() {
    return ProtobufFieldId.getDescriptor();
  }

  @Override
  public ProtobufFieldId createMessage() {
    return ProtobufFieldId.newInstance();
  }

  @Override
  public Fields unpack(ProtobufFieldId msg) {
    return Fields.values()[msg.getId()];
  }

  @Override
  public void pack(ProtobufFieldId msg, Fields value) {
    msg.setId(value.ordinal());
  }
}
