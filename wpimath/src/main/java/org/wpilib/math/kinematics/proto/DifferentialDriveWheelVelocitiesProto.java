// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics.proto;

import org.wpilib.math.kinematics.DifferentialDriveWheelVelocities;
import org.wpilib.math.proto.Kinematics.ProtobufDifferentialDriveWheelVelocities;
import org.wpilib.util.protobuf.Protobuf;
import us.hebi.quickbuf.Descriptors.Descriptor;

public class DifferentialDriveWheelVelocitiesProto
    implements Protobuf<
        DifferentialDriveWheelVelocities, ProtobufDifferentialDriveWheelVelocities> {
  @Override
  public Class<DifferentialDriveWheelVelocities> getTypeClass() {
    return DifferentialDriveWheelVelocities.class;
  }

  @Override
  public Descriptor getDescriptor() {
    return ProtobufDifferentialDriveWheelVelocities.getDescriptor();
  }

  @Override
  public ProtobufDifferentialDriveWheelVelocities createMessage() {
    return ProtobufDifferentialDriveWheelVelocities.newInstance();
  }

  @Override
  public DifferentialDriveWheelVelocities unpack(ProtobufDifferentialDriveWheelVelocities msg) {
    return new DifferentialDriveWheelVelocities(msg.getLeft(), msg.getRight());
  }

  @Override
  public void pack(
      ProtobufDifferentialDriveWheelVelocities msg, DifferentialDriveWheelVelocities value) {
    msg.setLeft(value.left);
    msg.setRight(value.right);
  }
}
