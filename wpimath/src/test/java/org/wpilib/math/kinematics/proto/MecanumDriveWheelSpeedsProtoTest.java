// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.kinematics.proto;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.kinematics.MecanumDriveWheelSpeeds;
import edu.wpi.first.math.proto.Kinematics.ProtobufMecanumDriveWheelSpeeds;
import org.junit.jupiter.api.Test;

class MecanumDriveWheelSpeedsProtoTest {
  private static final MecanumDriveWheelSpeeds DATA =
      new MecanumDriveWheelSpeeds(2.29, 17.4, 4.4, 0.229);

  @Test
  void testRoundtrip() {
    ProtobufMecanumDriveWheelSpeeds proto = MecanumDriveWheelSpeeds.proto.createMessage();
    MecanumDriveWheelSpeeds.proto.pack(proto, DATA);

    MecanumDriveWheelSpeeds data = MecanumDriveWheelSpeeds.proto.unpack(proto);
    assertEquals(DATA.frontLeft, data.frontLeft);
    assertEquals(DATA.frontRight, data.frontRight);
    assertEquals(DATA.rearLeft, data.rearLeft);
    assertEquals(DATA.rearRight, data.rearRight);
  }
}
