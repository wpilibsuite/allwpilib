// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.util;

import static org.junit.jupiter.api.Assertions.assertDoesNotThrow;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

public class WPIUtilJNITest {
  @BeforeEach
  public void disableMockTimeBeforeEach() {
    WPIUtilJNI.disableMockTime();
  }

  @AfterEach
  public void disableMockTimeAfterEach() {
    WPIUtilJNI.disableMockTime();
  }

  @Test
  public void testEnableMockTimeResetsMockAndProgramStartTime() {
    WPIUtilJNI.setMockTime(1234L);

    WPIUtilJNI.enableMockTime();

    assertEquals(0L, WPIUtilJNI.now());
    assertEquals(0L, WPIUtilJNI.getProgramStartTime());
  }

  @Test
  public void testSetMockTimeDoesNotChangeProgramStartTime() {
    WPIUtilJNI.enableMockTime();

    WPIUtilJNI.setMockTime(1234L);

    assertEquals(1234L, WPIUtilJNI.now());
    assertEquals(0L, WPIUtilJNI.getProgramStartTime());
  }

  @Test
  public void testNow() {
    assertDoesNotThrow(WPIUtilJNI::now);
  }

  @Test
  public void testGetProgramStartTime() {
    long startTime = WPIUtilJNI.getProgramStartTime();

    assertEquals(startTime, WPIUtilJNI.getProgramStartTime());
  }

  @Test
  public void testDisableMockTimeRestoresProgramStartTime() {
    long startTime = WPIUtilJNI.getProgramStartTime();

    WPIUtilJNI.enableMockTime();
    assertEquals(0L, WPIUtilJNI.getProgramStartTime());

    WPIUtilJNI.disableMockTime();

    assertEquals(startTime, WPIUtilJNI.getProgramStartTime());
    assertTrue(WPIUtilJNI.now() >= startTime);
  }
}
