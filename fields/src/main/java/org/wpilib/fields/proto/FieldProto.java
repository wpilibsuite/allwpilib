// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.fields.proto;

import org.wpilib.fields.Field;
import org.wpilib.fields.FieldImage;
import org.wpilib.fields.FieldTag;
import org.wpilib.fields.proto.detail.ProtobufField;
import org.wpilib.util.protobuf.Protobuf;
import us.hebi.quickbuf.Descriptors.Descriptor;

public class FieldProto implements Protobuf<Field, ProtobufField> {
  @Override
  public Class<Field> getTypeClass() {
    return Field.class;
  }

  @Override
  public Descriptor getDescriptor() {
    return ProtobufField.getDescriptor();
  }

  @Override
  public ProtobufField createMessage() {
    return ProtobufField.newInstance();
  }

  @Override
  public Field unpack(ProtobufField msg) {
    return new Field(
        msg.getName(),
        msg.getSeason(),
        msg.getGame(),
        msg.hasImage() ? FieldImage.proto.unpack(msg.getImage()) : null,
        msg.getFieldDimensions().getLength(),
        msg.getFieldDimensions().getWidth(),
        msg.getProgram(),
        msg.hasTags() ? Protobuf.unpackList(msg.getTags(), FieldTag.proto) : null);
  }

  @Override
  public void pack(ProtobufField msg, Field value) {
    msg.setName(value.name);
    msg.setSeason(value.season);
    msg.setGame(value.game);
    var fieldDimensions = msg.getMutableFieldDimensions();
    fieldDimensions.setLength(value.length);
    fieldDimensions.setWidth(value.width);
    msg.setProgram(value.program);
    if (value.hasImage()) {
      FieldImage.proto.pack(msg.getMutableImage(), value.fieldImage);
    }
    if (value.hasTags()) {
      Protobuf.packList(msg.getMutableTags(), value.getTags(), FieldTag.proto);
    }
  }
}
