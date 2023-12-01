// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.system.plant.struct;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.system.plant.DCMotor;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import org.junit.jupiter.api.Test;

class DCMotorStructTest {
  private static final DCMotor DATA = new DCMotor(1.91, 19.1, 1.74, 1.74, 22.9, 3);

  @Test
  void testRoundtrip() {
    ByteBuffer buffer = ByteBuffer.allocate(DCMotor.struct.getSize());
    buffer.order(ByteOrder.LITTLE_ENDIAN);
    DCMotor.struct.pack(buffer, DATA);
    buffer.rewind();

    DCMotor data = DCMotor.struct.unpack(buffer);
    assertEquals(DATA.nominalVoltageVolts, data.nominalVoltageVolts);
    assertEquals(DATA.stallTorqueNewtonMeters, data.stallTorqueNewtonMeters);
    assertEquals(DATA.stallCurrentAmps, data.stallCurrentAmps);
    assertEquals(DATA.freeCurrentAmps, data.freeCurrentAmps);
    assertEquals(DATA.freeSpeedRadPerSec, data.freeSpeedRadPerSec);
    assertEquals(DATA.rOhms, data.rOhms);
    assertEquals(DATA.KvRadPerSecPerVolt, data.KvRadPerSecPerVolt);
    assertEquals(DATA.KtNMPerAmp, data.KtNMPerAmp);
  }
}
