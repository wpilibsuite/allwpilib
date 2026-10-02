// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.simulation;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;
import org.wpilib.hardware.hal.HAL;

class PWMMotorControllerSimTest {
  @Test
  void testMotor() {
    HAL.initialize();

    try (var motor = new TestPWMMotorController(0)) {
      PWMMotorControllerSim sim = new PWMMotorControllerSim(motor);

      motor.setThrottle(0);
      assertEquals(0, sim.getThrottle());

      motor.setThrottle(0.354);
      assertEquals(0.354, sim.getThrottle());

      motor.setThrottle(-0.785);
      assertEquals(-0.785, sim.getThrottle());
    }
  }
}
