// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.kinematics.proto;

import edu.wpi.first.math.kinematics.DifferentialDriveWheelPositions;
import edu.wpi.first.math.proto.Kinematics.ProtobufDifferentialDriveWheelPositions;
import edu.wpi.first.util.protobuf.Protobuf;
import us.hebi.quickbuf.Descriptors.Descriptor;

public class DifferentialDriveWheelPositionsProto
    implements Protobuf<DifferentialDriveWheelPositions, ProtobufDifferentialDriveWheelPositions> {
  @Override
  public Class<DifferentialDriveWheelPositions> getTypeClass() {
    return DifferentialDriveWheelPositions.class;
  }

  @Override
  public Descriptor getDescriptor() {
    return ProtobufDifferentialDriveWheelPositions.getDescriptor();
  }

  @Override
  public ProtobufDifferentialDriveWheelPositions createMessage() {
    return ProtobufDifferentialDriveWheelPositions.newInstance();
  }

  @Override
  public DifferentialDriveWheelPositions unpack(ProtobufDifferentialDriveWheelPositions msg) {
    return new DifferentialDriveWheelPositions(msg.getLeft(), msg.getRight());
  }

  @Override
  public void pack(
      ProtobufDifferentialDriveWheelPositions msg, DifferentialDriveWheelPositions value) {
    msg.setLeft(value.leftMeters);
    msg.setRight(value.rightMeters);
  }
}
