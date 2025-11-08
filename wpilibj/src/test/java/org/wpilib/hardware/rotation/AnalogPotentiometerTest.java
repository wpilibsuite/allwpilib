// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.hardware.rotation;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.wpilib.hardware.hal.HAL;
import org.wpilib.simulation.AnalogInputSim;
import org.wpilib.simulation.RoboRioSim;
import org.junit.jupiter.api.Test;

class AnalogPotentiometerTest {
  @Test
  void testInitializeWithAnalogInput() {
    HAL.initialize(500, 0);

    try (AnalogInput ai = new AnalogInput(0);
        AnalogPotentiometer pot = new AnalogPotentiometer(ai)) {
      AnalogInputSim sim = new AnalogInputSim(ai);

      RoboRioSim.resetData();
      sim.setVoltage(2.8);
      assertEquals(2.8 / 3.3, pot.get());
    }
  }

  @Test
  void testInitializeWithAnalogInputAndScale() {
    HAL.initialize(500, 0);

    try (AnalogInput ai = new AnalogInput(0);
        AnalogPotentiometer pot = new AnalogPotentiometer(ai, 270.0)) {
      RoboRioSim.resetData();
      AnalogInputSim sim = new AnalogInputSim(ai);

      sim.setVoltage(3.3);
      assertEquals(270.0, pot.get());

      sim.setVoltage(2.5);
      assertEquals((2.5 / 3.3) * 270.0, pot.get());

      sim.setVoltage(0.0);
      assertEquals(0.0, pot.get());
    }
  }

  @Test
  void testInitializeWithChannel() {
    HAL.initialize(500, 0);

    try (AnalogPotentiometer pot = new AnalogPotentiometer(1)) {
      RoboRioSim.resetData();
      AnalogInputSim sim = new AnalogInputSim(1);

      sim.setVoltage(3.3);
      assertEquals(1.0, pot.get());
    }
  }

  @Test
  void testInitializeWithChannelAndScale() {
    HAL.initialize(500, 0);

    try (AnalogPotentiometer pot = new AnalogPotentiometer(1, 180.0)) {
      RoboRioSim.resetData();
      AnalogInputSim sim = new AnalogInputSim(1);

      sim.setVoltage(3.3);
      assertEquals(180.0, pot.get());

      sim.setVoltage(0.0);
      assertEquals(0.0, pot.get());
    }
  }

  @Test
  void testWithModifiedBatteryVoltage() {
    try (AnalogPotentiometer pot = new AnalogPotentiometer(1, 180.0, 90.0)) {
      RoboRioSim.resetData();
      AnalogInputSim sim = new AnalogInputSim(1);

      // Test at 3.3v
      sim.setVoltage(3.3);
      assertEquals(270, pot.get());

      sim.setVoltage(0.0);
      assertEquals(90, pot.get());

      // Simulate a lower battery voltage
      RoboRioSim.setUserVoltage3V3(2.5);

      sim.setVoltage(2.5);
      assertEquals(270, pot.get());

      sim.setVoltage(2.0);
      assertEquals(234, pot.get());

      sim.setVoltage(0.0);
      assertEquals(90, pot.get());
    }
  }
}
