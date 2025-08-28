// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics.proto;

import org.wpilib.math.kinematics.ChassisSpeeds;
import org.wpilib.math.proto.Kinematics.ProtobufChassisSpeeds;
import edu.wpi.first.util.protobuf.Protobuf;
import us.hebi.quickbuf.Descriptors.Descriptor;

public class ChassisSpeedsProto implements Protobuf<ChassisSpeeds, ProtobufChassisSpeeds> {
  @Override
  public Class<ChassisSpeeds> getTypeClass() {
    return ChassisSpeeds.class;
  }

  @Override
  public Descriptor getDescriptor() {
    return ProtobufChassisSpeeds.getDescriptor();
  }

  @Override
  public ProtobufChassisSpeeds createMessage() {
    return ProtobufChassisSpeeds.newInstance();
  }

  @Override
  public ChassisSpeeds unpack(ProtobufChassisSpeeds msg) {
    return new ChassisSpeeds(msg.getVx(), msg.getVy(), msg.getOmega());
  }

  @Override
  public void pack(ProtobufChassisSpeeds msg, ChassisSpeeds value) {
    msg.setVx(value.vx);
    msg.setVy(value.vy);
    msg.setOmega(value.omega);
  }
}
