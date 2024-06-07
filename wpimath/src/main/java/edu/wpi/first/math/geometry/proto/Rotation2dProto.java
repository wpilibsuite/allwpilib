// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.geometry.proto;

import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.proto.Geometry2D.ProtobufRotation2d;
import edu.wpi.first.util.protobuf.Protobuf;
import us.hebi.quickbuf.Descriptors.Descriptor;

public class Rotation2dProto implements Protobuf<Rotation2d, ProtobufRotation2d> {
  @Override
  public Class<Rotation2d> getTypeClass() {
    return Rotation2d.class;
  }

  @Override
  public Descriptor getDescriptor() {
    return ProtobufRotation2d.getDescriptor();
  }

  @Override
  public ProtobufRotation2d createMessage() {
    return ProtobufRotation2d.newInstance();
  }

  @Override
  public Rotation2d unpack(ProtobufRotation2d msg) {
    return new Rotation2d(msg.getValue());
  }

  @Override
  public void pack(ProtobufRotation2d msg, Rotation2d value) {
    msg.setValue(value.getRadians());
  }

  @Override
  public boolean isImmutable() {
    return true;
  }
}
