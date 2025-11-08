// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.geometry.struct;

import edu.wpi.first.math.geometry.Quaternion;
import edu.wpi.first.util.struct.Struct;
import java.nio.ByteBuffer;

public class QuaternionStruct implements Struct<Quaternion> {
  @Override
  public Class<Quaternion> getTypeClass() {
    return Quaternion.class;
  }

  @Override
  public String getTypeName() {
    return "Quaternion";
  }

  @Override
  public int getSize() {
    return kSizeDouble * 4;
  }

  @Override
  public String getSchema() {
    return "double w;double x;double y;double z";
  }

  @Override
  public Quaternion unpack(ByteBuffer bb) {
    double w = bb.getDouble();
    double x = bb.getDouble();
    double y = bb.getDouble();
    double z = bb.getDouble();
    return new Quaternion(w, x, y, z);
  }

  @Override
  public void pack(ByteBuffer bb, Quaternion value) {
    bb.putDouble(value.getW());
    bb.putDouble(value.getX());
    bb.putDouble(value.getY());
    bb.putDouble(value.getZ());
  }

  @Override
  public boolean isImmutable() {
    return true;
  }
}
