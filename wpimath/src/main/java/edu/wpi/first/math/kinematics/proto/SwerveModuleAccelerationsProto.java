// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.kinematics.proto;

import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.kinematics.SwerveModuleAccelerations;
import edu.wpi.first.math.proto.Kinematics.ProtobufSwerveModuleAccelerations;
import edu.wpi.first.util.protobuf.Protobuf;
import us.hebi.quickbuf.Descriptors.Descriptor;

public class SwerveModuleAccelerationsProto
    implements Protobuf<SwerveModuleAccelerations, ProtobufSwerveModuleAccelerations> {
  @Override
  public Class<SwerveModuleAccelerations> getTypeClass() {
    return SwerveModuleAccelerations.class;
  }

  @Override
  public Descriptor getDescriptor() {
    return ProtobufSwerveModuleAccelerations.getDescriptor();
  }

  @Override
  public ProtobufSwerveModuleAccelerations createMessage() {
    return ProtobufSwerveModuleAccelerations.newInstance();
  }

  @Override
  public SwerveModuleAccelerations unpack(ProtobufSwerveModuleAccelerations msg) {
    return new SwerveModuleAccelerations(msg.getAcceleration(), new Rotation2d(msg.getAngle()));
  }

  @Override
  public void pack(ProtobufSwerveModuleAccelerations msg, SwerveModuleAccelerations value) {
    msg.setAcceleration(value.acceleration);
    msg.setAngle(value.angle.getRadians());
  }
}
