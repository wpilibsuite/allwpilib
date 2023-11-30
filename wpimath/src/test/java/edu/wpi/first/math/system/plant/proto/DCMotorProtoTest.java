// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.system.plant.proto;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.proto.Plant.ProtobufDCMotor;
import edu.wpi.first.math.system.plant.DCMotor;
import org.junit.jupiter.api.Test;

class DCMotorProtoTest {
  private static final DCMotor DATA = new DCMotor(1.91, 19.1, 1.74, 1.74, 22.9, 3);

  @Test
  void testRoundtrip() {
    ProtobufDCMotor proto = DCMotor.proto.createMessage();
    DCMotor.proto.pack(proto, DATA);

    DCMotor data = DCMotor.proto.unpack(proto);
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
