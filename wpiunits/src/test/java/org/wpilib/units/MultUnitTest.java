// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertSame;

import org.junit.jupiter.api.Test;

class MultUnitTest {
  @Test
  void testAutomaticNames() {
    var unitA = new ExampleUnit(1, "Ay", "a");
    var unitB = new ExampleUnit(1, "Bee", "b");
    var mult = MultUnit.combine(unitA, unitB);
    assertEquals("Ay-Bee", mult.name());
    assertEquals("a*b", mult.symbol());
  }

  @Test
  void testCombine() {
    var unitA = new ExampleUnit(100);
    var unitB = new ExampleUnit(0.912);
    var mult = MultUnit.combine(unitA, unitB);
    assertEquals(91.2, mult.toBaseUnits(1));
  }

  @Test
  void testCaches() {
    var unitA = new ExampleUnit(1);
    var unitB = new ExampleUnit(2);
    var mult = MultUnit.combine(unitA, unitB);
    assertSame(mult, MultUnit.combine(unitA, unitB));
  }
}
