// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.wpilibj.simulation.SharpIRSim;
import org.junit.jupiter.api.Test;

class SharpIRTest {
  @Test
  void testSharpIR() {
    try (SharpIR s = SharpIR.GP2Y0A02YK0F(1)) {
      SharpIRSim sim = new SharpIRSim(s);

      assertEquals(20, s.getRangeCM());

      sim.setRangeCm(30);
      assertEquals(30, s.getRangeCM());

      sim.setRangeCm(300);
      assertEquals(150, s.getRangeCM());
    }
  }
}
