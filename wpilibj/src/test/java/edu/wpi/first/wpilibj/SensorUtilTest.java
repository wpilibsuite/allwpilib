// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;

class SensorUtilTest {
  @Test
  void testgetDefaultCtrePcmModule() {
    assertEquals(0, SensorUtil.getDefaultCTREPCMModule());
  }
}
