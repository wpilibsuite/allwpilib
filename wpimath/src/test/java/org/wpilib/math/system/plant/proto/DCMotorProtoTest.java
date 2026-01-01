// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.system.plant.proto;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;
import org.wpilib.math.proto.ProtobufPlant.ProtobufDCMotor;
import org.wpilib.math.system.plant.DCMotor;

class DCMotorProtoTest {
  private static final DCMotor DATA = new DCMotor(1.91, 19.1, 1.74, 1.74, 22.9, 3);

  @Test
  void testRoundtrip() {
    ProtobufDCMotor proto = DCMotor.proto.createMessage();
    DCMotor.proto.pack(proto, DATA);

    DCMotor data = DCMotor.proto.unpack(proto);
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
