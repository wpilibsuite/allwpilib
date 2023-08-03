// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;

import org.junit.jupiter.api.Test;

class ResistanceCalculatorTest {
  @Test
  void edgeCaseTest() {
    var calc = new ResistanceCalculator();

    // Make sure it doesn't try to do a linear regression with only 1 point
    assertFalse(calc.calculate(1.0, 1.0).isPresent());

    // Make sure points with current 0 are ignored
    assertFalse(calc.calculate(0.0, 1.0).isPresent());
    assertFalse(calc.calculate(0.0, 1.0).isPresent());
  }

  @Test
  void resistanceCalculationTest() {
    final double tolerance = 0.5;
    var calc = new ResistanceCalculator();

    calc.calculate(1, 1);
    assertEquals(1.1282, calc.calculate(40.0, 45.0).getAsDouble(), tolerance);
    assertEquals(1.0361, calc.calculate(50.0, 50.0).getAsDouble(), tolerance);
    assertEquals(0.7832, calc.calculate(60.0, 40.0).getAsDouble(), tolerance);

    // R² should be too low here
    assertFalse(calc.calculate(100.0, 0.0).isPresent());
  }
}
