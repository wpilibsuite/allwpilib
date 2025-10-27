// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.kinematics.proto;

import edu.wpi.first.math.kinematics.DifferentialDriveKinematics;
import edu.wpi.first.math.proto.Kinematics.ProtobufDifferentialDriveKinematics;
import edu.wpi.first.util.protobuf.Protobuf;
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
