// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.geometry.struct;

import edu.wpi.first.math.geometry.Twist2d;
import edu.wpi.first.util.struct.Struct;
import java.nio.ByteBuffer;

public class Twist2dStruct implements Struct<Twist2d> {
  @Override
  public Class<Twist2d> getTypeClass() {
    return Twist2d.class;
  }

  @Override
  public String getTypeName() {
    return "Twist2d";
  }

  @Override
  public int getSize() {
    return kSizeDouble * 3;
  }

  @Override
  public String getSchema() {
    return "double dx;double dy;double dtheta";
  }

  @Override
  public Twist2d unpack(ByteBuffer bb) {
    double dx = bb.getDouble();
    double dy = bb.getDouble();
    double dtheta = bb.getDouble();
    return new Twist2d(dx, dy, dtheta);
  }

  @Override
  public void pack(ByteBuffer bb, Twist2d value) {
    bb.putDouble(value.dx);
    bb.putDouble(value.dy);
    bb.putDouble(value.dtheta);
  }
}
