// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics.proto;

import org.wpilib.math.kinematics.MecanumDriveWheelSpeeds;
import org.wpilib.math.proto.ProtobufMecanumDriveWheelSpeeds;
import org.wpilib.util.protobuf.Protobuf;
import us.hebi.quickbuf.Descriptors.Descriptor;

public class MecanumDriveWheelSpeedsProto
    implements Protobuf<MecanumDriveWheelSpeeds, ProtobufMecanumDriveWheelSpeeds> {
  @Override
  public Class<MecanumDriveWheelSpeeds> getTypeClass() {
    return MecanumDriveWheelSpeeds.class;
  }

  @Override
  public Descriptor getDescriptor() {
    return ProtobufMecanumDriveWheelSpeeds.getDescriptor();
  }

  @Override
  public ProtobufMecanumDriveWheelSpeeds createMessage() {
    return ProtobufMecanumDriveWheelSpeeds.newInstance();
  }

  @Override
  public MecanumDriveWheelSpeeds unpack(ProtobufMecanumDriveWheelSpeeds msg) {
    return new MecanumDriveWheelSpeeds(
        msg.getFrontLeft(), msg.getFrontRight(), msg.getRearLeft(), msg.getRearRight());
  }

  @Override
  public void pack(ProtobufMecanumDriveWheelSpeeds msg, MecanumDriveWheelSpeeds value) {
    msg.setFrontLeft(value.frontLeft);
    msg.setFrontRight(value.frontRight);
    msg.setRearLeft(value.rearLeft);
    msg.setRearRight(value.rearRight);
  }
}
