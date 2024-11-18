// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics.proto;

import org.wpilib.math.kinematics.MecanumDriveMotorVoltages;
import org.wpilib.math.proto.Kinematics.ProtobufMecanumDriveMotorVoltages;
import org.wpilib.util.protobuf.Protobuf;
import us.hebi.quickbuf.Descriptors.Descriptor;

public final class MecanumDriveMotorVoltagesProto
    implements Protobuf<MecanumDriveMotorVoltages, ProtobufMecanumDriveMotorVoltages> {
  @Override
  public Class<MecanumDriveMotorVoltages> getTypeClass() {
    return MecanumDriveMotorVoltages.class;
  }

  @Override
  public Descriptor getDescriptor() {
    return ProtobufMecanumDriveMotorVoltages.getDescriptor();
  }

  @Override
  public ProtobufMecanumDriveMotorVoltages createMessage() {
    return ProtobufMecanumDriveMotorVoltages.newInstance();
  }

  @Override
  public MecanumDriveMotorVoltages unpack(ProtobufMecanumDriveMotorVoltages msg) {
    return new MecanumDriveMotorVoltages(
        msg.getFrontLeft(), msg.getFrontRight(), msg.getRearLeft(), msg.getRearRight());
  }

  @Override
  public void pack(ProtobufMecanumDriveMotorVoltages msg, MecanumDriveMotorVoltages value) {
    msg.setFrontLeft(value.frontLeftVoltage);
    msg.setFrontRight(value.frontRightVoltage);
    msg.setRearLeft(value.rearLeftVoltage);
    msg.setRearRight(value.rearRightVoltage);
  }
}
