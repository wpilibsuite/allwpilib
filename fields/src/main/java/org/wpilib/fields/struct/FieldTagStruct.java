// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.fields.struct;

import java.nio.ByteBuffer;
import org.wpilib.fields.FieldTag;
import org.wpilib.math.geometry.Pose3d;
import org.wpilib.util.struct.Struct;

public class FieldTagStruct implements Struct<FieldTag> {
  @Override
  public Class<FieldTag> getTypeClass() {
    return FieldTag.class;
  }

  @Override
  public String getTypeName() {
    return "FieldTag";
  }

  @Override
  public int getSize() {
    return INT32_SIZE + Pose3d.struct.getSize();
  }

  @Override
  public String getSchema() {
    return "int32 id;Pose3d pose";
  }

  @Override
  public Struct<?>[] getNested() {
    return new Struct<?>[] {Pose3d.struct};
  }

  @Override
  public FieldTag unpack(ByteBuffer bb) {
    int id = bb.getInt();
    Pose3d pose = Pose3d.struct.unpack(bb);
    return new FieldTag(id, pose);
  }

  @Override
  public void pack(ByteBuffer bb, FieldTag value) {
    bb.putInt(value.getID());
    Pose3d.struct.pack(bb, value.getPose());
  }
}
