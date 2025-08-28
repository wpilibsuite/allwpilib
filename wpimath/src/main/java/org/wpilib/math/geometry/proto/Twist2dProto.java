// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.geometry.proto;

import org.wpilib.math.geometry.Twist2d;
import org.wpilib.math.proto.Geometry2D.ProtobufTwist2d;
import edu.wpi.first.util.protobuf.Protobuf;
import us.hebi.quickbuf.Descriptors.Descriptor;

public class Twist2dProto implements Protobuf<Twist2d, ProtobufTwist2d> {
  @Override
  public Class<Twist2d> getTypeClass() {
    return Twist2d.class;
  }

  @Override
  public Descriptor getDescriptor() {
    return ProtobufTwist2d.getDescriptor();
  }

  @Override
  public ProtobufTwist2d createMessage() {
    return ProtobufTwist2d.newInstance();
  }

  @Override
  public Twist2d unpack(ProtobufTwist2d msg) {
    return new Twist2d(msg.getDx(), msg.getDy(), msg.getDtheta());
  }

  @Override
  public void pack(ProtobufTwist2d msg, Twist2d value) {
    msg.setDx(value.dx);
    msg.setDy(value.dy);
    msg.setDtheta(value.dtheta);
  }
}
