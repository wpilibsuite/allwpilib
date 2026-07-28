// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.units;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;

class UnitTest { // :)
  @Test
  void testOf() {
    ExampleUnit u = new ExampleUnit(1);
    Measure<ExampleUnit> fiveOfSomething = u.of(5);
    assertEquals(5, fiveOfSomething.magnitude(), 0);
    assertEquals(u, fiveOfSomething.unit());
  }
}
