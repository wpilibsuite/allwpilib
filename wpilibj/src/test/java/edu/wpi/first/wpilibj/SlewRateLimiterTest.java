/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

public class SlewRateLimiterTest {
  @Test
  void slewRateLimitTest() {
    SlewRateLimiter limiter = new SlewRateLimiter(1);
    Timer.delay(1);
    assertTrue(limiter.calculate(2) < 2);
  }

  @Test
  void slewRateNoLimitTest() {
    SlewRateLimiter limiter = new SlewRateLimiter(1);
    Timer.delay(1);
    assertEquals(limiter.calculate(0.5), 0.5);
  }
}
