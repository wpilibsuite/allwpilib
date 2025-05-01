// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.wpilibj.GenericHID.RumbleType;
import edu.wpi.first.wpilibj.simulation.GenericHIDSim;
import org.junit.jupiter.api.Test;

class GenericHIDTest {
  private static final double EPSILON = 0.0001;

  @Test
  void testRumbleRange() {
    GenericHID hid = new GenericHID(0);
    GenericHIDSim sim = new GenericHIDSim(0);

    for (int i = 0; i <= 100; i++) {
      double rumbleValue = i / 100.0;
      hid.setRumble(RumbleType.BOTH_RUMBLE, rumbleValue);
      assertEquals(rumbleValue, sim.getRumble(RumbleType.LEFT_RUMBLE), EPSILON);
      assertEquals(rumbleValue, sim.getRumble(RumbleType.RIGHT_RUMBLE), EPSILON);
    }
  }

  @Test
  void testRumbleTypes() {
    GenericHID hid = new GenericHID(0);
    GenericHIDSim sim = new GenericHIDSim(0);

    // Make sure both are off
    hid.setRumble(RumbleType.BOTH_RUMBLE, 0);
    assertEquals(0, sim.getRumble(RumbleType.BOTH_RUMBLE), EPSILON);

    // test both
    hid.setRumble(RumbleType.BOTH_RUMBLE, 1);
    assertEquals(1, sim.getRumble(RumbleType.LEFT_RUMBLE), EPSILON);
    assertEquals(1, sim.getRumble(RumbleType.RIGHT_RUMBLE), EPSILON);
    hid.setRumble(RumbleType.BOTH_RUMBLE, 0);

    // test left only
    hid.setRumble(RumbleType.LEFT_RUMBLE, 1);
    assertEquals(1, sim.getRumble(RumbleType.LEFT_RUMBLE), EPSILON);
    assertEquals(0, sim.getRumble(RumbleType.RIGHT_RUMBLE), EPSILON);
    hid.setRumble(RumbleType.LEFT_RUMBLE, 0);

    // test right only
    hid.setRumble(RumbleType.RIGHT_RUMBLE, 1);
    assertEquals(0, sim.getRumble(RumbleType.LEFT_RUMBLE), EPSILON);
    assertEquals(1, sim.getRumble(RumbleType.RIGHT_RUMBLE), EPSILON);
    hid.setRumble(RumbleType.RIGHT_RUMBLE, 0);
  }
}
