// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.controller;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;

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
