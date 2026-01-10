// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.geometry.proto;

import org.wpilib.math.geometry.Translation3d;
import org.wpilib.math.proto.ProtobufTranslation3d;
import org.wpilib.util.protobuf.Protobuf;
import us.hebi.quickbuf.Descriptors.Descriptor;

public class Translation3dProto implements Protobuf<Translation3d, ProtobufTranslation3d> {
  @Override
  public Class<Translation3d> getTypeClass() {
    return Translation3d.class;
  }

  @Override
  public Descriptor getDescriptor() {
    return ProtobufTranslation3d.getDescriptor();
  }

  @Override
  public ProtobufTranslation3d createMessage() {
    return ProtobufTranslation3d.newInstance();
  }

  @Override
  public Translation3d unpack(ProtobufTranslation3d msg) {
    return new Translation3d(msg.getX(), msg.getY(), msg.getZ());
  }

  @Override
  public void pack(ProtobufTranslation3d msg, Translation3d value) {
    msg.setX(value.getX());
    msg.setY(value.getY());
    msg.setZ(value.getZ());
  }

  @Override
  public boolean isImmutable() {
    return true;
  }
}
