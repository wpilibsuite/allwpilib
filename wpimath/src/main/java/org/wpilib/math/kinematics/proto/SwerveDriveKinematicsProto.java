// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.kinematics.proto;

import edu.wpi.first.math.geometry.Translation2d;
import edu.wpi.first.math.kinematics.SwerveDriveKinematics;
import edu.wpi.first.math.proto.Kinematics.ProtobufSwerveDriveKinematics;
import edu.wpi.first.util.protobuf.Protobuf;
import us.hebi.quickbuf.Descriptors.Descriptor;

public final class SwerveDriveKinematicsProto
    implements Protobuf<SwerveDriveKinematics, ProtobufSwerveDriveKinematics> {
  @Override
  public Class<SwerveDriveKinematics> getTypeClass() {
    return SwerveDriveKinematics.class;
  }

  @Override
  public Descriptor getDescriptor() {
    return ProtobufSwerveDriveKinematics.getDescriptor();
  }

  @Override
  public ProtobufSwerveDriveKinematics createMessage() {
    return ProtobufSwerveDriveKinematics.newInstance();
  }

  @Override
  public SwerveDriveKinematics unpack(ProtobufSwerveDriveKinematics msg) {
    return new SwerveDriveKinematics(Protobuf.unpackArray(msg.getModules(), Translation2d.proto));
  }

  @Override
  public void pack(ProtobufSwerveDriveKinematics msg, SwerveDriveKinematics value) {
    Protobuf.packArray(msg.getMutableModules(), value.getModules(), Translation2d.proto);
  }
}
