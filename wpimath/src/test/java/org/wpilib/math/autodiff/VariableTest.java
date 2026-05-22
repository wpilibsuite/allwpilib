// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.autodiff;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;

class VariableTest {
  @Test
  void testDefaultConstructor() {
    assertEquals(0, Variable.totalNativeMemoryUsage());

    try (var a = new Variable()) {
      assertEquals(0.0, a.value());
      assertEquals(ExpressionType.LINEAR, a.type());
    }

    assertEquals(0, Variable.totalNativeMemoryUsage());
  }

  @Test
  void testConstantConstructor() {
    assertEquals(0, Variable.totalNativeMemoryUsage());

    try (var pool = new VariablePool()) {
      // float
      var a = new Variable(1.0);
      assertEquals(1, a.value());
      assertEquals(ExpressionType.CONSTANT, a.type());

      // int
      var b = new Variable(2);
      assertEquals(2, b.value());
      assertEquals(ExpressionType.CONSTANT, b.type());
    }

    assertEquals(0, Variable.totalNativeMemoryUsage());
  }

  @Test
  void testSetValue() {
    assertEquals(0, Variable.totalNativeMemoryUsage());

    try (var a = new Variable()) {
      a.setValue(1.0);
      assertEquals(1.0, a.value());

      a.setValue(2.0);
      assertEquals(2.0, a.value());
    }

    assertEquals(0, Variable.totalNativeMemoryUsage());
  }
}
