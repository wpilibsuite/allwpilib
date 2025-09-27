// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.hardware.power;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;
import org.wpilib.hardware.hal.HAL;
import org.wpilib.hardware.power.PowerDistribution.ModuleType;
import org.wpilib.simulation.PDPSim;

class PowerDistributionTest {
  @Test
  void testGetAllCurrents() {
    HAL.initialize(500, 0);
    PowerDistribution pdp = new PowerDistribution(0, 1, ModuleType.kRev);
    PDPSim sim = new PDPSim(pdp);

    for (int i = 0; i < pdp.getNumChannels(); i++) {
      sim.setCurrent(i, 24 - i);
    }
    double[] currents = pdp.getAllCurrents();
    for (int i = 0; i < pdp.getNumChannels(); i++) {
      assertEquals(24 - i, currents[i]);
    }
  }
}
