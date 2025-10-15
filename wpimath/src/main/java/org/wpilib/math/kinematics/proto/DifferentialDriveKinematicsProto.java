// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics.proto;

import org.wpilib.math.kinematics.DifferentialDriveKinematics;
import org.wpilib.math.proto.Kinematics.ProtobufDifferentialDriveKinematics;
import org.wpilib.util.protobuf.Protobuf;
import us.hebi.quickbuf.Descriptors.Descriptor;

public class DifferentialDriveKinematicsProto
    implements Protobuf<DifferentialDriveKinematics, ProtobufDifferentialDriveKinematics> {
  @Override
  public Class<DifferentialDriveKinematics> getTypeClass() {
    return DifferentialDriveKinematics.class;
  }

  @Override
  public Descriptor getDescriptor() {
    return ProtobufDifferentialDriveKinematics.getDescriptor();
  }

  @Override
  public ProtobufDifferentialDriveKinematics createMessage() {
    return ProtobufDifferentialDriveKinematics.newInstance();
  }

  @Override
  public DifferentialDriveKinematics unpack(ProtobufDifferentialDriveKinematics msg) {
    return new DifferentialDriveKinematics(msg.getTrackwidth());
  }

  @Override
  public void pack(ProtobufDifferentialDriveKinematics msg, DifferentialDriveKinematics value) {
    msg.setTrackwidth(value.trackwidth);
  }
}
