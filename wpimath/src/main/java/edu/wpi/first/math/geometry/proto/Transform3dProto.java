// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.geometry.proto;

import edu.wpi.first.math.geometry.Rotation3d;
import edu.wpi.first.math.geometry.Transform3d;
import edu.wpi.first.math.geometry.Translation3d;
import edu.wpi.first.math.proto.Geometry3D.ProtobufTransform3d;
import edu.wpi.first.util.protobuf.Protobuf;
import us.hebi.quickbuf.Descriptors.Descriptor;

public class Transform3dProto implements Protobuf<Transform3d, ProtobufTransform3d> {
  @Override
  public Class<Transform3d> getTypeClass() {
    return Transform3d.class;
  }

  @Override
  public Descriptor getDescriptor() {
    return ProtobufTransform3d.getDescriptor();
  }

  @Override
  public ProtobufTransform3d createMessage() {
    return ProtobufTransform3d.newInstance();
  }

  @Override
  public Transform3d unpack(ProtobufTransform3d msg) {
    return new Transform3d(
        Translation3d.proto.unpack(msg.getTranslation()),
        Rotation3d.proto.unpack(msg.getRotation()));
  }

  @Override
  public void pack(ProtobufTransform3d msg, Transform3d value) {
    Translation3d.proto.pack(msg.getMutableTranslation(), value.getTranslation());
    Rotation3d.proto.pack(msg.getMutableRotation(), value.getRotation());
  }

  @Override
  public boolean isImmutable() {
    return true;
  }
}
