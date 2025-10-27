// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.geometry.struct;

import edu.wpi.first.math.geometry.Translation3d;
import edu.wpi.first.util.struct.Struct;
import java.nio.ByteBuffer;

public class Translation3dStruct implements Struct<Translation3d> {
  @Override
  public Class<Translation3d> getTypeClass() {
    return Translation3d.class;
  }

  @Override
  public String getTypeName() {
    return "Translation3d";
  }

  @Override
  public int getSize() {
    return kSizeDouble * 3;
  }

  @Override
  public String getSchema() {
    return "double x;double y;double z";
  }

  @Override
  public Translation3d unpack(ByteBuffer bb) {
    double x = bb.getDouble();
    double y = bb.getDouble();
    double z = bb.getDouble();
    return new Translation3d(x, y, z);
  }

  @Override
  public void pack(ByteBuffer bb, Translation3d value) {
    bb.putDouble(value.getX());
    bb.putDouble(value.getY());
    bb.putDouble(value.getZ());
  }

  @Override
  public boolean isImmutable() {
    return true;
  }
}
