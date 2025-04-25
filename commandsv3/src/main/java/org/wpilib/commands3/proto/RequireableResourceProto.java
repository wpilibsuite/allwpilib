// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.commands3.proto;

import edu.wpi.first.util.protobuf.Protobuf;
import org.wpilib.commands3.RequireableResource;
import us.hebi.quickbuf.Descriptors;

public class RequireableResourceProto
    implements Protobuf<RequireableResource, ProtobufRequireableResource> {
  @Override
  public Class<RequireableResource> getTypeClass() {
    return RequireableResource.class;
  }

  @Override
  public Descriptors.Descriptor getDescriptor() {
    return ProtobufRequireableResource.getDescriptor();
  }

  @Override
  public ProtobufRequireableResource createMessage() {
    return ProtobufRequireableResource.newInstance();
  }

  @Override
  public RequireableResource unpack(ProtobufRequireableResource msg) {
    throw new UnsupportedOperationException("Deserialization not supported");
  }

  @Override
  public void pack(ProtobufRequireableResource msg, RequireableResource value) {
    msg.setName(value.getName());
  }
}
