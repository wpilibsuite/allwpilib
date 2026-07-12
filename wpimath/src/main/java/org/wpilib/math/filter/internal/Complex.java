// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.filter.internal;

/**
 * Minimal immutable complex number, package-private to support the BiquadFilter design code.
 * Operations are named to match the C++ {@code std::complex<double>} surface used by the filter
 * design helpers, not Java's {@code BigDecimal}-style arithmetic.
 */
final class Complex {
  static final Complex ONE = new Complex(1.0, 0.0);

  final double re;
  final double im;

  Complex(double re, double im) {
    this.re = re;
    this.im = im;
  }

  /** Construct from polar form: r·(cos(θ) + i·sin(θ)). */
  static Complex polar(double r, double theta) {
    return new Complex(r * Math.cos(theta), r * Math.sin(theta));
  }

  double real() {
    return re;
  }

  double imag() {
    return im;
  }

  /** Magnitude |z| = sqrt(x² + y²). */
  double abs() {
    return Math.hypot(re, im);
  }

  /** Squared magnitude |z|² = x² + y² (matches std::norm). */
  double normSq() {
    return re * re + im * im;
  }

  Complex conj() {
    return new Complex(re, -im);
  }

  Complex negate() {
    return new Complex(-re, -im);
  }

  Complex add(Complex other) {
    return new Complex(re + other.re, im + other.im);
  }

  Complex add(double s) {
    return new Complex(re + s, im);
  }

  Complex sub(Complex other) {
    return new Complex(re - other.re, im - other.im);
  }

  Complex sub(double s) {
    return new Complex(re - s, im);
  }

  Complex mul(Complex other) {
    return new Complex(re * other.re - im * other.im, re * other.im + im * other.re);
  }

  Complex mul(double s) {
    return new Complex(re * s, im * s);
  }

  Complex div(Complex other) {
    double d = other.normSq();
    return new Complex((re * other.re + im * other.im) / d, (im * other.re - re * other.im) / d);
  }

  Complex div(double s) {
    return new Complex(re / s, im / s);
  }

  /** Sinh on the principal branch: sinh(x+iy) = sinh(x)·cos(y) + i·cosh(x)·sin(y). */
  Complex sinh() {
    return new Complex(Math.sinh(re) * Math.cos(im), Math.cosh(re) * Math.sin(im));
  }

  /**
   * Principal-branch square root. Matches std::sqrt for std::complex<double> — the result has
   * non-negative real part, and im ≥ 0 when input has im ≥ 0.
   */
  Complex sqrt() {
    if (re == 0.0 && im == 0.0) {
      return new Complex(0.0, 0.0);
    }
    double r = abs();
    double sgn = im >= 0.0 ? 1.0 : -1.0;
    return new Complex(Math.sqrt(0.5 * (r + re)), sgn * Math.sqrt(0.5 * (r - re)));
  }

  /** Principal-branch complex log: log(z) = ln|z| + i·arg(z). */
  Complex log() {
    return new Complex(Math.log(abs()), Math.atan2(im, re));
  }

  /** Principal-branch arcsin via -i·log(i·z + sqrt(1 - z²)). */
  Complex asin() {
    Complex inside = new Complex(-im, re).add(ONE.sub(this.mul(this)).sqrt());
    Complex l = inside.log();
    return new Complex(l.im, -l.re);
  }

  /** Principal-branch atanh via 0.5·log((1+z)/(1-z)). */
  Complex atanh() {
    Complex num = new Complex(1.0 + re, im);
    Complex den = new Complex(1.0 - re, -im);
    return num.div(den).log().mul(0.5);
  }
}
