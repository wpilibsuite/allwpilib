// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.estimator;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;

class DCMotorInputErrorEstimatorTest {
  @Test
  void testPositiveBias() {
    var estimator = new DCMotorInputErrorEstimator(1.0, 0.5, 0.005);
    estimator.reset(1.0, 0.0);
    assertEquals(5.637486730707648, estimator.calculate(2.0, 2.0));
  }
}
