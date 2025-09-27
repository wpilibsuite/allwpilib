// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.spline.struct;

import edu.wpi.first.math.spline.CubicHermiteSpline;
import edu.wpi.first.util.struct.Struct;
import java.nio.ByteBuffer;

public final class CubicHermiteSplineStruct implements Struct<CubicHermiteSpline> {
  @Override
  public Class<CubicHermiteSpline> getTypeClass() {
    return CubicHermiteSpline.class;
  }

  @Override
  public String getTypeName() {
    return "CubicHermiteSpline";
  }

  @Override
  public int getSize() {
    return kSizeDouble * 4 * 2;
  }

  @Override
  public String getSchema() {
    return "double xInitial[2];double xFinal[2];double yInitial[2];double yFinal[2]";
  }

  @Override
  public CubicHermiteSpline unpack(ByteBuffer bb) {
    return new CubicHermiteSpline(
        Struct.unpackDoubleArray(bb, 2),
        Struct.unpackDoubleArray(bb, 2),
        Struct.unpackDoubleArray(bb, 2),
        Struct.unpackDoubleArray(bb, 2));
  }

  @Override
  public void pack(ByteBuffer bb, CubicHermiteSpline value) {
    Struct.packArray(bb, value.xInitialControlVector);
    Struct.packArray(bb, value.xFinalControlVector);
    Struct.packArray(bb, value.yInitialControlVector);
    Struct.packArray(bb, value.yFinalControlVector);
  }
}
