// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics.proto;

import org.wpilib.math.geometry.Rotation2d;
import org.wpilib.math.kinematics.SwerveModuleAcceleration;
import org.wpilib.math.proto.ProtobufKinematics.ProtobufSwerveModuleAcceleration;
import org.wpilib.util.protobuf.Protobuf;
import us.hebi.quickbuf.Descriptors.Descriptor;

public class SwerveModuleAccelerationProto
    implements Protobuf<SwerveModuleAcceleration, ProtobufSwerveModuleAcceleration> {
  @Override
  public Class<SwerveModuleAcceleration> getTypeClass() {
    return SwerveModuleAcceleration.class;
  }

  @Override
  public Descriptor getDescriptor() {
    return ProtobufSwerveModuleAcceleration.getDescriptor();
  }

  @Override
  public ProtobufSwerveModuleAcceleration createMessage() {
    return ProtobufSwerveModuleAcceleration.newInstance();
  }

  @Override
  public SwerveModuleAcceleration unpack(ProtobufSwerveModuleAcceleration msg) {
    return new SwerveModuleAcceleration(
        msg.getAcceleration(), Rotation2d.proto.unpack(msg.getAngle()));
  }

  @Override
  public void pack(ProtobufSwerveModuleAcceleration msg, SwerveModuleAcceleration value) {
    msg.setAcceleration(value.acceleration);
    Rotation2d.proto.pack(msg.getMutableAngle(), value.angle);
  }
}
