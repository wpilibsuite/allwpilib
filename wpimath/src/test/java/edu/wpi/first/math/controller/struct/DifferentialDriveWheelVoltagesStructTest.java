// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.controller.struct;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.controller.DifferentialDriveWheelVoltages;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import org.junit.jupiter.api.Test;

class DifferentialDriveWheelVoltagesStructTest {
  private static final DifferentialDriveWheelVoltages DATA =
      new DifferentialDriveWheelVoltages(0.174, 0.191);

  @Test
  void testRoundtrip() {
    ByteBuffer buffer = ByteBuffer.allocate(DifferentialDriveWheelVoltages.struct.getSize());
    buffer.order(ByteOrder.LITTLE_ENDIAN);
    DifferentialDriveWheelVoltages.struct.pack(buffer, DATA);
    buffer.rewind();

    DifferentialDriveWheelVoltages data = DifferentialDriveWheelVoltages.struct.unpack(buffer);
    assertEquals(DATA.left, data.left);
    assertEquals(DATA.right, data.right);
  }
}
