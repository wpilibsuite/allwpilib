// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.drivers.motor;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;
import org.wpilib.hardware.hal.HAL;
import org.wpilib.simulation.PWMMotorControllerSim;

class PWMMotorControllerTest {
  @Test
  void reportsThrottleToSimulation() {
    HAL.initialize();

    try (var motor = new Spark(0)) {
      var sim = new PWMMotorControllerSim(motor);

      motor.setThrottle(0.354);

      assertEquals(0.354, sim.getThrottle());
    }
  }
}
