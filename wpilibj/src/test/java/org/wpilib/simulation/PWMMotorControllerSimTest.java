// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.simulation;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;
import org.wpilib.hardware.hal.HAL;
import org.wpilib.hardware.motor.Spark;

class PWMMotorControllerSimTest {
  @Test
  void testMotor() {
    HAL.initialize(500, 0);

    try (Spark spark = new Spark(0)) {
      PWMMotorControllerSim sim = new PWMMotorControllerSim(spark);

      spark.setDutyCycle(0);
      assertEquals(0, sim.getDutyCycle());

      spark.setDutyCycle(0.354);
      assertEquals(0.354, sim.getDutyCycle());

      spark.setDutyCycle(-0.785);
      assertEquals(-0.785, sim.getDutyCycle());
    }
  }
}
