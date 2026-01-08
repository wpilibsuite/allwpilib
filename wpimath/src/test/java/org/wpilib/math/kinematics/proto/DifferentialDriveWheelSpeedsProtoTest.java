// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics.proto;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;
import org.wpilib.math.kinematics.DifferentialDriveWheelSpeeds;
import org.wpilib.math.proto.ProtobufDifferentialDriveWheelSpeeds;

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
