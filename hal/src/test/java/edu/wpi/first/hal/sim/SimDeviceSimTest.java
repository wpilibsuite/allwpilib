/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.hal.sim;

import org.junit.jupiter.api.Test;

import edu.wpi.first.hal.SimBoolean;
import edu.wpi.first.hal.SimDevice;

import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

class SimDeviceSimTest {
  @Test
  void testBasic() {
    SimDevice dev = SimDevice.create("test");
    SimBoolean devBool = dev.createBoolean("bool", false, false);

    SimDeviceSim sim = new SimDeviceSim("test");
    SimBoolean simBool = sim.getBoolean("bool");

    assertFalse(simBool.get());
    simBool.set(true);
    assertTrue(devBool.get());
  }
}
