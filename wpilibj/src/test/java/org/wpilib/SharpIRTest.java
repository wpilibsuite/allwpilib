// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.wpilib.simulation.SharpIRSim;
import org.junit.jupiter.api.Test;

class SharpIRTest {
  @Test
  void testSharpIR() {
    try (SharpIR s = SharpIR.GP2Y0A02YK0F(1)) {
      SharpIRSim sim = new SharpIRSim(s);

      assertEquals(0.2, s.getRange());

      sim.setRange(0.3);
      assertEquals(0.3, s.getRange());

      sim.setRange(3);
      assertEquals(1.5, s.getRange());
    }
  }
}
