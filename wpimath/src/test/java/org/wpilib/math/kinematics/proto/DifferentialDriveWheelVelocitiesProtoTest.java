// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics.proto;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;
import org.wpilib.math.kinematics.DifferentialDriveWheelVelocities;
import org.wpilib.math.proto.Kinematics.ProtobufDifferentialDriveWheelVelocities;

class DifferentialDriveWheelVelocitiesProtoTest {
  private static final DifferentialDriveWheelVelocities DATA =
      new DifferentialDriveWheelVelocities(1.74, 35.04);

  @Test
  void testRoundtrip() {
    ProtobufDifferentialDriveWheelVelocities proto =
        DifferentialDriveWheelVelocities.proto.createMessage();
    DifferentialDriveWheelVelocities.proto.pack(proto, DATA);

    DifferentialDriveWheelVelocities data = DifferentialDriveWheelVelocities.proto.unpack(proto);
    assertEquals(DATA.left, data.left);
    assertEquals(DATA.right, data.right);
  }
}
