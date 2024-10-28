// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.wpilibj.GenericHID.RumbleType;
import edu.wpi.first.wpilibj.simulation.GenericHIDSim;
import org.junit.jupiter.api.Test;

class GenericHIDTest {
  private static final double kEpsilon = 0.0001;

  @Test
  void testRumbleRange() {
    GenericHID hid = new GenericHID(0);
    GenericHIDSim sim = new GenericHIDSim(0);

    for (int i = 0; i <= 100; i++) {
      double rumbleValue = i / 100.0;
      hid.setRumble(RumbleType.kBothRumble, rumbleValue);
      assertEquals(rumbleValue, sim.getRumble(RumbleType.kLeftRumble), kEpsilon);
      assertEquals(rumbleValue, sim.getRumble(RumbleType.kRightRumble), kEpsilon);
    }
  }

  @Test
  void testRumbleTypes() {
    GenericHID hid = new GenericHID(0);
    GenericHIDSim sim = new GenericHIDSim(0);

    // Make sure both are off
    hid.setRumble(RumbleType.kBothRumble, 0);
    assertEquals(0, sim.getRumble(RumbleType.kBothRumble), kEpsilon);

    // test both
    hid.setRumble(RumbleType.kBothRumble, 1);
    assertEquals(1, sim.getRumble(RumbleType.kLeftRumble), kEpsilon);
    assertEquals(1, sim.getRumble(RumbleType.kRightRumble), kEpsilon);
    hid.setRumble(RumbleType.kBothRumble, 0);

    // test left only
    hid.setRumble(RumbleType.kLeftRumble, 1);
    assertEquals(1, sim.getRumble(RumbleType.kLeftRumble), kEpsilon);
    assertEquals(0, sim.getRumble(RumbleType.kRightRumble), kEpsilon);
    hid.setRumble(RumbleType.kLeftRumble, 0);

    // test right only
    hid.setRumble(RumbleType.kRightRumble, 1);
    assertEquals(0, sim.getRumble(RumbleType.kLeftRumble), kEpsilon);
    assertEquals(1, sim.getRumble(RumbleType.kRightRumble), kEpsilon);
    hid.setRumble(RumbleType.kRightRumble, 0);
  }
}
