// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.geometry.proto;

import edu.wpi.first.math.geometry.Ellipse2d;
import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.proto.Geometry2D.ProtobufEllipse2d;
import edu.wpi.first.util.protobuf.Protobuf;
import us.hebi.quickbuf.Descriptors.Descriptor;

public class Ellipse2dProto implements Protobuf<Ellipse2d, ProtobufEllipse2d> {
  @Override
  public Class<Ellipse2d> getTypeClass() {
    return Ellipse2d.class;
  }

  @Override
  public Descriptor getDescriptor() {
    return ProtobufEllipse2d.getDescriptor();
  }

  @Override
  public ProtobufEllipse2d createMessage() {
    return ProtobufEllipse2d.newInstance();
  }

  @Override
  public Ellipse2d unpack(ProtobufEllipse2d msg) {
    return new Ellipse2d(
        Pose2d.proto.unpack(msg.getCenter()), msg.getXSemiAxis(), msg.getYSemiAxis());
  }

  @Override
  public void pack(ProtobufEllipse2d msg, Ellipse2d value) {
    Pose2d.proto.pack(msg.getMutableCenter(), value.getCenter());
    msg.setXSemiAxis(value.getXSemiAxis());
    msg.setYSemiAxis(value.getYSemiAxis());
  }
}
