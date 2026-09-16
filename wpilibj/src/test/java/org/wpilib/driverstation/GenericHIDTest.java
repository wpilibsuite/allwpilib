// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.driverstation;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;
import org.wpilib.driverstation.GenericHID.RumbleType;
import org.wpilib.simulation.DriverStationSim;
import org.wpilib.simulation.GenericHIDSim;

class GenericHIDTest {
  private static final double EPSILON = 0.0001;

  @Test
  void testRumbleRange() {
    GenericHID hid = new GenericHID(0);
    GenericHIDSim sim = new GenericHIDSim(0);

    for (int i = 0; i <= 100; i++) {
      double rumbleValue = i / 100.0;
      hid.setRumble(RumbleType.LEFT_RUMBLE, rumbleValue);
      assertEquals(rumbleValue, sim.getRumble(RumbleType.LEFT_RUMBLE), EPSILON);

      hid.setRumble(RumbleType.RIGHT_RUMBLE, rumbleValue);
      assertEquals(rumbleValue, sim.getRumble(RumbleType.RIGHT_RUMBLE), EPSILON);

      hid.setRumble(RumbleType.LEFT_TRIGGER_RUMBLE, rumbleValue);
      assertEquals(rumbleValue, sim.getRumble(RumbleType.LEFT_TRIGGER_RUMBLE), EPSILON);

      hid.setRumble(RumbleType.RIGHT_TRIGGER_RUMBLE, rumbleValue);
      assertEquals(rumbleValue, sim.getRumble(RumbleType.RIGHT_TRIGGER_RUMBLE), EPSILON);
    }
  }

  @Test
  void testRawRumbleRange() {
    GenericHID hid = new GenericHID(0);

    hid.setRawRumble(RumbleType.LEFT_RUMBLE, 0x1234);
    assertEquals(0x1234, DriverStationSim.getJoystickRumble(0, 0));

    hid.setRawRumble(RumbleType.RIGHT_RUMBLE, 0x5678);
    assertEquals(0x5678, DriverStationSim.getJoystickRumble(0, 1));

    hid.setRawRumble(RumbleType.LEFT_TRIGGER_RUMBLE, 0x9ABC);
    assertEquals(0x9ABC, DriverStationSim.getJoystickRumble(0, 2));

    hid.setRawRumble(RumbleType.RIGHT_TRIGGER_RUMBLE, 0xDEF0);
    assertEquals(0xDEF0, DriverStationSim.getJoystickRumble(0, 3));

    hid.setRawRumble(RumbleType.LEFT_RUMBLE, -1);
    assertEquals(0, DriverStationSim.getJoystickRumble(0, 0));

    hid.setRawRumble(RumbleType.LEFT_RUMBLE, 65536);
    assertEquals(65535, DriverStationSim.getJoystickRumble(0, 0));
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
    assertEquals(0, sim.getRumble(RumbleType.LEFT_RUMBLE), EPSILON);
    assertEquals(0, sim.getRumble(RumbleType.RIGHT_RUMBLE), EPSILON);
    assertEquals(0, sim.getRumble(RumbleType.LEFT_TRIGGER_RUMBLE), EPSILON);
    assertEquals(0, sim.getRumble(RumbleType.RIGHT_TRIGGER_RUMBLE), EPSILON);

    // test left only
    hid.setRumble(RumbleType.LEFT_RUMBLE, 1);
    assertEquals(1, sim.getRumble(RumbleType.LEFT_RUMBLE), EPSILON);
    assertEquals(0, sim.getRumble(RumbleType.RIGHT_RUMBLE), EPSILON);
    assertEquals(0, sim.getRumble(RumbleType.LEFT_TRIGGER_RUMBLE), EPSILON);
    assertEquals(0, sim.getRumble(RumbleType.RIGHT_TRIGGER_RUMBLE), EPSILON);
    hid.setRumble(RumbleType.LEFT_RUMBLE, 0);

    // test right only
    hid.setRumble(RumbleType.RIGHT_RUMBLE, 1);
    assertEquals(0, sim.getRumble(RumbleType.LEFT_RUMBLE), EPSILON);
    assertEquals(1, sim.getRumble(RumbleType.RIGHT_RUMBLE), EPSILON);
    assertEquals(0, sim.getRumble(RumbleType.LEFT_TRIGGER_RUMBLE), EPSILON);
    assertEquals(0, sim.getRumble(RumbleType.RIGHT_TRIGGER_RUMBLE), EPSILON);
    hid.setRumble(RumbleType.RIGHT_RUMBLE, 0);

    // test left trigger only
    hid.setRumble(RumbleType.LEFT_TRIGGER_RUMBLE, 1);
    assertEquals(0, sim.getRumble(RumbleType.LEFT_RUMBLE), EPSILON);
    assertEquals(0, sim.getRumble(RumbleType.RIGHT_RUMBLE), EPSILON);
    assertEquals(1, sim.getRumble(RumbleType.LEFT_TRIGGER_RUMBLE), EPSILON);
    assertEquals(0, sim.getRumble(RumbleType.RIGHT_TRIGGER_RUMBLE), EPSILON);
    hid.setRumble(RumbleType.LEFT_TRIGGER_RUMBLE, 0);

    // test right trigger only
    hid.setRumble(RumbleType.RIGHT_TRIGGER_RUMBLE, 1);
    assertEquals(0, sim.getRumble(RumbleType.LEFT_RUMBLE), EPSILON);
    assertEquals(0, sim.getRumble(RumbleType.RIGHT_RUMBLE), EPSILON);
    assertEquals(0, sim.getRumble(RumbleType.LEFT_TRIGGER_RUMBLE), EPSILON);
    assertEquals(1, sim.getRumble(RumbleType.RIGHT_TRIGGER_RUMBLE), EPSILON);
    hid.setRumble(RumbleType.RIGHT_TRIGGER_RUMBLE, 0);
  }
}
