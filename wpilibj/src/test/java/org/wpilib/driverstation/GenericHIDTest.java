// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.driverstation;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;
import org.wpilib.driverstation.GenericHID.RumbleType;
import org.wpilib.simulation.GenericHIDSim;

class GenericHIDTest {
  private static final double kEpsilon = 0.0001;

  @Test
  void testRumbleRange() {
    GenericHID hid = new GenericHID(0);
    GenericHIDSim sim = new GenericHIDSim(0);

    for (int i = 0; i <= 100; i++) {
      double rumbleValue = i / 100.0;
      hid.setRumble(RumbleType.kLeftRumble, rumbleValue);
      assertEquals(rumbleValue, sim.getRumble(RumbleType.kLeftRumble), kEpsilon);

      hid.setRumble(RumbleType.kRightRumble, rumbleValue);
      assertEquals(rumbleValue, sim.getRumble(RumbleType.kRightRumble), kEpsilon);

      hid.setRumble(RumbleType.kLeftTriggerRumble, rumbleValue);
      assertEquals(rumbleValue, sim.getRumble(RumbleType.kLeftTriggerRumble), kEpsilon);

      hid.setRumble(RumbleType.kRightTriggerRumble, rumbleValue);
      assertEquals(rumbleValue, sim.getRumble(RumbleType.kRightTriggerRumble), kEpsilon);
    }
  }

  @Test
  void testRumbleTypes() {
    GenericHID hid = new GenericHID(0);
    final GenericHIDSim sim = new GenericHIDSim(0);

    // Make sure all are off
    hid.setRumble(RumbleType.kLeftRumble, 0);
    hid.setRumble(RumbleType.kRightRumble, 0);
    hid.setRumble(RumbleType.kLeftTriggerRumble, 0);
    hid.setRumble(RumbleType.kRightTriggerRumble, 0);
    assertEquals(0, sim.getRumble(RumbleType.kLeftRumble), kEpsilon);
    assertEquals(0, sim.getRumble(RumbleType.kRightRumble), kEpsilon);
    assertEquals(0, sim.getRumble(RumbleType.kLeftTriggerRumble), kEpsilon);
    assertEquals(0, sim.getRumble(RumbleType.kRightTriggerRumble), kEpsilon);

    // test left only
    hid.setRumble(RumbleType.kLeftRumble, 1);
    assertEquals(1, sim.getRumble(RumbleType.kLeftRumble), kEpsilon);
    assertEquals(0, sim.getRumble(RumbleType.kRightRumble), kEpsilon);
    assertEquals(0, sim.getRumble(RumbleType.kLeftTriggerRumble), kEpsilon);
    assertEquals(0, sim.getRumble(RumbleType.kRightTriggerRumble), kEpsilon);
    hid.setRumble(RumbleType.kLeftRumble, 0);

    // test right only
    hid.setRumble(RumbleType.kRightRumble, 1);
    assertEquals(0, sim.getRumble(RumbleType.kLeftRumble), kEpsilon);
    assertEquals(1, sim.getRumble(RumbleType.kRightRumble), kEpsilon);
    assertEquals(0, sim.getRumble(RumbleType.kLeftTriggerRumble), kEpsilon);
    assertEquals(0, sim.getRumble(RumbleType.kRightTriggerRumble), kEpsilon);
    hid.setRumble(RumbleType.kRightRumble, 0);

    // test left trigger only
    hid.setRumble(RumbleType.kLeftTriggerRumble, 1);
    assertEquals(0, sim.getRumble(RumbleType.kLeftRumble), kEpsilon);
    assertEquals(0, sim.getRumble(RumbleType.kRightRumble), kEpsilon);
    assertEquals(1, sim.getRumble(RumbleType.kLeftTriggerRumble), kEpsilon);
    assertEquals(0, sim.getRumble(RumbleType.kRightTriggerRumble), kEpsilon);
    hid.setRumble(RumbleType.kLeftTriggerRumble, 0);

    // test right trigger only
    hid.setRumble(RumbleType.kRightTriggerRumble, 1);
    assertEquals(0, sim.getRumble(RumbleType.kLeftRumble), kEpsilon);
    assertEquals(0, sim.getRumble(RumbleType.kRightRumble), kEpsilon);
    assertEquals(0, sim.getRumble(RumbleType.kLeftTriggerRumble), kEpsilon);
    assertEquals(1, sim.getRumble(RumbleType.kRightTriggerRumble), kEpsilon);
    hid.setRumble(RumbleType.kRightTriggerRumble, 0);
  }
}
