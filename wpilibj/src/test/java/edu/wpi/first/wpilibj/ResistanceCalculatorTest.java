// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;

class ResistanceCalculatorTest {
  @Test
  void edgeCaseTest() {
    var calc = new ResistanceCalculator();

    // Make sure it doesn't try to do a linear regression with only 1 point
    assertEquals(Double.NaN, calc.calculate(1, 1));

    // Make sure points with current 0 are ignored
    assertEquals(Double.NaN, calc.calculate(0, 1));
    assertEquals(Double.NaN, calc.calculate(0, 1));
  }

  @Test
  void resistanceCalculationTest() {
    var tolerance = 0.5;
    var calc = new ResistanceCalculator();

    calc.calculate(1, 1);
    assertEquals(-1.1282, calc.calculate(40, 45), tolerance);
    assertEquals(-1.0361, calc.calculate(50, 50), tolerance);
    assertEquals(-0.7832, calc.calculate(60, 40), tolerance);
    // r^2 should be too low here
    assertEquals(Double.NaN, calc.calculate(100, 0));
  }
}
