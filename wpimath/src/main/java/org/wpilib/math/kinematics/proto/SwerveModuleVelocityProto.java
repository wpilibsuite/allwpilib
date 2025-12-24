// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics.proto;

import org.wpilib.math.geometry.Rotation2d;
import org.wpilib.math.kinematics.SwerveModuleVelocity;
import org.wpilib.math.proto.Kinematics.ProtobufSwerveModuleVelocity;
import org.wpilib.util.protobuf.Protobuf;
import us.hebi.quickbuf.Descriptors.Descriptor;

public class SwerveModuleVelocityProto
    implements Protobuf<SwerveModuleVelocity, ProtobufSwerveModuleVelocity> {
  @Override
  public Class<SwerveModuleVelocity> getTypeClass() {
    return SwerveModuleVelocity.class;
  }

  @Override
  public Descriptor getDescriptor() {
    return ProtobufSwerveModuleVelocity.getDescriptor();
  }

  @Override
  public ProtobufSwerveModuleVelocity createMessage() {
    return ProtobufSwerveModuleVelocity.newInstance();
  }

  @Override
  public SwerveModuleVelocity unpack(ProtobufSwerveModuleVelocity msg) {
    return new SwerveModuleVelocity(msg.getVelocity(), Rotation2d.proto.unpack(msg.getAngle()));
  }

  @Override
  public void pack(ProtobufSwerveModuleVelocity msg, SwerveModuleVelocity value) {
    msg.setVelocity(value.velocity);
    Rotation2d.proto.pack(msg.getMutableAngle(), value.angle);
  }
}
