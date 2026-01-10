// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.geometry.proto;

import org.wpilib.math.geometry.Twist3d;
import org.wpilib.math.proto.ProtobufTwist3d;
import org.wpilib.util.protobuf.Protobuf;
import us.hebi.quickbuf.Descriptors.Descriptor;

public class Twist3dProto implements Protobuf<Twist3d, ProtobufTwist3d> {
  @Override
  public Class<Twist3d> getTypeClass() {
    return Twist3d.class;
  }

  @Override
  public Descriptor getDescriptor() {
    return ProtobufTwist3d.getDescriptor();
  }

  @Override
  public ProtobufTwist3d createMessage() {
    return ProtobufTwist3d.newInstance();
  }

  @Override
  public Twist3d unpack(ProtobufTwist3d msg) {
    return new Twist3d(
        msg.getDx(), msg.getDy(), msg.getDz(), msg.getRx(), msg.getRy(), msg.getRz());
  }

  @Override
  public void pack(ProtobufTwist3d msg, Twist3d value) {
    msg.setDx(value.dx);
    msg.setDy(value.dy);
    msg.setDz(value.dz);
    msg.setRx(value.rx);
    msg.setRy(value.ry);
    msg.setRz(value.rz);
  }
}
