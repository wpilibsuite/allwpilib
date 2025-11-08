// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.geometry.struct;

import edu.wpi.first.math.geometry.Ellipse2d;
import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.util.struct.Struct;
import java.nio.ByteBuffer;

public class Ellipse2dStruct implements Struct<Ellipse2d> {
  @Override
  public Class<Ellipse2d> getTypeClass() {
    return Ellipse2d.class;
  }

  @Override
  public String getTypeName() {
    return "Ellipse2d";
  }

  @Override
  public int getSize() {
    return Pose2d.struct.getSize() + kSizeDouble + kSizeDouble;
  }

  @Override
  public String getSchema() {
    return "Pose2d center;double xSemiAxis; double ySemiAxis";
  }

  @Override
  public Struct<?>[] getNested() {
    return new Struct<?>[] {Pose2d.struct};
  }

  @Override
  public Ellipse2d unpack(ByteBuffer bb) {
    Pose2d center = Pose2d.struct.unpack(bb);
    double xSemiAxis = bb.getDouble();
    double ySemiAxis = bb.getDouble();
    return new Ellipse2d(center, xSemiAxis, ySemiAxis);
  }

  @Override
  public void pack(ByteBuffer bb, Ellipse2d value) {
    Pose2d.struct.pack(bb, value.getCenter());
    bb.putDouble(value.getXSemiAxis());
    bb.putDouble(value.getYSemiAxis());
  }
}
