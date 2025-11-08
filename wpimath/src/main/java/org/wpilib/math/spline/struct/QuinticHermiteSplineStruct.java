// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.spline.struct;

import edu.wpi.first.math.spline.QuinticHermiteSpline;
import edu.wpi.first.util.struct.Struct;
import java.nio.ByteBuffer;

public final class QuinticHermiteSplineStruct implements Struct<QuinticHermiteSpline> {
  @Override
  public Class<QuinticHermiteSpline> getTypeClass() {
    return QuinticHermiteSpline.class;
  }

  @Override
  public String getTypeName() {
    return "QuinticHermiteSpline";
  }

  @Override
  public int getSize() {
    return kSizeDouble * 4 * 3;
  }

  @Override
  public String getSchema() {
    return "double xInitial[3];double xFinal[3];double yInitial[3];double yFinal[3]";
  }

  @Override
  public QuinticHermiteSpline unpack(ByteBuffer bb) {
    return new QuinticHermiteSpline(
        Struct.unpackDoubleArray(bb, 3),
        Struct.unpackDoubleArray(bb, 3),
        Struct.unpackDoubleArray(bb, 3),
        Struct.unpackDoubleArray(bb, 3));
  }

  @Override
  public void pack(ByteBuffer bb, QuinticHermiteSpline value) {
    Struct.packArray(bb, value.xInitialControlVector);
    Struct.packArray(bb, value.xFinalControlVector);
    Struct.packArray(bb, value.yInitialControlVector);
    Struct.packArray(bb, value.yFinalControlVector);
  }
}
