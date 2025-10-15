// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.geometry.struct;

import edu.wpi.first.math.geometry.Rotation3d;
import edu.wpi.first.math.geometry.Transform3d;
import edu.wpi.first.math.geometry.Translation3d;
import edu.wpi.first.util.struct.Struct;
import java.nio.ByteBuffer;

public class Transform3dStruct implements Struct<Transform3d> {
  @Override
  public Class<Transform3d> getTypeClass() {
    return Transform3d.class;
  }

  @Override
  public String getTypeName() {
    return "Transform3d";
  }

  @Override
  public int getSize() {
    return Translation3d.struct.getSize() + Rotation3d.struct.getSize();
  }

  @Override
  public String getSchema() {
    return "Translation3d translation;Rotation3d rotation";
  }

  @Override
  public Struct<?>[] getNested() {
    return new Struct<?>[] {Translation3d.struct, Rotation3d.struct};
  }

  @Override
  public Transform3d unpack(ByteBuffer bb) {
    Translation3d translation = Translation3d.struct.unpack(bb);
    Rotation3d rotation = Rotation3d.struct.unpack(bb);
    return new Transform3d(translation, rotation);
  }

  @Override
  public void pack(ByteBuffer bb, Transform3d value) {
    Translation3d.struct.pack(bb, value.getTranslation());
    Rotation3d.struct.pack(bb, value.getRotation());
  }

  @Override
  public boolean isImmutable() {
    return true;
  }
}
