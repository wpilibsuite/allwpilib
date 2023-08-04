// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertThrows;

import org.junit.jupiter.api.Test;

class SensorUtilTest {
  @Test
  void checkAnalogInputChannel() {
    assertThrows(IllegalArgumentException.class, () -> SensorUtil.checkAnalogInputChannel(100));
  }

  @Test
  void checkAnalogOutputChannel() {
    assertThrows(IllegalArgumentException.class, () -> SensorUtil.checkAnalogOutputChannel(100));
  }

  @Test
  void testInvalidDigitalChannel() {
    assertThrows(IllegalArgumentException.class, () -> SensorUtil.checkDigitalChannel(100));
  }

  @Test
  void testInvalidPwmChannel() {
    assertThrows(IllegalArgumentException.class, () -> SensorUtil.checkPWMChannel(100));
  }

  @Test
  void testInvalidRelayModule() {
    assertThrows(IllegalArgumentException.class, () -> SensorUtil.checkRelayChannel(100));
  }

  @Test
  void testgetDefaultCtrePcmModule() {
    assertEquals(0, SensorUtil.getDefaultCTREPCMModule());
  }
}
