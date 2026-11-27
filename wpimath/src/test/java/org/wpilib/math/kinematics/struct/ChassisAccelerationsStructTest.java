// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics.struct;

import static org.junit.jupiter.api.Assertions.assertEquals;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import org.junit.jupiter.api.Test;
import org.wpilib.math.kinematics.ChassisAccelerations;

class ChassisAccelerationsStructTest {
  private static final ChassisAccelerations DATA = new ChassisAccelerations(2.29, 2.2, 0.3504);

  @Test
  void testRoundtrip() {
    ByteBuffer buffer = ByteBuffer.allocate(ChassisAccelerations.struct.getSize());
    buffer.order(ByteOrder.LITTLE_ENDIAN);
    ChassisAccelerations.struct.pack(buffer, DATA);
    buffer.rewind();

    ChassisAccelerations data = ChassisAccelerations.struct.unpack(buffer);
    assertEquals(DATA.ax, data.ax);
    assertEquals(DATA.ay, data.ay);
    assertEquals(DATA.alpha, data.alpha);
  }
}
