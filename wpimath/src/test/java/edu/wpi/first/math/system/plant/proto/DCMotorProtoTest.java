// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.system.plant.proto;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.proto.Plant.ProtobufDCMotor;
import edu.wpi.first.math.system.plant.DCMotor;
import org.junit.jupiter.api.Test;

class DCMotorProtoTest {
  private static final DCMotor DATA = new DCMotor(1.91, 19.1, 1.74, 1.74, 22.9);

  @Test
  void testRoundtrip() {
    ProtobufDCMotor proto = DCMotor.proto.createMessage();
    DCMotor.proto.pack(proto, DATA);

    DCMotor data = DCMotor.proto.unpack(proto);
    assertEquals(DATA.nominalVoltage.baseUnitMagnitude(), data.nominalVoltage.baseUnitMagnitude());
    assertEquals(DATA.stallTorque.baseUnitMagnitude(), data.stallTorque.baseUnitMagnitude());
    assertEquals(DATA.stallCurrent.baseUnitMagnitude(), data.stallCurrent.baseUnitMagnitude());
    assertEquals(DATA.freeCurrent.baseUnitMagnitude(), data.freeCurrent.baseUnitMagnitude());
    assertEquals(DATA.freeSpeed.baseUnitMagnitude(), data.freeSpeed.baseUnitMagnitude());
    assertEquals(DATA.internalResistance.baseUnitMagnitude(), data.internalResistance.baseUnitMagnitude());
    assertEquals(DATA.kv.baseUnitMagnitude(), data.kv.baseUnitMagnitude());
    assertEquals(DATA.kt.baseUnitMagnitude(), data.kt.baseUnitMagnitude());
  }
}
