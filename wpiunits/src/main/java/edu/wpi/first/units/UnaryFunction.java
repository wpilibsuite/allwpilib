// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import java.util.Objects;

/**
 * A function that accepts a single {@code double} and returns a {@code double} result. This is used
 * to represent arbitrary mapping functions for converting units to and from a base unit
 * representation. Temperature units, in particular, typically have an offset from a value in Kelvin
 * and may have a multiplication factor added in, which means that units cannot always be
 * represented as simple ratios of their base units.
 */
@FunctionalInterface
public interface UnaryFunction {
  /** The identity function that simply returns the input value. */
  UnaryFunction IDENTITY = x -> x;

  /**
   * Applies this function to the input value and returns the result.
   *
   * @param input the input value to the function
   * @return the result
   */
  double apply(double input);

  /**
   * Constructs a new function that first calls this function, then passes the result to another as
   * input.
   *
   * <pre>
   * f = x -&gt; x + 1 // f(x) = x + 1
   * g = x -&gt; 2 * x // g(x) = 2x
   *
   * h = f.pipeTo(g) // h(x) = g(f(x))
   * </pre>
   *
   * @param next the next operation to pipe to
   * @return the composite function g(f(x))
   */
  default UnaryFunction pipeTo(UnaryFunction next) {
    Objects.requireNonNull(next, "The next operation in the chain must be provided");

    return x -> next.apply(this.apply(x));
  }

  /**
   * Creates a composite function h(x) such that h(x) = f(x) * g(x).
   *
   * @param multiplier the function to multiply this one by
   * @return the composite function f(x) * g(x)
   */
  default UnaryFunction mult(UnaryFunction multiplier) {
    Objects.requireNonNull(multiplier, "A multiplier function must be provided");

    return x -> this.apply(x) * multiplier.apply(x);
  }

  /**
   * Creates a composite function h(x) such that h(x) = k * f(x).
   *
   * @param multiplier the constant value to multiply this function's results by
   * @return the composite function k * f(x)
   */
  default UnaryFunction mult(double multiplier) {
    return x -> this.apply(x) * multiplier;
  }

  /**
   * Creates a composite function h(x) such that h(x) = f(x) / g(x).
   *
   * @param divisor the function to divide this one by
   * @return the composite function f(x) / g(x)
   */
  default UnaryFunction div(UnaryFunction divisor) {
    Objects.requireNonNull(divisor, "A divisor function must be provided");

    return x -> {
      double numerator = this.apply(x);

      // fast-track to avoid another function call
      // avoids returning NaN if divisor is also zero
      if (numerator == 0) {
        return 0;
      }

      double div = divisor.apply(x);
      return numerator / div; // NOTE: returns +Infinity or -Infinity if div is zero
    };
  }

  /**
   * Creates a composite function h(x) such that h(x) = 1/k * f(x).
   *
   * @param divisor the constant value to divide this function's results by
   * @return the composite function 1/k * f(x)
   */
  default UnaryFunction div(double divisor) {
    return x -> this.apply(x) / divisor;
  }

  /**
   * Creates a composite function h(x) such that h(x) = f(x) ^ g(x).
   *
   * @param exponent the function to exponentiate this function's results by
   * @return the composite function f(x) ^ g(x)
   */
  default UnaryFunction exp(UnaryFunction exponent) {
    Objects.requireNonNull(exponent, "An exponent function must be provided");

    return x -> Math.pow(this.apply(x), exponent.apply(x));
  }

  /**
   * Creates a composite function h(x) such that h(x) = f(x) ^ k.
   *
   * @param exponent the constant value to exponentiate this function's results by
   * @return the composite function f(x) ^ k
   */
  default UnaryFunction exp(double exponent) {
    return x -> Math.pow(this.apply(x), exponent);
  }
}
