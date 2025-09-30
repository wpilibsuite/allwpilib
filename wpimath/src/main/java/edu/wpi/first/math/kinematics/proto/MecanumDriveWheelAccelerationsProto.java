// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.kinematics.proto;

import edu.wpi.first.math.kinematics.MecanumDriveWheelAccelerations;
import edu.wpi.first.math.proto.Kinematics.ProtobufMecanumDriveWheelAccelerations;
import edu.wpi.first.util.protobuf.Protobuf;
import us.hebi.quickbuf.Descriptors.Descriptor;

public class MecanumDriveWheelAccelerationsProto
    implements Protobuf<MecanumDriveWheelAccelerations, ProtobufMecanumDriveWheelAccelerations> {
  @Override
  public Class<MecanumDriveWheelAccelerations> getTypeClass() {
    return MecanumDriveWheelAccelerations.class;
  }

  @Override
  public Descriptor getDescriptor() {
    return ProtobufMecanumDriveWheelAccelerations.getDescriptor();
  }

  @Override
  public ProtobufMecanumDriveWheelAccelerations createMessage() {
    return ProtobufMecanumDriveWheelAccelerations.newInstance();
  }

  @Override
  public MecanumDriveWheelAccelerations unpack(ProtobufMecanumDriveWheelAccelerations msg) {
    return new MecanumDriveWheelAccelerations(
        msg.getFrontLeft(), msg.getFrontRight(), msg.getRearLeft(), msg.getRearRight());
  }

  @Override
  public void pack(
      ProtobufMecanumDriveWheelAccelerations msg, MecanumDriveWheelAccelerations value) {
    msg.setFrontLeft(value.frontLeft);
    msg.setFrontRight(value.frontRight);
    msg.setRearLeft(value.rearLeft);
    msg.setRearRight(value.rearRight);
  }
}
