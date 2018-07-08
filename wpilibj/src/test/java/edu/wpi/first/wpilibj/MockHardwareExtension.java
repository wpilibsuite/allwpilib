/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import org.junit.jupiter.api.extension.BeforeAllCallback;
import org.junit.jupiter.api.extension.ExtensionContext;
import org.junit.jupiter.api.extension.ExtensionContext.Namespace;

import edu.wpi.first.wpilibj.hal.HAL;
import edu.wpi.first.wpilibj.sim.DriverStationSim;

public final class MockHardwareExtension implements BeforeAllCallback {
  private static ExtensionContext getRoot(ExtensionContext context) {
    return context.getParent().map(MockHardwareExtension::getRoot).orElse(context);
  }

  @Override
  public void beforeAll(ExtensionContext context) throws Exception {
    getRoot(context).getStore(Namespace.GLOBAL).getOrComputeIfAbsent("HAL Initalized", key -> {
      initializeHardware();
      return true;
    }, Boolean.class);
  }

  private void initializeHardware() {
    HAL.initialize(500, 0);
    DriverStationSim dsSim = new DriverStationSim();
    dsSim.setDsAttached(true);
    dsSim.setAutonomous(false);
    dsSim.setEnabled(true);
    dsSim.setTest(true);


  }
}
