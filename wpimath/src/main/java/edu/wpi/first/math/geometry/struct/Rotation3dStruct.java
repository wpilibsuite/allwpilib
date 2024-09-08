// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.geometry.struct;

import edu.wpi.first.math.geometry.Quaternion;
import edu.wpi.first.math.geometry.Rotation3d;
import edu.wpi.first.util.struct.Struct;
import java.nio.ByteBuffer;

public class Rotation3dStruct implements Struct<Rotation3d> {
  @Override
  public Class<Rotation3d> getTypeClass() {
    return Rotation3d.class;
  }

  @Override
  public String getTypeName() {
    return "Rotation3d";
  }

  @Override
  public int getSize() {
    return Quaternion.struct.getSize();
  }

  @Override
  public String getSchema() {
    return "Quaternion q";
  }

  @Override
  public Struct<?>[] getNested() {
    return new Struct<?>[] {Quaternion.struct};
  }

  @Override
  public Rotation3d unpack(ByteBuffer bb) {
    Quaternion q = Quaternion.struct.unpack(bb);
    return new Rotation3d(q);
  }

  @Override
  public void pack(ByteBuffer bb, Rotation3d value) {
    Quaternion.struct.pack(bb, value.getQuaternion());
  }

  @Override
  public boolean isImmutable() {
    return true;
  }
}
