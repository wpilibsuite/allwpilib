// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.autodiff;

import static edu.wpi.first.math.MatrixAssertions.assertEquals;
import static edu.wpi.first.math.autodiff.Variable.abs;
import static edu.wpi.first.math.autodiff.Variable.acos;
import static edu.wpi.first.math.autodiff.Variable.asin;
import static edu.wpi.first.math.autodiff.Variable.atan;
import static edu.wpi.first.math.autodiff.Variable.atan2;
import static edu.wpi.first.math.autodiff.Variable.cbrt;
import static edu.wpi.first.math.autodiff.Variable.cos;
import static edu.wpi.first.math.autodiff.Variable.cosh;
import static edu.wpi.first.math.autodiff.Variable.exp;
import static edu.wpi.first.math.autodiff.Variable.hypot;
import static edu.wpi.first.math.autodiff.Variable.log;
import static edu.wpi.first.math.autodiff.Variable.log10;
import static edu.wpi.first.math.autodiff.Variable.pow;
import static edu.wpi.first.math.autodiff.Variable.signum;
import static edu.wpi.first.math.autodiff.Variable.sin;
import static edu.wpi.first.math.autodiff.Variable.sinh;
import static edu.wpi.first.math.autodiff.Variable.sqrt;
import static edu.wpi.first.math.autodiff.Variable.tan;
import static edu.wpi.first.math.autodiff.Variable.tanh;
import static org.junit.jupiter.api.Assertions.assertEquals;

import org.ejml.simple.SimpleMatrix;
import org.junit.jupiter.api.Test;

class GradientTest {
  @Test
  void testTrivialCase() {
    var a = new Variable();
    a.setValue(10);
    var b = new Variable();
    b.setValue(20);
    var c = a;

    assertEquals(1.0, new Gradient(a, a).value().get(0, 0));
    assertEquals(0.0, new Gradient(a, b).value().get(0, 0));
    assertEquals(1.0, new Gradient(c, a).value().get(0, 0));
    assertEquals(0.0, new Gradient(c, b).value().get(0, 0));
  }

  @Test
  void testUnaryPlus() {
    var a = new Variable();
    a.setValue(10);
    var c = a.unaryPlus();

    assertEquals(a.value(), c.value());
    assertEquals(1.0, new Gradient(c, a).value().get(0, 0));
  }

  @Test
  void testUnaryMinus() {
    var a = new Variable();
    a.setValue(10);
    var c = a.unaryMinus();

    assertEquals(a.unaryMinus().value(), c.value());
    assertEquals(-1.0, new Gradient(c, a).value().get(0, 0));
  }

  @Test
  void testIdenticalVariables() {
    var a = new Variable();
    a.setValue(10);
    var x = a;
    var c = a.times(a).plus(x);

    assertEquals(a.value() * a.value() + x.value(), c.value());
    assertEquals(
        2 * a.value() + new Gradient(x, a).value().get(0, 0), new Gradient(c, a).value().get(0, 0));
    assertEquals(
        2 * a.value() * new Gradient(a, x).value().get(0, 0) + 1,
        new Gradient(c, x).value().get(0, 0));
  }

  @Test
  void testElementary() {
    var a = new Variable();
    a.setValue(1.0);
    var b = new Variable();
    b.setValue(2.0);

    var c = a.times(-2);
    assertEquals(-2.0, new Gradient(c, a).value().get(0, 0));

    c = a.div(3.0);
    assertEquals(1.0 / 3.0, new Gradient(c, a).value().get(0, 0));

    a.setValue(100.0);
    b.setValue(200.0);

    c = a.plus(b);
    assertEquals(1.0, new Gradient(c, a).value().get(0, 0));
    assertEquals(1.0, new Gradient(c, b).value().get(0, 0));

    c = a.minus(b);
    assertEquals(1.0, new Gradient(c, a).value().get(0, 0));
    assertEquals(-1.0, new Gradient(c, b).value().get(0, 0));

    c = a.unaryMinus().plus(b);
    assertEquals(-1.0, new Gradient(c, a).value().get(0, 0));
    assertEquals(1.0, new Gradient(c, b).value().get(0, 0));

    c = a.plus(1);
    assertEquals(1.0, new Gradient(c, a).value().get(0, 0));
  }

  @Test
  void testTrigonometry() {
    var x = new Variable();
    x.setValue(0.5);

    // Math.sin(x)
    assertEquals(Math.sin(x.value()), sin(x).value());

    var g = new Gradient(sin(x), x);
    assertEquals(Math.cos(x.value()), g.get().value().get(0, 0));
    assertEquals(Math.cos(x.value()), g.value().get(0, 0));

    // Math.cos(x)
    assertEquals(Math.cos(x.value()), cos(x).value());

    g = new Gradient(cos(x), x);
    assertEquals(-Math.sin(x.value()), g.get().value().get(0, 0));
    assertEquals(-Math.sin(x.value()), g.value().get(0, 0));

    // Math.tan(x)
    assertEquals(Math.tan(x.value()), tan(x).value());

    g = new Gradient(tan(x), x);
    assertEquals(1.0 / (Math.cos(x.value()) * Math.cos(x.value())), g.get().value().get(0, 0));
    assertEquals(1.0 / (Math.cos(x.value()) * Math.cos(x.value())), g.value().get(0, 0));

    // Math.asin(x)
    assertEquals(Math.asin(x.value()), asin(x).value(), 1e-15);

    g = new Gradient(asin(x), x);
    assertEquals(1.0 / Math.sqrt(1 - x.value() * x.value()), g.get().value().get(0, 0));
    assertEquals(1.0 / Math.sqrt(1 - x.value() * x.value()), g.value().get(0, 0));

    // Math.acos(x)
    assertEquals(Math.acos(x.value()), acos(x).value(), 1e-15);

    g = new Gradient(acos(x), x);
    assertEquals(-1.0 / Math.sqrt(1 - x.value() * x.value()), g.get().value().get(0, 0));
    assertEquals(-1.0 / Math.sqrt(1 - x.value() * x.value()), g.value().get(0, 0));

    // Math.atan(x)
    assertEquals(Math.atan(x.value()), atan(x).value(), 1e-15);

    g = new Gradient(atan(x), x);
    assertEquals(1.0 / (1 + x.value() * x.value()), g.get().value().get(0, 0));
    assertEquals(1.0 / (1 + x.value() * x.value()), g.value().get(0, 0));
  }

  @Test
  void testHyperbolic() {
    var x = new Variable();
    x.setValue(1.0);

    // sinh(x)
    assertEquals(Math.sinh(x.value()), sinh(x).value());

    var g = new Gradient(sinh(x), x);
    assertEquals(Math.cosh(x.value()), g.get().value().get(0, 0), 1e-15);
    assertEquals(Math.cosh(x.value()), g.value().get(0, 0), 1e-15);

    // Math.cosh(x)
    assertEquals(Math.cosh(x.value()), cosh(x).value(), 1e-15);

    g = new Gradient(cosh(x), x);
    assertEquals(Math.sinh(x.value()), g.get().value().get(0, 0));
    assertEquals(Math.sinh(x.value()), g.value().get(0, 0));

    // tanh(x)
    assertEquals(Math.tanh(x.value()), tanh(x).value());

    g = new Gradient(tanh(x), x);
    assertEquals(
        1.0 / (Math.cosh(x.value()) * Math.cosh(x.value())), g.get().value().get(0, 0), 1e-15);
    assertEquals(1.0 / (Math.cosh(x.value()) * Math.cosh(x.value())), g.value().get(0, 0), 1e-15);
  }

  @Test
  void testExponential() {
    var x = new Variable();
    x.setValue(1.0);

    // Math.log(x)
    assertEquals(Math.log(x.value()), log(x).value());

    var g = new Gradient(log(x), x);
    assertEquals(1.0 / x.value(), g.get().value().get(0, 0));
    assertEquals(1.0 / x.value(), g.value().get(0, 0));

    // Math.log10(x)
    assertEquals(Math.log10(x.value()), log10(x).value());

    g = new Gradient(log10(x), x);
    assertEquals(1.0 / (Math.log(10.0) * x.value()), g.get().value().get(0, 0));
    assertEquals(1.0 / (Math.log(10.0) * x.value()), g.value().get(0, 0));

    // Math.exp(x)
    assertEquals(Math.exp(x.value()), exp(x).value(), 1e-15);

    g = new Gradient(exp(x), x);
    assertEquals(Math.exp(x.value()), g.get().value().get(0, 0), 1e-15);
    assertEquals(Math.exp(x.value()), g.value().get(0, 0), 1e-15);
  }

  @Test
  void testPower() {
    var x = new Variable();
    x.setValue(1.0);
    var a = new Variable();
    a.setValue(2.0);

    // Math.sqrt(x)
    assertEquals(Math.sqrt(x.value()), sqrt(x).value());

    var g = new Gradient(sqrt(x), x);
    assertEquals(0.5 / Math.sqrt(x.value()), g.get().value().get(0, 0));
    assertEquals(0.5 / Math.sqrt(x.value()), g.value().get(0, 0));

    // Math.sqrt(a)
    assertEquals(Math.sqrt(a.value()), sqrt(a).value());

    g = new Gradient(sqrt(a), a);
    assertEquals(0.5 / Math.sqrt(a.value()), g.get().value().get(0, 0));
    assertEquals(0.5 / Math.sqrt(a.value()), g.value().get(0, 0));

    // Math.cbrt(x)
    assertEquals(Math.cbrt(x.value()), cbrt(x).value());

    g = new Gradient(cbrt(x), x);
    assertEquals(
        1.0 / (3.0 * Math.cbrt(x.value()) * Math.cbrt(x.value())), g.get().value().get(0, 0));
    assertEquals(1.0 / (3.0 * Math.cbrt(x.value()) * Math.cbrt(x.value())), g.value().get(0, 0));

    // Math.cbrt(a)
    assertEquals(Math.cbrt(a.value()), cbrt(a).value(), 1e-15);

    g = new Gradient(cbrt(a), a);
    assertEquals(
        1.0 / (3.0 * Math.cbrt(a.value()) * Math.cbrt(a.value())),
        g.get().value().get(0, 0),
        1e-15);
    assertEquals(
        1.0 / (3.0 * Math.cbrt(a.value()) * Math.cbrt(a.value())), g.value().get(0, 0), 1e-15);

    // x²
    assertEquals(Math.pow(x.value(), 2.0), pow(x, 2.0).value());

    g = new Gradient(pow(x, 2.0), x);
    assertEquals(2.0 * x.value(), g.get().value().get(0, 0));
    assertEquals(2.0 * x.value(), g.value().get(0, 0));

    // 2ˣ
    assertEquals(Math.pow(2.0, x.value()), pow(2.0, x).value());

    g = new Gradient(pow(2.0, x), x);
    assertEquals(Math.log(2.0) * Math.pow(2.0, x.value()), g.get().value().get(0, 0));
    assertEquals(Math.log(2.0) * Math.pow(2.0, x.value()), g.value().get(0, 0));

    // xˣ
    assertEquals(Math.pow(x.value(), x.value()), pow(x, x).value());

    g = new Gradient(pow(x, x), x);
    assertEquals(
        (Math.log(x.value()) + 1) * Math.pow(x.value(), x.value()), g.get().value().get(0, 0));
    assertEquals((Math.log(x.value()) + 1) * Math.pow(x.value(), x.value()), g.value().get(0, 0));

    // y(a)
    var y = a.times(2);
    assertEquals(2 * a.value(), y.value());

    g = new Gradient(y, a);
    assertEquals(2.0, g.get().value().get(0, 0));
    assertEquals(2.0, g.value().get(0, 0));

    // xʸ(x)
    assertEquals(Math.pow(x.value(), y.value()), pow(x, y).value());

    g = new Gradient(pow(x, y), x);
    assertEquals(y.value() / x.value() * Math.pow(x.value(), y.value()), g.get().value().get(0, 0));
    assertEquals(y.value() / x.value() * Math.pow(x.value(), y.value()), g.value().get(0, 0));

    // xʸ(a)
    assertEquals(Math.pow(x.value(), y.value()), pow(x, y).value());

    g = new Gradient(pow(x, y), a);
    assertEquals(
        Math.pow(x.value(), y.value())
            * (y.value() / x.value() * new Gradient(x, a).value().get(0, 0)
                + Math.log(x.value()) * new Gradient(y, a).value().get(0, 0)),
        g.get().value().get(0, 0));
    assertEquals(
        Math.pow(x.value(), y.value())
            * (y.value() / x.value() * new Gradient(x, a).value().get(0, 0)
                + Math.log(x.value()) * new Gradient(y, a).value().get(0, 0)),
        g.value().get(0, 0));

    // xʸ(y)
    assertEquals(Math.pow(x.value(), y.value()), pow(x, y).value());

    g = new Gradient(pow(x, y), y);
    assertEquals(Math.log(x.value()) * Math.pow(x.value(), y.value()), g.get().value().get(0, 0));
    assertEquals(Math.log(x.value()) * Math.pow(x.value(), y.value()), g.value().get(0, 0));
  }

  @Test
  void testAbs() {
    var x = new Variable();
    var g = new Gradient(abs(x), x);

    x.setValue(1.0);
    assertEquals(Math.abs(x.value()), abs(x).value());
    assertEquals(1.0, g.get().value().get(0, 0));
    assertEquals(1.0, g.value().get(0, 0));

    x.setValue(-1.0);
    assertEquals(Math.abs(x.value()), abs(x).value());
    assertEquals(-1.0, g.get().value().get(0, 0));
    assertEquals(-1.0, g.value().get(0, 0));

    x.setValue(0.0);
    assertEquals(Math.abs(x.value()), abs(x).value());
    assertEquals(0.0, g.get().value().get(0, 0));
    assertEquals(0.0, g.value().get(0, 0));
  }

  @Test
  void testAtan2() {
    var x = new Variable();
    var y = new Variable();

    // Testing atan2 function on (double, var)
    x.setValue(1.0);
    y.setValue(0.9);
    assertEquals(Math.atan2(2.0, x.value()), atan2(2.0, x).value());

    var g = new Gradient(atan2(2.0, x), x);
    assertEquals(-2.0 / (2 * 2 + x.value() * x.value()), g.get().value().get(0, 0), 1e-15);
    assertEquals(-2.0 / (2 * 2 + x.value() * x.value()), g.value().get(0, 0), 1e-15);

    // Testing atan2 function on (var, double)
    x.setValue(1.0);
    y.setValue(0.9);
    assertEquals(Math.atan2(x.value(), 2.0), atan2(x, 2.0).value());

    g = new Gradient(atan2(x, 2.0), x);
    assertEquals(2.0 / (2 * 2 + x.value() * x.value()), g.get().value().get(0, 0), 1e-15);
    assertEquals(2.0 / (2 * 2 + x.value() * x.value()), g.value().get(0, 0), 1e-15);

    // Testing atan2 function on (var, var)
    x.setValue(1.1);
    y.setValue(0.9);
    assertEquals(Math.atan2(y.value(), x.value()), atan2(y, x).value(), 1e-15);

    g = new Gradient(atan2(y, x), y);
    assertEquals(
        x.value() / (x.value() * x.value() + y.value() * y.value()),
        g.get().value().get(0, 0),
        1e-15);
    assertEquals(
        x.value() / (x.value() * x.value() + y.value() * y.value()), g.value().get(0, 0), 1e-15);

    g = new Gradient(atan2(y, x), x);
    assertEquals(
        -y.value() / (x.value() * x.value() + y.value() * y.value()),
        g.get().value().get(0, 0),
        1e-15);
    assertEquals(
        -y.value() / (x.value() * x.value() + y.value() * y.value()), g.value().get(0, 0), 1e-15);

    // Testing atan2 function on (expr, expr)
    assertEquals(
        3 * Math.atan2(Math.sin(y.value()), 2 * x.value() + 1),
        3 * atan2(sin(y), x.times(2).plus(1)).value(),
        1e-15);

    g = new Gradient(atan2(sin(y), x.times(2).plus(1)).times(3), y);
    assertEquals(
        3
            * (2 * x.value() + 1)
            * Math.cos(y.value())
            / ((2 * x.value() + 1) * (2 * x.value() + 1)
                + Math.sin(y.value()) * Math.sin(y.value())),
        g.get().value().get(0, 0),
        1e-15);
    assertEquals(
        3
            * (2 * x.value() + 1)
            * Math.cos(y.value())
            / ((2 * x.value() + 1) * (2 * x.value() + 1)
                + Math.sin(y.value()) * Math.sin(y.value())),
        g.value().get(0, 0),
        1e-15);

    g = new Gradient(atan2(sin(y), x.times(2).plus(1)).times(3), x);
    assertEquals(
        3
            * -2
            * Math.sin(y.value())
            / ((2 * x.value() + 1) * (2 * x.value() + 1)
                + Math.sin(y.value()) * Math.sin(y.value())),
        g.get().value().get(0, 0),
        1e-15);
    assertEquals(
        3
            * -2
            * Math.sin(y.value())
            / ((2 * x.value() + 1) * (2 * x.value() + 1)
                + Math.sin(y.value()) * Math.sin(y.value())),
        g.value().get(0, 0),
        1e-15);
  }

  private double hypot(double x, double y, double z) {
    return Math.sqrt(x * x + y * y + z * z);
  }

  @Test
  void testHypot() {
    var x = new Variable();
    var y = new Variable();

    // Testing hypot function on (var, double)
    x.setValue(1.8);
    y.setValue(1.5);
    assertEquals(Math.hypot(x.value(), 2.0), Variable.hypot(x, 2.0).value());

    var g = new Gradient(Variable.hypot(x, 2.0), x);
    assertEquals(x.value() / Math.hypot(x.value(), 2.0), g.get().value().get(0, 0));
    assertEquals(x.value() / Math.hypot(x.value(), 2.0), g.value().get(0, 0));

    // Testing hypot function on (double, var)
    assertEquals(Math.hypot(2.0, y.value()), Variable.hypot(2.0, y).value());

    g = new Gradient(Variable.hypot(2.0, y), y);
    assertEquals(y.value() / Math.hypot(2.0, y.value()), g.get().value().get(0, 0));
    assertEquals(y.value() / Math.hypot(2.0, y.value()), g.value().get(0, 0));

    // Testing hypot function on (var, var)
    x.setValue(1.3);
    y.setValue(2.3);
    assertEquals(Math.hypot(x.value(), y.value()), Variable.hypot(x, y).value());

    g = new Gradient(Variable.hypot(x, y), x);
    assertEquals(x.value() / Math.hypot(x.value(), y.value()), g.get().value().get(0, 0));
    assertEquals(x.value() / Math.hypot(x.value(), y.value()), g.value().get(0, 0));

    g = new Gradient(Variable.hypot(x, y), y);
    assertEquals(y.value() / Math.hypot(x.value(), y.value()), g.get().value().get(0, 0));
    assertEquals(y.value() / Math.hypot(x.value(), y.value()), g.value().get(0, 0));

    // Testing hypot function on (expr, expr)
    x.setValue(1.3);
    y.setValue(2.3);
    assertEquals(
        Math.hypot(2.0 * x.value(), 3.0 * y.value()),
        Variable.hypot(x.times(2.0), y.times(3.0)).value());

    g = new Gradient(Variable.hypot(x.times(2.0), y.times(3.0)), x);
    assertEquals(
        4.0 * x.value() / Math.hypot(2.0 * x.value(), 3.0 * y.value()), g.get().value().get(0, 0));
    assertEquals(
        4.0 * x.value() / Math.hypot(2.0 * x.value(), 3.0 * y.value()), g.value().get(0, 0));

    g = new Gradient(Variable.hypot(x.times(2.0), y.times(3.0)), y);
    assertEquals(
        9.0 * y.value() / Math.hypot(2.0 * x.value(), 3.0 * y.value()), g.get().value().get(0, 0));
    assertEquals(
        9.0 * y.value() / Math.hypot(2.0 * x.value(), 3.0 * y.value()), g.value().get(0, 0));

    // Testing hypot function on (var, var, var)
    var z = new Variable();
    x.setValue(1.3);
    y.setValue(2.3);
    z.setValue(3.3);
    assertEquals(Variable.hypot(x, y, z).value(), hypot(x.value(), y.value(), z.value()));

    g = new Gradient(Variable.hypot(x, y, z), x);
    assertEquals(x.value() / hypot(x.value(), y.value(), z.value()), g.get().value().get(0, 0));
    assertEquals(x.value() / hypot(x.value(), y.value(), z.value()), g.value().get(0, 0));

    g = new Gradient(Variable.hypot(x, y, z), y);
    assertEquals(y.value() / hypot(x.value(), y.value(), z.value()), g.get().value().get(0, 0));
    assertEquals(y.value() / hypot(x.value(), y.value(), z.value()), g.value().get(0, 0));

    g = new Gradient(Variable.hypot(x, y, z), z);
    assertEquals(z.value() / hypot(x.value(), y.value(), z.value()), g.get().value().get(0, 0));
    assertEquals(z.value() / hypot(x.value(), y.value(), z.value()), g.value().get(0, 0));
  }

  @Test
  void testMiscellaneous() {
    var x = new Variable();

    // dx/dx
    x.setValue(3.0);
    assertEquals(Math.abs(x.value()), abs(x).value());

    var g = new Gradient(x, x);
    assertEquals(1.0, g.get().value().get(0, 0));
    assertEquals(1.0, g.value().get(0, 0));
  }

  @Test
  void testVariableReuse() {
    var a = new Variable();
    a.setValue(10);

    var b = new Variable();
    b.setValue(20);

    var x = a.times(b);

    var g = new Gradient(x, a);

    assertEquals(20.0, g.get().value().get(0, 0));
    assertEquals(20.0, g.value().get(0, 0));

    b.setValue(10);
    assertEquals(10.0, g.get().value().get(0, 0));
    assertEquals(10.0, g.value().get(0, 0));
  }

  @Test
  void testSignum() {
    var x = new Variable();

    // signum(1.0)
    x.setValue(1.0);
    assertEquals(Math.signum(x.value()), signum(x).value());

    var g = new Gradient(signum(x), x);
    assertEquals(0.0, g.get().value().get(0, 0));
    assertEquals(0.0, g.value().get(0, 0));

    // signum(-1.0)
    x.setValue(-1.0);
    assertEquals(Math.signum(x.value()), signum(x).value());

    g = new Gradient(signum(x), x);
    assertEquals(0.0, g.get().value().get(0, 0));
    assertEquals(0.0, g.value().get(0, 0));

    // signum(0.0)
    x.setValue(0.0);
    assertEquals(Math.signum(x.value()), signum(x).value());

    g = new Gradient(signum(x), x);
    assertEquals(0.0, g.get().value().get(0, 0));
    assertEquals(0.0, g.value().get(0, 0));
  }

  @Test
  void testNonScalar() {
    var x = new VariableMatrix(3);
    x.get(0).setValue(1);
    x.get(1).setValue(2);
    x.get(2).setValue(3);

    // y = [x₁ + 3x₂ − 5x₃]
    //
    // dy/dx = [1  3  −5]
    var y = x.get(0).plus(x.get(1).times(3)).minus(x.get(2).times(5));
    var g = new Gradient(y, x);

    var expected_g = new SimpleMatrix(new double[][] {{1.0}, {3.0}, {-5.0}});

    var g_get_value = g.get().value();
    assertEquals(3, g_get_value.getNumRows());
    assertEquals(1, g_get_value.getNumCols());
    assertEquals(expected_g, g_get_value);

    var g_value = g.value();
    assertEquals(3, g_value.getNumRows());
    assertEquals(1, g_value.getNumCols());
    assertEquals(expected_g, g_value);
  }
}
