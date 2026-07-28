// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.autodiff;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.wpilib.math.MatrixAssertions.assertEquals;

import org.ejml.simple.SimpleMatrix;
import org.junit.jupiter.api.Test;

class JacobianTest {
  @Test
  void testYEqualsX() {
    assertEquals(0, Variable.totalNativeMemoryUsage());

    try (var pool = new VariablePool()) {
      var x = new VariableMatrix(3);
      for (int i = 0; i < 3; ++i) {
        x.get(i).setValue(i + 1);
      }

      // y = x
      //
      //         [1  0  0]
      // dy/dx = [0  1  0]
      //         [0  0  1]
      var y = x;
      var J = new Jacobian(y, x);

      var expected_J =
          new SimpleMatrix(new double[][] {{1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}});
      assertEquals(expected_J, J.get().value());
      assertEquals(expected_J, J.value());

      J.close();
    }

    assertEquals(0, Variable.totalNativeMemoryUsage());
  }

  @Test
  void testYEquals3X() {
    assertEquals(0, Variable.totalNativeMemoryUsage());

    try (var pool = new VariablePool()) {
      var x = new VariableMatrix(3);
      for (int i = 0; i < 3; ++i) {
        x.get(i).setValue(i + 1);
      }

      // y = 3x
      //
      //         [3  0  0]
      // dy/dx = [0  3  0]
      //         [0  0  3]
      var y = x.times(3);
      var J = new Jacobian(y, x);

      var expected_J =
          new SimpleMatrix(new double[][] {{3.0, 0.0, 0.0}, {0.0, 3.0, 0.0}, {0.0, 0.0, 3.0}});
      assertEquals(expected_J, J.get().value());
      assertEquals(expected_J, J.value());

      J.close();
    }

    assertEquals(0, Variable.totalNativeMemoryUsage());
  }

  @Test
  void testProducts() {
    assertEquals(0, Variable.totalNativeMemoryUsage());

    try (var pool = new VariablePool()) {
      var x = new VariableMatrix(3);
      for (int i = 0; i < 3; ++i) {
        x.get(i).setValue(i + 1);
      }

      //     [x₁x₂]
      // y = [x₂x₃]
      //     [x₁x₃]
      //
      //         [x₂  x₁  0 ]
      // dy/dx = [0   x₃  x₂]
      //         [x₃  0   x₁]
      //
      //         [2  1  0]
      // dy/dx = [0  3  2]
      //         [3  0  1]
      var y = new VariableMatrix(3);
      y.set(0, x.get(0).times(x.get(1)));
      y.set(1, x.get(1).times(x.get(2)));
      y.set(2, x.get(0).times(x.get(2)));
      var J = new Jacobian(y, x);

      var expected_J =
          new SimpleMatrix(new double[][] {{2.0, 1.0, 0.0}, {0.0, 3.0, 2.0}, {3.0, 0.0, 1.0}});
      assertEquals(expected_J, J.get().value());
      assertEquals(expected_J, J.value());

      J.close();
    }

    assertEquals(0, Variable.totalNativeMemoryUsage());
  }

  @Test
  void testNestedProducts() {
    assertEquals(0, Variable.totalNativeMemoryUsage());

    try (var pool = new VariablePool()) {
      var x = new VariableMatrix(1);
      x.get(0).setValue(3);
      assertEquals(3.0, x.value(0));

      //     [ 5x]   [15]
      // y = [ 7x] = [21]
      //     [11x]   [33]
      var y = new VariableMatrix(3);
      y.set(0, x.get(0).times(5));
      y.set(1, x.get(0).times(7));
      y.set(2, x.get(0).times(11));
      assertEquals(15.0, y.value(0));
      assertEquals(21.0, y.value(1));
      assertEquals(33.0, y.value(2));

      //     [y₁y₂]   [15⋅21]   [315]
      // z = [y₂y₃] = [21⋅33] = [693]
      //     [y₁y₃]   [15⋅33]   [495]
      var z = new VariableMatrix(3);
      z.set(0, y.get(0).times(y.get(1)));
      z.set(1, y.get(1).times(y.get(2)));
      z.set(2, y.get(0).times(y.get(2)));
      assertEquals(315.0, z.value(0));
      assertEquals(693.0, z.value(1));
      assertEquals(495.0, z.value(2));

      //     [ 5x]
      // y = [ 7x]
      //     [11x]
      //
      //         [ 5]
      // dy/dx = [ 7]
      //         [11]
      var J = new Jacobian(y, x);
      assertEquals(5.0, J.get().value(0, 0));
      assertEquals(7.0, J.get().value(1, 0));
      assertEquals(11.0, J.get().value(2, 0));
      assertEquals(5.0, J.value().get(0, 0));
      assertEquals(7.0, J.value().get(1, 0));
      assertEquals(11.0, J.value().get(2, 0));

      //     [y₁y₂]
      // z = [y₂y₃]
      //     [y₁y₃]
      //
      //         [y₂  y₁  0 ]   [21  15   0]
      // dz/dy = [0   y₃  y₂] = [ 0  33  21]
      //         [y₃  0   y₁]   [33   0  15]
      J.close();
      J = new Jacobian(z, y);
      var expected_J =
          new SimpleMatrix(
              new double[][] {{21.0, 15.0, 0.0}, {0.0, 33.0, 21.0}, {33.0, 0.0, 15.0}});
      assertEquals(expected_J, J.get().value());
      assertEquals(expected_J, J.value());

      //     [y₁y₂]   [5x⋅ 7x]   [35x²]
      // z = [y₂y₃] = [7x⋅11x] = [77x²]
      //     [y₁y₃]   [5x⋅11x]   [55x²]
      //
      //         [ 70x]   [210]
      // dz/dx = [154x] = [462]
      //         [110x] = [330]
      J.close();
      J = new Jacobian(z, x);
      expected_J = new SimpleMatrix(new double[][] {{210.0}, {462.0}, {330.0}});
      assertEquals(expected_J, J.get().value());
      assertEquals(expected_J, J.value());

      J.close();
    }

    assertEquals(0, Variable.totalNativeMemoryUsage());
  }

  @Test
  void testNonSquare() {
    assertEquals(0, Variable.totalNativeMemoryUsage());

    try (var pool = new VariablePool()) {
      var x = new VariableMatrix(3);
      for (int i = 0; i < 3; ++i) {
        x.get(i).setValue(i + 1);
      }

      // y = [x₁ + 3x₂ − 5x₃]
      //
      // dy/dx = [1  3  −5]
      var y = new VariableMatrix(1);
      y.set(0, x.get(0).plus(x.get(1).times(3)).minus(x.get(2).times(5)));
      var J = new Jacobian(y, x);

      var expected_J = new SimpleMatrix(new double[][] {{1.0, 3.0, -5.0}});

      var J_get_value = J.get().value();
      assertEquals(1, J_get_value.getNumRows());
      assertEquals(3, J_get_value.getNumCols());
      assertEquals(expected_J, J_get_value);

      var J_value = J.value();
      assertEquals(1, J_value.getNumRows());
      assertEquals(3, J_value.getNumCols());
      assertEquals(expected_J, J_value);

      J.close();
    }

    assertEquals(0, Variable.totalNativeMemoryUsage());
  }

  @Test
  void testVariableReuse() {
    assertEquals(0, Variable.totalNativeMemoryUsage());

    try (var pool = new VariablePool()) {
      var x = new VariableMatrix(2);
      for (int i = 0; i < 2; ++i) {
        x.get(i).setValue(i + 1);
      }

      // y = [x₁x₂]
      var y = new VariableMatrix(1);
      y.set(0, x.get(0).times(x.get(1)));

      var jacobian = new Jacobian(y, x);

      // dy/dx = [x₂  x₁]
      // dy/dx = [2  1]
      var J = jacobian.value();

      assertEquals(1, J.getNumRows());
      assertEquals(2, J.getNumCols());
      assertEquals(2.0, J.get(0, 0));
      assertEquals(1.0, J.get(0, 1));

      x.get(0).setValue(2);
      x.get(1).setValue(1);
      // dy/dx = [x₂  x₁]
      // dy/dx = [1  2]
      J = jacobian.value();

      assertEquals(1, J.getNumRows());
      assertEquals(2, J.getNumCols());
      assertEquals(1.0, J.get(0, 0));
      assertEquals(2.0, J.get(0, 1));

      jacobian.close();
    }

    assertEquals(0, Variable.totalNativeMemoryUsage());
  }
}
