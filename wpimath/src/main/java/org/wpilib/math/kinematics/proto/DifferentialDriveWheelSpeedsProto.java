// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics.proto;

import org.wpilib.math.kinematics.DifferentialDriveWheelSpeeds;
import org.wpilib.math.proto.Kinematics.ProtobufDifferentialDriveWheelSpeeds;
import edu.wpi.first.util.protobuf.Protobuf;
import us.hebi.quickbuf.Descriptors.Descriptor;

public class DifferentialDriveWheelSpeedsProto
    implements Protobuf<DifferentialDriveWheelSpeeds, ProtobufDifferentialDriveWheelSpeeds> {
  @Override
  public Class<DifferentialDriveWheelSpeeds> getTypeClass() {
    return DifferentialDriveWheelSpeeds.class;
  }

  @Override
  public Descriptor getDescriptor() {
    return ProtobufDifferentialDriveWheelSpeeds.getDescriptor();
  }

  @Override
  public ProtobufDifferentialDriveWheelSpeeds createMessage() {
    return ProtobufDifferentialDriveWheelSpeeds.newInstance();
  }

  @Override
  public DifferentialDriveWheelSpeeds unpack(ProtobufDifferentialDriveWheelSpeeds msg) {
    return new DifferentialDriveWheelSpeeds(msg.getLeft(), msg.getRight());
  }

  @Override
  public void pack(ProtobufDifferentialDriveWheelSpeeds msg, DifferentialDriveWheelSpeeds value) {
    msg.setLeft(value.left);
    msg.setRight(value.right);
  }
}
