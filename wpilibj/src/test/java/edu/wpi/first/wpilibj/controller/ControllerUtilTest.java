/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.controller;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;

class ControllerUtilTest {
  @Test
  void testGetModulusError() {
    // Test symmetric range
    assertEquals(-20.0, ControllerUtil.getModulusError(170.0, -170.0, -180.0, 180.0));
    assertEquals(-20.0, ControllerUtil.getModulusError(170.0 + 360.0, -170.0, -180.0, 180.0));
    assertEquals(-20.0, ControllerUtil.getModulusError(170.0, -170.0 + 360.0, -180.0, 180.0));
    assertEquals(20.0, ControllerUtil.getModulusError(-170.0, 170.0, -180.0, 180.0));
    assertEquals(20.0, ControllerUtil.getModulusError(-170.0 + 360.0, 170.0, -180.0, 180.0));
    assertEquals(20.0, ControllerUtil.getModulusError(-170.0, 170.0 + 360.0, -180.0, 180.0));

    // Test range start at zero
    assertEquals(-20.0, ControllerUtil.getModulusError(170.0, 190.0, 0.0, 360.0));
    assertEquals(-20.0, ControllerUtil.getModulusError(170.0 + 360.0, 190.0, 0.0, 360.0));
    assertEquals(-20.0, ControllerUtil.getModulusError(170.0, 190.0 + 360, 0.0, 360.0));

    // Test asymmetric range that doesn't start at zero
    assertEquals(-20.0, ControllerUtil.getModulusError(170.0, -170.0, -170.0, 190.0));
  }
}
