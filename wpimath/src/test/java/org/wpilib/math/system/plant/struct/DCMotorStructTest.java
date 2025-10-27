// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.system.plant.struct;

import static org.junit.jupiter.api.Assertions.assertEquals;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import org.junit.jupiter.api.Test;
import org.wpilib.math.system.plant.DCMotor;

class DCMotorStructTest {
  private static final DCMotor DATA = new DCMotor(1.91, 19.1, 1.74, 1.74, 22.9, 3);

  @Test
  void testRoundtrip() {
    ByteBuffer buffer = ByteBuffer.allocate(DCMotor.struct.getSize());
    buffer.order(ByteOrder.LITTLE_ENDIAN);
    DCMotor.struct.pack(buffer, DATA);
    buffer.rewind();

    DCMotor data = DCMotor.struct.unpack(buffer);
    assertEquals(DATA.nominalVoltage, data.nominalVoltage);
    assertEquals(DATA.stallTorque, data.stallTorque);
    assertEquals(DATA.stallCurrent, data.stallCurrent);
    assertEquals(DATA.freeCurrent, data.freeCurrent);
    assertEquals(DATA.freeSpeed, data.freeSpeed);
    assertEquals(DATA.R, data.R);
    assertEquals(DATA.Kv, data.Kv);
    assertEquals(DATA.Kt, data.Kt);
  }
}
