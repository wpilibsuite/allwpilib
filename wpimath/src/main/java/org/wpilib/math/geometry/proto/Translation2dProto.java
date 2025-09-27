// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.geometry.proto;

import edu.wpi.first.math.geometry.Translation2d;
import edu.wpi.first.math.proto.Geometry2D.ProtobufTranslation2d;
import edu.wpi.first.util.protobuf.Protobuf;
import us.hebi.quickbuf.Descriptors.Descriptor;

public class Translation2dProto implements Protobuf<Translation2d, ProtobufTranslation2d> {
  @Override
  public Class<Translation2d> getTypeClass() {
    return Translation2d.class;
  }

  @Override
  public Descriptor getDescriptor() {
    return ProtobufTranslation2d.getDescriptor();
  }

  @Override
  public ProtobufTranslation2d createMessage() {
    return ProtobufTranslation2d.newInstance();
  }

  @Override
  public Translation2d unpack(ProtobufTranslation2d msg) {
    return new Translation2d(msg.getX(), msg.getY());
  }

  @Override
  public void pack(ProtobufTranslation2d msg, Translation2d value) {
    msg.setX(value.getX());
    msg.setY(value.getY());
  }

  @Override
  public boolean isImmutable() {
    return true;
  }
}
