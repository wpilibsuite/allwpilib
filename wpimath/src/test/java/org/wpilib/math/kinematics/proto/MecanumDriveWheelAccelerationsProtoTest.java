// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics.proto;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;
import org.wpilib.math.kinematics.MecanumDriveWheelAccelerations;
import org.wpilib.math.kinematics.proto.detail.ProtobufMecanumDriveWheelAccelerations;

class MecanumDriveWheelAccelerationsProtoTest {
  private static final MecanumDriveWheelAccelerations DATA =
      new MecanumDriveWheelAccelerations(2.29, 17.4, 4.4, 0.229);

  @Test
  void testRoundtrip() {
    ProtobufMecanumDriveWheelAccelerations proto =
        MecanumDriveWheelAccelerations.proto.createMessage();
    MecanumDriveWheelAccelerations.proto.pack(proto, DATA);

    MecanumDriveWheelAccelerations data = MecanumDriveWheelAccelerations.proto.unpack(proto);
    assertEquals(DATA.frontLeft, data.frontLeft);
    assertEquals(DATA.frontRight, data.frontRight);
    assertEquals(DATA.rearLeft, data.rearLeft);
    assertEquals(DATA.rearRight, data.rearRight);
  }
}
