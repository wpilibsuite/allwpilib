// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.spline.proto;

import edu.wpi.first.math.proto.Spline.ProtobufQuinticHermiteSpline;
import edu.wpi.first.math.spline.QuinticHermiteSpline;
import edu.wpi.first.util.protobuf.Protobuf;
import us.hebi.quickbuf.Descriptors.Descriptor;

public final class QuinticHermiteSplineProto
    implements Protobuf<QuinticHermiteSpline, ProtobufQuinticHermiteSpline> {
  @Override
  public Class<QuinticHermiteSpline> getTypeClass() {
    return QuinticHermiteSpline.class;
  }

  @Override
  public Descriptor getDescriptor() {
    return ProtobufQuinticHermiteSpline.getDescriptor();
  }

  @Override
  public ProtobufQuinticHermiteSpline createMessage() {
    return ProtobufQuinticHermiteSpline.newInstance();
  }

  @Override
  public QuinticHermiteSpline unpack(ProtobufQuinticHermiteSpline msg) {
    return new QuinticHermiteSpline(
        Protobuf.unpackArray(msg.getXInitial()),
        Protobuf.unpackArray(msg.getXFinal()),
        Protobuf.unpackArray(msg.getYInitial()),
        Protobuf.unpackArray(msg.getYFinal()));
  }

  @Override
  public void pack(ProtobufQuinticHermiteSpline msg, QuinticHermiteSpline value) {
    Protobuf.packArray(msg.getMutableXInitial(), value.xInitialControlVector);
    Protobuf.packArray(msg.getMutableXFinal(), value.xFinalControlVector);
    Protobuf.packArray(msg.getMutableYInitial(), value.yInitialControlVector);
    Protobuf.packArray(msg.getMutableYFinal(), value.yFinalControlVector);
  }
}
