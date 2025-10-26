// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.filter;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.util.WPIUtilJNI;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

class TapCountFilterTest {
  @BeforeEach
  void setUp() {
    WPIUtilJNI.enableMockTime();
    WPIUtilJNI.setMockTime(0L);
  }

  @AfterEach
  void tearDown() {
    WPIUtilJNI.setMockTime(0L);
    WPIUtilJNI.disableMockTime();
  }

  @Test
  void tapCountFilterActivatedTest() {
    var filter = new TapCountFilter(2, 0.2);

    assertFalse(filter.calculate(true)); // First tap

    WPIUtilJNI.setMockTime(50_000L);
    assertFalse(filter.calculate(false)); // First tap ended

    WPIUtilJNI.setMockTime(100_000L);
    assertTrue(filter.calculate(true)); // Second tap

    WPIUtilJNI.setMockTime(150_000L);
    assertTrue(filter.calculate(true)); // Still true

    WPIUtilJNI.setMockTime(250_000L);
    assertTrue(filter.calculate(true)); // Still true

    WPIUtilJNI.setMockTime(300_000L);
    assertFalse(filter.calculate(false)); // Input false, should reset
  }

  @Test
  void tapCountFilterExpiredTest() {
    var filter = new TapCountFilter(2, 0.2);

    assertFalse(filter.calculate(true)); // First tap

    WPIUtilJNI.setMockTime(50_000L);
    filter.calculate(false); // First tap ended

    WPIUtilJNI.setMockTime(250_000L);
    assertFalse(filter.calculate(true)); // Second tap after window expired

    WPIUtilJNI.setMockTime(300_000L);
    assertFalse(filter.calculate(true)); // Still false
  }

  @Test
  void tapCountFilterParamsTest() {
    var filter = new TapCountFilter(2, 0.2);

    assertEquals(filter.getRequiredTaps(), 2);
    assertEquals(filter.getTapWindowSeconds(), 0.2);

    filter.setRequiredTaps(3);

    assertEquals(filter.getRequiredTaps(), 3);

    filter.setTapWindowSeconds(0.5);

    assertEquals(filter.getTapWindowSeconds(), 0.5);
  }
}
