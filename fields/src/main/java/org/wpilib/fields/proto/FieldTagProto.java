// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.fields.proto;

import org.wpilib.fields.FieldTag;
import org.wpilib.fields.proto.detail.ProtobufFieldTag;
import org.wpilib.math.geometry.Pose3d;
import org.wpilib.util.protobuf.Protobuf;
import us.hebi.quickbuf.Descriptors.Descriptor;

public class FieldTagProto implements Protobuf<FieldTag, ProtobufFieldTag> {
  @Override
  public Class<FieldTag> getTypeClass() {
    return FieldTag.class;
  }

  @Override
  public Descriptor getDescriptor() {
    return ProtobufFieldTag.getDescriptor();
  }

  @Override
  public ProtobufFieldTag createMessage() {
    return ProtobufFieldTag.newInstance();
  }

  @Override
  public FieldTag unpack(ProtobufFieldTag msg) {
    return new FieldTag(msg.getId(), Pose3d.proto.unpack(msg.getPose()));
  }

  @Override
  public void pack(ProtobufFieldTag msg, FieldTag value) {
    msg.setId(value.getID());
    Pose3d.proto.pack(msg.getMutablePose(), value.getPose());
  }
}
