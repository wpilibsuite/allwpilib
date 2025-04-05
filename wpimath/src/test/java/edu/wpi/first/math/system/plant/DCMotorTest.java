// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.system.plant;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.util.Units;
import org.junit.jupiter.api.Test;

class DCMotorTest {
  @Test
  void testDcMotorEquality() {
    var gearboxA = DCMotor.getKrakenX60(2);
    var gearboxB =
        new DCMotor(12, 14.18, 732, 4, Units.rotationsPerMinuteToRadiansPerSecond(6000), 1);

    assertEquals(gearboxA, gearboxB);
  }
}
