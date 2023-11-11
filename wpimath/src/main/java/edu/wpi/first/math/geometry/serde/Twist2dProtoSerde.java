// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.geometry.serde;

import edu.wpi.first.math.geometry.Twist2d;
import edu.wpi.first.math.proto.Geometry2D.ProtobufTwist2d;
import edu.wpi.first.util.protobuf.Protobuf;
import us.hebi.quickbuf.Descriptors.Descriptor;

public class Twist2dProtoSerde implements Protobuf<Twist2d, ProtobufTwist2d> {
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
    return new Twist2d(msg.getDxMeters(), msg.getDyMeters(), msg.getDthetaRadians());
  }

  @Override
  public void pack(ProtobufTwist2d msg, Twist2d value) {
    msg.setDxMeters(value.dx);
    msg.setDyMeters(value.dy);
    msg.setDthetaRadians(value.dtheta);
  }
}
