// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import java.util.Objects;

@FunctionalInterface
public interface UnaryFunction {
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
   */
  default UnaryFunction pipeTo(UnaryFunction next) {
    Objects.requireNonNull(next, "The next operation in the chain must be provided");

    return x -> next.apply(this.apply(x));
  }

  /** h(x) = f(x) * g(x). */
  default UnaryFunction mult(UnaryFunction multiplier) {
    Objects.requireNonNull(multiplier, "A multiplier function must be provided");

    return x -> this.apply(x) * multiplier.apply(x);
  }

  /** g(x) = f(x) * k. */
  default UnaryFunction mult(double multiplier) {
    return x -> this.apply(x) * multiplier;
  }

  /** h(x) = f(x) / g(x). */
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

  /** g(x) * f(x) / k. */
  default UnaryFunction div(double divisor) {
    return x -> this.apply(x) / divisor;
  }

  /** h(x) = f(x) ^ g(x). */
  default UnaryFunction exp(UnaryFunction exponent) {
    Objects.requireNonNull(exponent, "An exponent function must be provided");

    return x -> Math.pow(this.apply(x), exponent.apply(x));
  }

  default UnaryFunction exp(double exponent) {
    return x -> Math.pow(this.apply(x), exponent);
  }
}
