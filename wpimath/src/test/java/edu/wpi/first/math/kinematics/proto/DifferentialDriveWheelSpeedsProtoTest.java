// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.kinematics.proto;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.kinematics.DifferentialDriveWheelSpeeds;
import edu.wpi.first.math.proto.Kinematics.ProtobufDifferentialDriveWheelSpeeds;
import org.junit.jupiter.api.Test;

class DifferentialDriveWheelSpeedsProtoTest {
  private static final DifferentialDriveWheelSpeeds DATA =
      new DifferentialDriveWheelSpeeds(1.74, 35.04);

  @Test
  void testRoundtrip() {
    ProtobufDifferentialDriveWheelSpeeds proto = DifferentialDriveWheelSpeeds.proto.createMessage();
    DifferentialDriveWheelSpeeds.proto.pack(proto, DATA);

    DifferentialDriveWheelSpeeds data = DifferentialDriveWheelSpeeds.proto.unpack(proto);
    assertEquals(DATA.left, data.left);
    assertEquals(DATA.right, data.right);
  }
}
