// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.autodiff;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;

class VariableTest {
  @Test
  void testDefaultConstructor() {
    var a = new Variable();

    assertEquals(0.0, a.value());
    assertEquals(ExpressionType.LINEAR, a.type());
  }
}
