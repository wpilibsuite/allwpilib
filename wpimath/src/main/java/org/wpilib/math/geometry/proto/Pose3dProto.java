// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.geometry.proto;

import edu.wpi.first.math.geometry.Pose3d;
import edu.wpi.first.math.geometry.Rotation3d;
import edu.wpi.first.math.geometry.Translation3d;
import edu.wpi.first.math.proto.Geometry3D.ProtobufPose3d;
import edu.wpi.first.util.protobuf.Protobuf;
import us.hebi.quickbuf.Descriptors.Descriptor;

public class Pose3dProto implements Protobuf<Pose3d, ProtobufPose3d> {
  @Override
  public Class<Pose3d> getTypeClass() {
    return Pose3d.class;
  }

  @Override
  public Descriptor getDescriptor() {
    return ProtobufPose3d.getDescriptor();
  }

  @Override
  public ProtobufPose3d createMessage() {
    return ProtobufPose3d.newInstance();
  }

  @Override
  public Pose3d unpack(ProtobufPose3d msg) {
    return new Pose3d(
        Translation3d.proto.unpack(msg.getTranslation()),
        Rotation3d.proto.unpack(msg.getRotation()));
  }

  @Override
  public void pack(ProtobufPose3d msg, Pose3d value) {
    Translation3d.proto.pack(msg.getMutableTranslation(), value.getTranslation());
    Rotation3d.proto.pack(msg.getMutableRotation(), value.getRotation());
  }

  @Override
  public boolean isImmutable() {
    return true;
  }
}
