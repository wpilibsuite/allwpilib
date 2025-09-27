// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.kinematics.proto;

import edu.wpi.first.math.kinematics.DifferentialDriveWheelAccelerations;
import edu.wpi.first.math.proto.Kinematics.ProtobufDifferentialDriveWheelAccelerations;
import edu.wpi.first.util.protobuf.Protobuf;
import us.hebi.quickbuf.Descriptors.Descriptor;

public class DifferentialDriveWheelAccelerationsProto
    implements Protobuf<
        DifferentialDriveWheelAccelerations, ProtobufDifferentialDriveWheelAccelerations> {
  @Override
  public Class<DifferentialDriveWheelAccelerations> getTypeClass() {
    return DifferentialDriveWheelAccelerations.class;
  }

  @Override
  public Descriptor getDescriptor() {
    return ProtobufDifferentialDriveWheelAccelerations.getDescriptor();
  }

  @Override
  public ProtobufDifferentialDriveWheelAccelerations createMessage() {
    return ProtobufDifferentialDriveWheelAccelerations.newInstance();
  }

  @Override
  public DifferentialDriveWheelAccelerations unpack(
      ProtobufDifferentialDriveWheelAccelerations msg) {
    return new DifferentialDriveWheelAccelerations(msg.getLeft(), msg.getRight());
  }

  @Override
  public void pack(
      ProtobufDifferentialDriveWheelAccelerations msg, DifferentialDriveWheelAccelerations value) {
    msg.setLeft(value.left);
    msg.setRight(value.right);
  }
}
