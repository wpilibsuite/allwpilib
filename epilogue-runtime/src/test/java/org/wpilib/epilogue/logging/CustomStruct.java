// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.epilogue.logging;

import java.nio.ByteBuffer;
import org.wpilib.util.struct.Struct;
import org.wpilib.util.struct.StructSerializable;

public record CustomStruct(int x) implements StructSerializable {
  public static final Serializer struct = new Serializer();

  public static final class Serializer implements Struct<CustomStruct> {
    @Override
    public Class<CustomStruct> getTypeClass() {
      return CustomStruct.class;
    }

    @Override
    public String getTypeName() {
      return "CustomStruct";
    }

    @Override
    public int getSize() {
      return kSizeInt32;
    }

    @Override
    public String getSchema() {
      return "int32 x;";
    }

    @Override
    public CustomStruct unpack(ByteBuffer bb) {
      return new CustomStruct(bb.getInt());
    }

    @Override
    public void pack(ByteBuffer bb, CustomStruct value) {
      bb.putInt(value.x);
    }
  }
}
