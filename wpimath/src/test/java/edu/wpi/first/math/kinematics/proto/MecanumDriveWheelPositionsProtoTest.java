// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.kinematics.proto;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.kinematics.MecanumDriveWheelPositions;
import edu.wpi.first.math.proto.Kinematics.ProtobufMecanumDriveWheelPositions;
import org.junit.jupiter.api.Test;

class MecanumDriveWheelPositionsProtoTest {
  private static final MecanumDriveWheelPositions DATA =
      new MecanumDriveWheelPositions(17.4, 2.29, 22.9, 1.74);

  @Test
  void testRoundtrip() {
    ProtobufMecanumDriveWheelPositions proto = MecanumDriveWheelPositions.proto.createMessage();
    MecanumDriveWheelPositions.proto.pack(proto, DATA);

    MecanumDriveWheelPositions data = MecanumDriveWheelPositions.proto.unpack(proto);
    assertEquals(DATA.frontLeftMeters, data.frontLeftMeters);
    assertEquals(DATA.frontRightMeters, data.frontRightMeters);
    assertEquals(DATA.rearLeftMeters, data.rearLeftMeters);
    assertEquals(DATA.rearRightMeters, data.rearRightMeters);
  }
}
