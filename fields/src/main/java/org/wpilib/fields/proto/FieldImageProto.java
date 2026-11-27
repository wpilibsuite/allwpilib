// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.fields.proto;

import org.wpilib.fields.FieldImage;
import org.wpilib.fields.proto.detail.ProtobufFieldImage;
import org.wpilib.util.protobuf.Protobuf;
import us.hebi.quickbuf.Descriptors.Descriptor;

public class FieldImageProto implements Protobuf<FieldImage, ProtobufFieldImage> {
  @Override
  public Class<FieldImage> getTypeClass() {
    return FieldImage.class;
  }

  @Override
  public Descriptor getDescriptor() {
    return ProtobufFieldImage.getDescriptor();
  }

  @Override
  public ProtobufFieldImage createMessage() {
    return ProtobufFieldImage.newInstance();
  }

  @Override
  public FieldImage unpack(ProtobufFieldImage msg) {
    return new FieldImage(
        msg.getPath(), msg.getTop(), msg.getLeft(), msg.getBottom(), msg.getRight());
  }

  @Override
  public void pack(ProtobufFieldImage msg, FieldImage value) {
    msg.setPath(value.path);
    msg.setTop(value.top);
    msg.setLeft(value.left);
    msg.setBottom(value.bottom);
    msg.setRight(value.right);
  }
}
