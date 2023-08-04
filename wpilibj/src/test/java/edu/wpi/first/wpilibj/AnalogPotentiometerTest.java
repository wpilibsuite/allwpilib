// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.hal.HAL;
import edu.wpi.first.wpilibj.simulation.AnalogInputSim;
import edu.wpi.first.wpilibj.simulation.RoboRioSim;
import org.junit.jupiter.api.Test;

class AnalogPotentiometerTest {
  @Test
  void testInitializeWithAnalogInput() {
    HAL.initialize(500, 0);

    try (AnalogInput ai = new AnalogInput(0);
        AnalogPotentiometer pot = new AnalogPotentiometer(ai)) {
      AnalogInputSim sim = new AnalogInputSim(ai);

      RoboRioSim.resetData();
      sim.setVoltage(4.0);
      assertEquals(0.8, pot.get());
    }
  }

  @Test
  void testInitializeWithAnalogInputAndScale() {
    HAL.initialize(500, 0);

    try (AnalogInput ai = new AnalogInput(0);
        AnalogPotentiometer pot = new AnalogPotentiometer(ai, 270.0)) {
      RoboRioSim.resetData();
      AnalogInputSim sim = new AnalogInputSim(ai);

      sim.setVoltage(5.0);
      assertEquals(270.0, pot.get());

      sim.setVoltage(2.5);
      assertEquals(135, pot.get());

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

      sim.setVoltage(5.0);
      assertEquals(1.0, pot.get());
    }
  }

  @Test
  void testInitializeWithChannelAndScale() {
    HAL.initialize(500, 0);

    try (AnalogPotentiometer pot = new AnalogPotentiometer(1, 180.0)) {
      RoboRioSim.resetData();
      AnalogInputSim sim = new AnalogInputSim(1);

      sim.setVoltage(5.0);
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

      // Test at 5v
      sim.setVoltage(5.0);
      assertEquals(270, pot.get());

      sim.setVoltage(0.0);
      assertEquals(90, pot.get());

      // Simulate a lower battery voltage
      RoboRioSim.setUserVoltage5V(2.5);

      sim.setVoltage(2.5);
      assertEquals(270, pot.get());

      sim.setVoltage(2.0);
      assertEquals(234, pot.get());

      sim.setVoltage(0.0);
      assertEquals(90, pot.get());
    }
  }
}
