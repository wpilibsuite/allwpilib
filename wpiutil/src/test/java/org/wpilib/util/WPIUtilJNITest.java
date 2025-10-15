// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util;

import static org.junit.jupiter.api.Assertions.assertDoesNotThrow;

import org.junit.jupiter.api.Test;

public class WPIUtilJNITest {
  @Test
  public void testEnableMockTime() {
    assertDoesNotThrow(WPIUtilJNI::enableMockTime);
  }

  @Test
  public void testSetMockTime() {
    assertDoesNotThrow(() -> WPIUtilJNI.setMockTime(0L));
  }

  @Test
  public void testNow() {
    assertDoesNotThrow(WPIUtilJNI::now);
  }
}
