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
      hid.setRumble(RumbleType.LEFT_RUMBLE, rumbleValue);
      assertEquals(rumbleValue, sim.getRumble(RumbleType.LEFT_RUMBLE), kEpsilon);

      hid.setRumble(RumbleType.RIGHT_RUMBLE, rumbleValue);
      assertEquals(rumbleValue, sim.getRumble(RumbleType.RIGHT_RUMBLE), kEpsilon);

      hid.setRumble(RumbleType.LEFT_TRIGGER_RUMBLE, rumbleValue);
      assertEquals(rumbleValue, sim.getRumble(RumbleType.LEFT_TRIGGER_RUMBLE), kEpsilon);

      hid.setRumble(RumbleType.RIGHT_TRIGGER_RUMBLE, rumbleValue);
      assertEquals(rumbleValue, sim.getRumble(RumbleType.RIGHT_TRIGGER_RUMBLE), kEpsilon);
    }
  }

  @Test
  void testRumbleTypes() {
    GenericHID hid = new GenericHID(0);
    final GenericHIDSim sim = new GenericHIDSim(0);

    // Make sure all are off
    hid.setRumble(RumbleType.LEFT_RUMBLE, 0);
    hid.setRumble(RumbleType.RIGHT_RUMBLE, 0);
    hid.setRumble(RumbleType.LEFT_TRIGGER_RUMBLE, 0);
    hid.setRumble(RumbleType.RIGHT_TRIGGER_RUMBLE, 0);
    assertEquals(0, sim.getRumble(RumbleType.LEFT_RUMBLE), kEpsilon);
    assertEquals(0, sim.getRumble(RumbleType.RIGHT_RUMBLE), kEpsilon);
    assertEquals(0, sim.getRumble(RumbleType.LEFT_TRIGGER_RUMBLE), kEpsilon);
    assertEquals(0, sim.getRumble(RumbleType.RIGHT_TRIGGER_RUMBLE), kEpsilon);

    // test left only
    hid.setRumble(RumbleType.LEFT_RUMBLE, 1);
    assertEquals(1, sim.getRumble(RumbleType.LEFT_RUMBLE), kEpsilon);
    assertEquals(0, sim.getRumble(RumbleType.RIGHT_RUMBLE), kEpsilon);
    assertEquals(0, sim.getRumble(RumbleType.LEFT_TRIGGER_RUMBLE), kEpsilon);
    assertEquals(0, sim.getRumble(RumbleType.RIGHT_TRIGGER_RUMBLE), kEpsilon);
    hid.setRumble(RumbleType.LEFT_RUMBLE, 0);

    // test right only
    hid.setRumble(RumbleType.RIGHT_RUMBLE, 1);
    assertEquals(0, sim.getRumble(RumbleType.LEFT_RUMBLE), kEpsilon);
    assertEquals(1, sim.getRumble(RumbleType.RIGHT_RUMBLE), kEpsilon);
    assertEquals(0, sim.getRumble(RumbleType.LEFT_TRIGGER_RUMBLE), kEpsilon);
    assertEquals(0, sim.getRumble(RumbleType.RIGHT_TRIGGER_RUMBLE), kEpsilon);
    hid.setRumble(RumbleType.RIGHT_RUMBLE, 0);

    // test left trigger only
    hid.setRumble(RumbleType.LEFT_TRIGGER_RUMBLE, 1);
    assertEquals(0, sim.getRumble(RumbleType.LEFT_RUMBLE), kEpsilon);
    assertEquals(0, sim.getRumble(RumbleType.RIGHT_RUMBLE), kEpsilon);
    assertEquals(1, sim.getRumble(RumbleType.LEFT_TRIGGER_RUMBLE), kEpsilon);
    assertEquals(0, sim.getRumble(RumbleType.RIGHT_TRIGGER_RUMBLE), kEpsilon);
    hid.setRumble(RumbleType.LEFT_TRIGGER_RUMBLE, 0);

    // test right trigger only
    hid.setRumble(RumbleType.RIGHT_TRIGGER_RUMBLE, 1);
    assertEquals(0, sim.getRumble(RumbleType.LEFT_RUMBLE), kEpsilon);
    assertEquals(0, sim.getRumble(RumbleType.RIGHT_RUMBLE), kEpsilon);
    assertEquals(0, sim.getRumble(RumbleType.LEFT_TRIGGER_RUMBLE), kEpsilon);
    assertEquals(1, sim.getRumble(RumbleType.RIGHT_TRIGGER_RUMBLE), kEpsilon);
    hid.setRumble(RumbleType.RIGHT_TRIGGER_RUMBLE, 0);
  }
}
