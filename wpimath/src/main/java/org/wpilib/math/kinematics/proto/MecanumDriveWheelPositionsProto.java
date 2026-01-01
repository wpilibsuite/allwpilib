// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics.proto;

import org.wpilib.math.kinematics.MecanumDriveWheelPositions;
import org.wpilib.math.proto.ProtobufKinematics.ProtobufMecanumDriveWheelPositions;
import org.wpilib.util.protobuf.Protobuf;
import us.hebi.quickbuf.Descriptors.Descriptor;

public class MecanumDriveWheelPositionsProto
    implements Protobuf<MecanumDriveWheelPositions, ProtobufMecanumDriveWheelPositions> {
  @Override
  public Class<MecanumDriveWheelPositions> getTypeClass() {
    return MecanumDriveWheelPositions.class;
  }

  @Override
  public Descriptor getDescriptor() {
    return ProtobufMecanumDriveWheelPositions.getDescriptor();
  }

  @Override
  public ProtobufMecanumDriveWheelPositions createMessage() {
    return ProtobufMecanumDriveWheelPositions.newInstance();
  }

  @Override
  public MecanumDriveWheelPositions unpack(ProtobufMecanumDriveWheelPositions msg) {
    return new MecanumDriveWheelPositions(
        msg.getFrontLeft(), msg.getFrontRight(), msg.getRearLeft(), msg.getRearRight());
  }

  @Override
  public void pack(ProtobufMecanumDriveWheelPositions msg, MecanumDriveWheelPositions value) {
    msg.setFrontLeft(value.frontLeft);
    msg.setFrontRight(value.frontRight);
    msg.setRearLeft(value.rearLeft);
    msg.setRearRight(value.rearRight);
  }
}
