// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.spline.proto;

import org.wpilib.math.proto.Spline.ProtobufCubicHermiteSpline;
import org.wpilib.math.spline.CubicHermiteSpline;
import org.wpilib.util.protobuf.Protobuf;
import us.hebi.quickbuf.Descriptors.Descriptor;

public final class CubicHermiteSplineProto
    implements Protobuf<CubicHermiteSpline, ProtobufCubicHermiteSpline> {
  @Override
  public Class<CubicHermiteSpline> getTypeClass() {
    return CubicHermiteSpline.class;
  }

  @Override
  public Descriptor getDescriptor() {
    return ProtobufCubicHermiteSpline.getDescriptor();
  }

  @Override
  public ProtobufCubicHermiteSpline createMessage() {
    return ProtobufCubicHermiteSpline.newInstance();
  }

  @Override
  public CubicHermiteSpline unpack(ProtobufCubicHermiteSpline msg) {
    return new CubicHermiteSpline(
        Protobuf.unpackArray(msg.getXInitial()),
        Protobuf.unpackArray(msg.getXFinal()),
        Protobuf.unpackArray(msg.getYInitial()),
        Protobuf.unpackArray(msg.getYFinal()));
  }

  @Override
  public void pack(ProtobufCubicHermiteSpline msg, CubicHermiteSpline value) {
    Protobuf.packArray(msg.getMutableXInitial(), value.xInitialControlVector);
    Protobuf.packArray(msg.getMutableXFinal(), value.xFinalControlVector);
    Protobuf.packArray(msg.getMutableYInitial(), value.yInitialControlVector);
    Protobuf.packArray(msg.getMutableYFinal(), value.yFinalControlVector);
  }
}
