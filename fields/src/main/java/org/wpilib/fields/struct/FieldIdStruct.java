// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.fields.struct;

import java.nio.ByteBuffer;
import org.wpilib.fields.Fields;
import org.wpilib.util.struct.Struct;

public class FieldIdStruct implements Struct<Fields> {
  @Override
  public Class<Fields> getTypeClass() {
    return Fields.class;
  }

  @Override
  public String getTypeName() {
    return "FieldId";
  }

  @Override
  public int getSize() {
    return INT32_SIZE;
  }

  @Override
  public String getSchema() {
    return "int32 id";
  }

  @Override
  public Fields unpack(ByteBuffer bb) {
    int id = bb.getInt();
    return Fields.values()[id];
  }

  @Override
  public void pack(ByteBuffer bb, Fields value) {
    bb.putInt(value.ordinal());
  }
}
