// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.geometry.proto;

import edu.wpi.first.math.geometry.Quaternion;
import edu.wpi.first.math.proto.Geometry3D.ProtobufQuaternion;
import edu.wpi.first.util.protobuf.Protobuf;
import us.hebi.quickbuf.Descriptors.Descriptor;

public class QuaternionProto implements Protobuf<Quaternion, ProtobufQuaternion> {
  @Override
  public Class<Quaternion> getTypeClass() {
    return Quaternion.class;
  }

  @Override
  public Descriptor getDescriptor() {
    return ProtobufQuaternion.getDescriptor();
  }

  @Override
  public ProtobufQuaternion createMessage() {
    return ProtobufQuaternion.newInstance();
  }

  @Override
  public Quaternion unpack(ProtobufQuaternion msg) {
    return new Quaternion(msg.getW(), msg.getX(), msg.getY(), msg.getZ());
  }

  @Override
  public void pack(ProtobufQuaternion msg, Quaternion value) {
    msg.setW(value.getW());
    msg.setX(value.getX());
    msg.setY(value.getY());
    msg.setZ(value.getZ());
  }

  @Override
  public boolean isImmutable() {
    return true;
  }
}
