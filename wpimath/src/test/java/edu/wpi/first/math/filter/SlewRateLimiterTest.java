// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.filter;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.util.WPIUtilJNI;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

class SlewRateLimiterTest {
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
  void slewRateLimitTest() {
    var limiter = new SlewRateLimiter(1);
    WPIUtilJNI.setMockTime(1000000L);
    assertTrue(limiter.calculate(2) < 2);
  }

  @Test
  void slewRateNoLimitTest() {
    var limiter = new SlewRateLimiter(1);
    WPIUtilJNI.setMockTime(1000000L);
    assertEquals(limiter.calculate(0.5), 0.5);
  }

  @Test
  void slewRatePositiveNegativeTest() {
    var limiter = new SlewRateLimiter(1, -0.5, 0);
    WPIUtilJNI.setMockTime(1000000L);
    assertEquals(limiter.calculate(2), 1);
    WPIUtilJNI.setMockTime(2000000L);
    assertEquals(limiter.calculate(0), 0.5);
  }
}
