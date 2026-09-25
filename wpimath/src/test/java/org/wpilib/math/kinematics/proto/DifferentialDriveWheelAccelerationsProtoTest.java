// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics.proto;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;
import org.wpilib.math.kinematics.DifferentialDriveWheelAccelerations;
import org.wpilib.math.kinematics.proto.detail.ProtobufDifferentialDriveWheelAccelerations;

class DifferentialDriveWheelAccelerationsProtoTest {
  private static final DifferentialDriveWheelAccelerations DATA =
      new DifferentialDriveWheelAccelerations(1.74, 35.04);

  @Test
  void testRoundtrip() {
    ProtobufDifferentialDriveWheelAccelerations proto =
        DifferentialDriveWheelAccelerations.proto.createMessage();
    DifferentialDriveWheelAccelerations.proto.pack(proto, DATA);

    DifferentialDriveWheelAccelerations data =
        DifferentialDriveWheelAccelerations.proto.unpack(proto);
    assertEquals(DATA.left, data.left);
    assertEquals(DATA.right, data.right);
  }
}
