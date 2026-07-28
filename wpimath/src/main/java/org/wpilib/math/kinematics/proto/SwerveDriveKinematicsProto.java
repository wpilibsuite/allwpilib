// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics.proto;

import org.wpilib.math.geometry.Translation2d;
import org.wpilib.math.kinematics.SwerveDriveKinematics;
import org.wpilib.math.proto.ProtobufSwerveDriveKinematics;
import org.wpilib.util.protobuf.Protobuf;
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
