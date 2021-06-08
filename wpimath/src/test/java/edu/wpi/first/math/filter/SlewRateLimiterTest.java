// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.filter;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.wpiutil.WPIUtilJNI;
import org.junit.jupiter.api.Test;

public class SlewRateLimiterTest {
  @Test
  void slewRateLimitTest() {
    WPIUtilJNI.enableMockTime();

    var limiter = new SlewRateLimiter(1);
    WPIUtilJNI.setMockTime(1000000L);
    assertTrue(limiter.calculate(2) < 2);

    WPIUtilJNI.setMockTime(0L);
  }

  @Test
  void slewRateNoLimitTest() {
    WPIUtilJNI.enableMockTime();

    var limiter = new SlewRateLimiter(1);
    WPIUtilJNI.setMockTime(1000000L);
    assertEquals(limiter.calculate(0.5), 0.5);

    WPIUtilJNI.setMockTime(0L);
  }
}
