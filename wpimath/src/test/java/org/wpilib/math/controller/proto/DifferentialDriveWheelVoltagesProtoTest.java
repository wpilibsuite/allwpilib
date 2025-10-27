// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.controller.proto;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.controller.DifferentialDriveWheelVoltages;
import edu.wpi.first.math.proto.Controller.ProtobufDifferentialDriveWheelVoltages;
import org.junit.jupiter.api.Test;

class DifferentialDriveWheelVoltagesProtoTest {
  private static final DifferentialDriveWheelVoltages DATA =
      new DifferentialDriveWheelVoltages(0.174, 0.191);

  @Test
  void testRoundtrip() {
    ProtobufDifferentialDriveWheelVoltages proto =
        DifferentialDriveWheelVoltages.proto.createMessage();
    DifferentialDriveWheelVoltages.proto.pack(proto, DATA);

    DifferentialDriveWheelVoltages data = DifferentialDriveWheelVoltages.proto.unpack(proto);
    assertEquals(DATA.left, data.left);
    assertEquals(DATA.right, data.right);
  }
}
