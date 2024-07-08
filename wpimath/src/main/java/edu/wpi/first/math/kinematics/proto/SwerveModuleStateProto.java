// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.kinematics.proto;

import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.kinematics.SwerveModuleState;
import edu.wpi.first.math.proto.Kinematics.ProtobufSwerveModuleState;
import edu.wpi.first.util.protobuf.Protobuf;
import us.hebi.quickbuf.Descriptors.Descriptor;

public class SwerveModuleStateProto
    implements Protobuf<SwerveModuleState, ProtobufSwerveModuleState> {
  @Override
  public Class<SwerveModuleState> getTypeClass() {
    return SwerveModuleState.class;
  }

  @Override
  public Descriptor getDescriptor() {
    return ProtobufSwerveModuleState.getDescriptor();
  }

  @Override
  public ProtobufSwerveModuleState createMessage() {
    return ProtobufSwerveModuleState.newInstance();
  }

  @Override
  public SwerveModuleState unpack(ProtobufSwerveModuleState msg) {
    return new SwerveModuleState(msg.getSpeed(), Rotation2d.proto.unpack(msg.getAngle()));
  }

  @Override
  public void pack(ProtobufSwerveModuleState msg, SwerveModuleState value) {
    msg.setSpeed(value.speedMetersPerSecond);
    Rotation2d.proto.pack(msg.getMutableAngle(), value.angle);
  }
}
