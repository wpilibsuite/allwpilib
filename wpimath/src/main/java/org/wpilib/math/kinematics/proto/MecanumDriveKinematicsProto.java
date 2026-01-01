// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics.proto;

import org.wpilib.math.geometry.Translation2d;
import org.wpilib.math.kinematics.MecanumDriveKinematics;
import org.wpilib.math.proto.ProtobufKinematics.ProtobufMecanumDriveKinematics;
import org.wpilib.util.protobuf.Protobuf;
import us.hebi.quickbuf.Descriptors.Descriptor;

public class MecanumDriveKinematicsProto
    implements Protobuf<MecanumDriveKinematics, ProtobufMecanumDriveKinematics> {
  @Override
  public Class<MecanumDriveKinematics> getTypeClass() {
    return MecanumDriveKinematics.class;
  }

  @Override
  public Descriptor getDescriptor() {
    return ProtobufMecanumDriveKinematics.getDescriptor();
  }

  @Override
  public ProtobufMecanumDriveKinematics createMessage() {
    return ProtobufMecanumDriveKinematics.newInstance();
  }

  @Override
  public MecanumDriveKinematics unpack(ProtobufMecanumDriveKinematics msg) {
    return new MecanumDriveKinematics(
        Translation2d.proto.unpack(msg.getFrontLeft()),
        Translation2d.proto.unpack(msg.getFrontRight()),
        Translation2d.proto.unpack(msg.getRearLeft()),
        Translation2d.proto.unpack(msg.getRearRight()));
  }

  @Override
  public void pack(ProtobufMecanumDriveKinematics msg, MecanumDriveKinematics value) {
    Translation2d.proto.pack(msg.getMutableFrontLeft(), value.getFrontLeft());
    Translation2d.proto.pack(msg.getMutableFrontRight(), value.getFrontRight());
    Translation2d.proto.pack(msg.getMutableRearLeft(), value.getRearLeft());
    Translation2d.proto.pack(msg.getMutableRearRight(), value.getRearRight());
  }
}
