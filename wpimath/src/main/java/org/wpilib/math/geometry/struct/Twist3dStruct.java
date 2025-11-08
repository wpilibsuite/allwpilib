// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.geometry.struct;

import edu.wpi.first.math.geometry.Twist3d;
import edu.wpi.first.util.struct.Struct;
import java.nio.ByteBuffer;

public class Twist3dStruct implements Struct<Twist3d> {
  @Override
  public Class<Twist3d> getTypeClass() {
    return Twist3d.class;
  }

  @Override
  public String getTypeName() {
    return "Twist3d";
  }

  @Override
  public int getSize() {
    return kSizeDouble * 6;
  }

  @Override
  public String getSchema() {
    return "double dx;double dy;double dz;double rx;double ry;double rz";
  }

  @Override
  public Twist3d unpack(ByteBuffer bb) {
    double dx = bb.getDouble();
    double dy = bb.getDouble();
    double dz = bb.getDouble();
    double rx = bb.getDouble();
    double ry = bb.getDouble();
    double rz = bb.getDouble();
    return new Twist3d(dx, dy, dz, rx, ry, rz);
  }

  @Override
  public void pack(ByteBuffer bb, Twist3d value) {
    bb.putDouble(value.dx);
    bb.putDouble(value.dy);
    bb.putDouble(value.dz);
    bb.putDouble(value.rx);
    bb.putDouble(value.ry);
    bb.putDouble(value.rz);
  }
}
