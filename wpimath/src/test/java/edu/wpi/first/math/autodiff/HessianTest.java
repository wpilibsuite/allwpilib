// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.autodiff;

import static edu.wpi.first.math.MatrixAssertions.assertEquals;
import static edu.wpi.first.math.Range.range;
import static edu.wpi.first.math.autodiff.Variable.pow;
import static edu.wpi.first.math.autodiff.Variable.tan;
import static org.junit.jupiter.api.Assertions.assertEquals;

import org.ejml.simple.SimpleMatrix;
import org.junit.jupiter.api.Test;

class HessianTest {
  @Test
  void testLinear() {
    // y = x
    var x = new VariableMatrix(1);
    x.get(0).setValue(3);
    var y = x.get(0);

    // dy/dx = 1
    double g = new Gradient(y, x.get(0)).value().get(0, 0);
    assertEquals(1.0, g);

    // d²y/dx² = 0
    var H = new Hessian(y, x);
    assertEquals(0.0, H.get().value(0, 0));
    assertEquals(0.0, H.value().get(0, 0));
  }

  @Test
  void testQuadratic() {
    // y = x²
    // y = x * x
    var x = new VariableMatrix(1);
    x.get(0).setValue(3);
    var y = x.get(0).times(x.get(0));

    // dy/dx = x (rhs) + x (lhs)
    //       = (3) + (3)
    //       = 6
    double g = new Gradient(y, x.get(0)).value().get(0, 0);
    assertEquals(6.0, g);

    // d²y/dx² = d/dx(x (rhs) + x (lhs))
    //         = 1 + 1
    //         = 2
    var H = new Hessian(y, x);
    assertEquals(2.0, H.get().value(0, 0));
    assertEquals(2.0, H.value().get(0, 0));
  }

  @Test
  void testSum() {
    var x = new VariableMatrix(5);
    x.get(0).setValue(1);
    x.get(1).setValue(2);
    x.get(2).setValue(3);
    x.get(3).setValue(4);
    x.get(4).setValue(5);

    // y = sum(x)
    var y = x.get(0).plus(x.get(1)).plus(x.get(2)).plus(x.get(3)).plus(x.get(4));
    assertEquals(15.0, y.value());

    var g = new Gradient(y, x);
    assertEquals(SimpleMatrix.filled(5, 1, 1.0), g.get().value());
    assertEquals(SimpleMatrix.filled(5, 1, 1.0), g.value());

    var H = new Hessian(y, x);
    assertEquals(SimpleMatrix.filled(5, 5, 0.0), H.get().value());
    assertEquals(SimpleMatrix.filled(5, 5, 0.0), H.value());
  }

  @Test
  void testSumOfProducts() {
    var x = new VariableMatrix(5);
    x.get(0).setValue(1);
    x.get(1).setValue(2);
    x.get(2).setValue(3);
    x.get(3).setValue(4);
    x.get(4).setValue(5);

    // y = ||x||²
    var y = x.T().times(x).get(0);
    assertEquals(1 + 2 * 2 + 3 * 3 + 4 * 4 + 5 * 5, y.value());

    var g = new Gradient(y, x);
    assertEquals(x.value().scale(2), g.get().value());
    assertEquals(x.value().scale(2), g.value());

    var H = new Hessian(y, x);

    var expected_H = SimpleMatrix.diag(2.0, 2.0, 2.0, 2.0, 2.0);
    assertEquals(expected_H, H.get().value());
    assertEquals(expected_H, H.value());
  }

  @Test
  void testProductOfSines() {
    var x = new VariableMatrix(5);
    x.get(0).setValue(1);
    x.get(1).setValue(2);
    x.get(2).setValue(3);
    x.get(3).setValue(4);
    x.get(4).setValue(5);

    // y = prod(sin(x))
    var temp = x.cwiseTransform(Variable::sin);
    var y = temp.stream().reduce(new Variable(1.0), (a, b) -> a.times(b));
    assertEquals(
        Math.sin(1) * Math.sin(2) * Math.sin(3) * Math.sin(4) * Math.sin(5), y.value(), 1e-15);

    var g = new Gradient(y, x);
    for (int i = 0; i < x.rows(); ++i) {
      assertEquals(y.div(tan(x.get(i))).value(), g.get().value(i), 1e-15);
      assertEquals(y.div(tan(x.get(i))).value(), g.value().get(i, 0), 1e-15);
    }

    var H = new Hessian(y, x);

    var expected_H = new SimpleMatrix(5, 5);
    for (int i = 0; i < x.rows(); ++i) {
      for (int j = 0; j < x.rows(); ++j) {
        if (i == j) {
          expected_H.set(
              i,
              j,
              g.value().get(i, 0)
                  / Math.tan(x.get(i).value())
                  * (1.0 - 1.0 / Math.pow(Math.cos(x.get(i).value()), 2)));
        } else {
          expected_H.set(i, j, g.value().get(j, 0) / Math.tan(x.get(i).value()));
        }
      }
    }

    var actual_H_dense = H.get().value();
    for (int i = 0; i < x.rows(); ++i) {
      for (int j = 0; j < x.rows(); ++j) {
        assertEquals(expected_H.get(i, j), actual_H_dense.get(i, j), 1e-15);
      }
    }

    var actual_H_sparse = H.value();
    for (int i = 0; i < x.rows(); ++i) {
      for (int j = 0; j < x.rows(); ++j) {
        assertEquals(expected_H.get(i, j), actual_H_sparse.get(i, j), 1e-15);
      }
    }
  }

  @Test
  void testSumOfSquaredResiduals() {
    var x = new VariableMatrix(5);
    x.get(0).setValue(1);
    x.get(1).setValue(1);
    x.get(2).setValue(1);
    x.get(3).setValue(1);
    x.get(4).setValue(1);

    // y = sum(diff(x).^2)
    var temp = x.block(0, 0, 4, 1).minus(x.block(1, 0, 4, 1)).cwiseTransform(a -> a.times(a));
    var y = temp.stream().reduce(new Variable(0.0), (a, b) -> a.plus(b));
    var g = new Gradient(y, x).value();

    assertEquals(0.0, y.value());
    assertEquals(g.get(0, 0), 2 * x.get(0).value() - 2 * x.get(1).value());
    assertEquals(g.get(1, 0), -2 * x.get(0).value() + 4 * x.get(1).value() - 2 * x.get(2).value());
    assertEquals(g.get(2, 0), -2 * x.get(1).value() + 4 * x.get(2).value() - 2 * x.get(3).value());
    assertEquals(g.get(3, 0), -2 * x.get(2).value() + 4 * x.get(3).value() - 2 * x.get(4).value());
    assertEquals(g.get(4, 0), -2 * x.get(3).value() + 2 * x.get(4).value());

    var H = new Hessian(y, x).value();

    var expected_H =
        new SimpleMatrix(
            new double[][] {
              {2.0, -2.0, 0.0, 0.0, 0.0},
              {-2.0, 4.0, -2.0, 0.0, 0.0},
              {0.0, -2.0, 4.0, -2.0, 0.0},
              {0.0, 0.0, -2.0, 4.0, -2.0},
              {0.0, 0.0, 0.0, -2.0, 2.0}
            });
    for (int i = 0; i < x.rows(); ++i) {
      for (int j = 0; j < x.rows(); ++j) {
        assertEquals(expected_H.get(i, j), H.get(i, j));
      }
    }
  }

  @Test
  void testSumOfSquares() {
    var r = new VariableMatrix(4);
    r.get(0).setValue(25.0);
    r.get(1).setValue(10.0);
    r.get(2).setValue(5.0);
    r.get(3).setValue(0.0);
    var x = new VariableMatrix(4);
    x.get(0).setValue(0.0);
    x.get(1).setValue(0.0);
    x.get(2).setValue(0.0);
    x.get(3).setValue(0.0);

    var J = new Variable(0.0);
    for (int i = 0; i < 4; ++i) {
      J = J.plus(r.get(i).minus(x.get(i)).times(r.get(i).minus(x.get(i))));
    }

    var H = new Hessian(J, x);

    var expected_H = SimpleMatrix.diag(2.0, 2.0, 2.0, 2.0);
    assertEquals(expected_H, H.get().value());
    assertEquals(expected_H, H.value());
  }

  @Test
  void testRosenbrock() {
    var input = new VariableMatrix(2);
    var x = input.get(0);
    var y = input.get(1);

    for (var x0 : range(-2.5, 2.5, 0.1)) {
      for (var y0 : range(-2.5, 2.5, 0.1)) {
        x.setValue(x0);
        y.setValue(y0);
        var z = pow(new Variable(1).minus(x), 2).plus(pow(y.minus(pow(x, 2)), 2).times(100));

        var H = new Hessian(z, input).value();
        assertEquals(-400 * (y0 - x0 * x0) + 800 * x0 * x0 + 2, H.get(0, 0), 1e-12);
        assertEquals(-400 * x0, H.get(0, 1), 1e-15);
        assertEquals(-400 * x0, H.get(1, 0), 1e-15);
        assertEquals(200, H.get(1, 1));
      }
    }
  }

  @Test
  void testVariableReuse() {
    Variable y;
    var x = new VariableMatrix(1);

    // y = x³
    x.get(0).setValue(1);
    y = x.get(0).times(x.get(0)).times(x.get(0));

    var hessian = new Hessian(y, x);

    // d²y/dx² = 6x
    // H = 6
    var H = hessian.value();

    assertEquals(1, H.getNumRows());
    assertEquals(1, H.getNumCols());
    assertEquals(6.0, H.get(0, 0));

    x.get(0).setValue(2);
    // d²y/dx² = 6x
    // H = 12
    H = hessian.value();

    assertEquals(1, H.getNumRows());
    assertEquals(1, H.getNumCols());
    assertEquals(12.0, H.get(0, 0));
  }
}
