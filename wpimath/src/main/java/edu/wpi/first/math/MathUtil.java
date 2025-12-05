// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math;

import edu.wpi.first.math.geometry.Translation2d;
import edu.wpi.first.math.geometry.Translation3d;

/** Math utility functions. */
public final class MathUtil {
  private MathUtil() {
    throw new AssertionError("utility class");
  }

  /**
   * Returns value clamped between low and high boundaries.
   *
   * @param value Value to clamp.
   * @param low The lower boundary to which to clamp value.
   * @param high The higher boundary to which to clamp value.
   * @return The clamped value.
   */
  public static int clamp(int value, int low, int high) {
    return Math.max(low, Math.min(value, high));
  }

  /**
   * Returns value clamped between low and high boundaries.
   *
   * @param value Value to clamp.
   * @param low The lower boundary to which to clamp value.
   * @param high The higher boundary to which to clamp value.
   * @return The clamped value.
   */
  public static double clamp(double value, double low, double high) {
    return Math.max(low, Math.min(value, high));
  }

  /**
   * Returns 0.0 if the given value is within the specified range around zero. The remaining range
   * between the deadband and the maximum magnitude is scaled from 0.0 to the maximum magnitude.
   *
   * @param value Value to clip.
   * @param deadband Range around zero.
   * @param maxMagnitude The maximum magnitude of the input. Can be infinite.
   * @return The value after the deadband is applied.
   */
  public static double applyDeadband(double value, double deadband, double maxMagnitude) {
    if (Math.abs(value) < deadband) {
      return 0;
    }
    if (value > 0.0) {
      // Map deadband to 0 and map max to max with a linear relationship.
      //
      //   y - y₁ = m(x - x₁)
      //   y - y₁ = (y₂ - y₁)/(x₂ - x₁) (x - x₁)
      //   y = (y₂ - y₁)/(x₂ - x₁) (x - x₁) + y₁
      //
      // (x₁, y₁) = (deadband, 0) and (x₂, y₂) = (max, max).
      //
      //   x₁ = deadband
      //   y₁ = 0
      //   x₂ = max
      //   y₂ = max
      //   y = (max - 0)/(max - deadband) (x - deadband) + 0
      //   y = max/(max - deadband) (x - deadband)
      //
      // To handle high values of max, rewrite so that max only appears on the denominator.
      //
      //   y = ((max - deadband) + deadband)/(max - deadband) (x - deadband)
      //   y = (1 + deadband/(max - deadband)) (x - deadband)
      return (1 + deadband / (maxMagnitude - deadband)) * (value - deadband);
    } else {
      // Map -deadband to 0 and map -max to -max with a linear relationship.
      //
      //   y - y₁ = m(x - x₁)
      //   y - y₁ = (y₂ - y₁)/(x₂ - x₁) (x - x₁)
      //   y = (y₂ - y₁)/(x₂ - x₁) (x - x₁) + y₁
      //
      // (x₁, y₁) = (-deadband, 0) and (x₂, y₂) = (-max, -max).
      //
      //   x₁ = -deadband
      //   y₁ = 0
      //   x₂ = -max
      //   y₂ = -max
      //   y = (-max - 0)/(-max + deadband) (x + deadband) + 0
      //   y = max/(max - deadband) (x + deadband)
      //
      // To handle high values of max, rewrite so that max only appears on the denominator.
      //
      //   y = ((max - deadband) + deadband)/(max - deadband) (x + deadband)
      //   y = (1 + deadband/(max - deadband)) (x + deadband)
      return (1 + deadband / (maxMagnitude - deadband)) * (value + deadband);
    }
  }

  /**
   * Returns 0.0 if the given value is within the specified range around zero. The remaining range
   * between the deadband and 1.0 is scaled from 0.0 to 1.0.
   *
   * @param value Value to clip.
   * @param deadband Range around zero.
   * @return The value after the deadband is applied.
   */
  public static double applyDeadband(double value, double deadband) {
    return applyDeadband(value, deadband, 1);
  }

  /**
   * Returns a zero vector if the given vector is within the specified distance from the origin. The
   * remaining distance between the deadband and the maximum distance is scaled from the origin to
   * the maximum distance.
   *
   * @param value Value to clip.
   * @param deadband Distance from origin.
   * @param maxMagnitude The maximum distance from the origin of the input. Can be infinite.
   * @param <R> The number of rows in the vector.
   * @return The value after the deadband is applied.
   */
  public static <R extends Num> Vector<R> applyDeadband(
      Vector<R> value, double deadband, double maxMagnitude) {
    if (value.norm() < 1e-9) {
      return value.times(0);
    }
    return value.unit().times(applyDeadband(value.norm(), deadband, maxMagnitude));
  }

  /**
   * Returns a zero vector if the given vector is within the specified distance from the origin. The
   * remaining distance between the deadband and a distance of 1.0 is scaled from the origin to a
   * distance of 1.0.
   *
   * @param value Value to clip.
   * @param deadband Distance from origin.
   * @param <R> The number of rows in the vector.
   * @return The value after the deadband is applied.
   */
  public static <R extends Num> Vector<R> applyDeadband(Vector<R> value, double deadband) {
    return applyDeadband(value, deadband, 1);
  }

  /**
   * Raises the input to the power of the given exponent while preserving its sign.
   *
   * <p>The function normalizes the input value to the range [0, 1] based on the maximum magnitude,
   * raises it to the power of the exponent, then scales the result back to the original range and
   * copying the sign. This keeps the value in the original range and gives consistent curve
   * behavior regardless of the input value's scale.
   *
   * <p>This is useful for applying smoother or more aggressive control response curves (e.g.
   * joystick input shaping).
   *
   * @param value The input value to transform.
   * @param exponent The exponent to apply (e.g. 1.0 = linear, 2.0 = squared curve). Must be
   *     positive.
   * @param maxMagnitude The maximum expected absolute value of input. Must be positive.
   * @return The transformed value with the same sign and scaled to the input range.
   */
  public static double copyDirectionPow(double value, double exponent, double maxMagnitude) {
    return Math.copySign(Math.pow(Math.abs(value) / maxMagnitude, exponent), value) * maxMagnitude;
  }

  /**
   * Raises the input to the power of the given exponent while preserving its sign.
   *
   * <p>This is useful for applying smoother or more aggressive control response curves (e.g.
   * joystick input shaping).
   *
   * @param value The input value to transform.
   * @param exponent The exponent to apply (e.g. 1.0 = linear, 2.0 = squared curve). Must be
   *     positive.
   * @return The transformed value with the same sign.
   */
  public static double copyDirectionPow(double value, double exponent) {
    return copyDirectionPow(value, exponent, 1);
  }

  /**
   * Raises the norm of the input to the power of the given exponent while preserving its direction.
   *
   * <p>The function normalizes the norm of the input to the range [0, 1] based on the maximum
   * distance, raises it to the power of the exponent, then scales the result back to the original
   * range. This keeps the value in the original max distance and gives consistent curve behavior
   * regardless of the input norm's scale.
   *
   * @param value The input vector to transform.
   * @param exponent The exponent to apply (e.g. 1.0 = linear, 2.0 = squared curve). Must be
   *     positive.
   * @param maxMagnitude The maximum expected distance from origin of input. Must be positive.
   * @param <R> The number of rows in the vector.
   * @return The transformed value with the same direction and norm scaled to the input range.
   */
  public static <R extends Num> Vector<R> copyDirectionPow(
      Vector<R> value, double exponent, double maxMagnitude) {
    if (value.norm() < 1e-9) {
      return value.times(0);
    }
    return value.unit().times(copyDirectionPow(value.norm(), exponent, maxMagnitude));
  }

  /**
   * Raises the norm of the input to the power of the given exponent while preserving its direction.
   *
   * @param value The input vector to transform.
   * @param exponent The exponent to apply (e.g. 1.0 = linear, 2.0 = squared curve). Must be
   *     positive.
   * @param <R> The number of rows in the vector.
   * @return The transformed value with the same direction.
   */
  public static <R extends Num> Vector<R> copyDirectionPow(Vector<R> value, double exponent) {
    return copyDirectionPow(value, exponent, 1);
  }

  /**
   * Returns modulus of input.
   *
   * @param input Input value to wrap.
   * @param minimumInput The minimum value expected from the input.
   * @param maximumInput The maximum value expected from the input.
   * @return The wrapped value.
   */
  public static double inputModulus(double input, double minimumInput, double maximumInput) {
    double modulus = maximumInput - minimumInput;

    // Wrap input if it's above the maximum input
    int numMax = (int) ((input - minimumInput) / modulus);
    input -= numMax * modulus;

    // Wrap input if it's below the minimum input
    int numMin = (int) ((input - maximumInput) / modulus);
    input -= numMin * modulus;

    return input;
  }

  /**
   * Wraps an angle to the range -π to π radians.
   *
   * @param angleRadians Angle to wrap in radians.
   * @return The wrapped angle.
   */
  public static double angleModulus(double angleRadians) {
    return inputModulus(angleRadians, -Math.PI, Math.PI);
  }

  /**
   * Perform linear interpolation between two values.
   *
   * @param startValue The value to start at.
   * @param endValue The value to end at.
   * @param t How far between the two values to interpolate. This is clamped to [0, 1].
   * @return The interpolated value.
   */
  public static double interpolate(double startValue, double endValue, double t) {
    return startValue + (endValue - startValue) * MathUtil.clamp(t, 0, 1);
  }

  /**
   * Return where within interpolation range [0, 1] q is between startValue and endValue.
   *
   * @param startValue Lower part of interpolation range.
   * @param endValue Upper part of interpolation range.
   * @param q Query.
   * @return Interpolant in range [0, 1].
   */
  public static double inverseInterpolate(double startValue, double endValue, double q) {
    double totalRange = endValue - startValue;
    if (totalRange <= 0) {
      return 0.0;
    }
    double queryToStart = q - startValue;
    if (queryToStart <= 0) {
      return 0.0;
    }
    return queryToStart / totalRange;
  }

  /**
   * Checks if the given value matches an expected value within a certain tolerance.
   *
   * @param expected The expected value
   * @param actual The actual value
   * @param tolerance The allowed difference between the actual and the expected value
   * @return Whether or not the actual value is within the allowed tolerance
   */
  public static boolean isNear(double expected, double actual, double tolerance) {
    if (tolerance < 0) {
      throw new IllegalArgumentException("Tolerance must be a non-negative number!");
    }
    return Math.abs(expected - actual) < tolerance;
  }

  /**
   * Checks if the given value matches an expected value within a certain tolerance. Supports
   * continuous input for cases like absolute encoders.
   *
   * <p>Continuous input means that the min and max value are considered to be the same point, and
   * tolerances can be checked across them. A common example would be for absolute encoders: calling
   * isNear(2, 359, 5, 0, 360) returns true because 359 is 1 away from 360 (which is treated as the
   * same as 0) and 2 is 2 away from 0, adding up to an error of 3 degrees, which is within the
   * given tolerance of 5.
   *
   * @param expected The expected value
   * @param actual The actual value
   * @param tolerance The allowed difference between the actual and the expected value
   * @param min Smallest value before wrapping around to the largest value
   * @param max Largest value before wrapping around to the smallest value
   * @return Whether or not the actual value is within the allowed tolerance
   */
  public static boolean isNear(
      double expected, double actual, double tolerance, double min, double max) {
    if (tolerance < 0) {
      throw new IllegalArgumentException("Tolerance must be a non-negative number!");
    }
    // Max error is exactly halfway between the min and max
    double errorBound = (max - min) / 2.0;
    double error = MathUtil.inputModulus(expected - actual, -errorBound, errorBound);
    return Math.abs(error) < tolerance;
  }

  /**
   * Limits translation velocity.
   *
   * @param current Translation at current timestep.
   * @param next Translation at next timestep.
   * @param dt Timestep duration.
   * @param maxVelocity Maximum translation velocity.
   * @return Returns the next Translation2d limited to maxVelocity
   */
  public static Translation2d slewRateLimit(
      Translation2d current, Translation2d next, double dt, double maxVelocity) {
    if (maxVelocity < 0) {
      Exception e = new IllegalArgumentException();
      MathSharedStore.reportError(
          "maxVelocity must be a non-negative number, got " + maxVelocity, e.getStackTrace());
      return next;
    }
    Translation2d diff = next.minus(current);
    double dist = diff.getNorm();
    if (dist < 1e-9) {
      return next;
    }
    if (dist > maxVelocity * dt) {
      // Move maximum allowed amount in direction of the difference
      return current.plus(diff.times(maxVelocity * dt / dist));
    }
    return next;
  }

  /**
   * Limits translation velocity.
   *
   * @param current Translation at current timestep.
   * @param next Translation at next timestep.
   * @param dt Timestep duration.
   * @param maxVelocity Maximum translation velocity.
   * @return Returns the next Translation3d limited to maxVelocity
   */
  public static Translation3d slewRateLimit(
      Translation3d current, Translation3d next, double dt, double maxVelocity) {
    if (maxVelocity < 0) {
      Exception e = new IllegalArgumentException();
      MathSharedStore.reportError(
          "maxVelocity must be a non-negative number, got " + maxVelocity, e.getStackTrace());
      return next;
    }
    Translation3d diff = next.minus(current);
    double dist = diff.getNorm();
    if (dist < 1e-9) {
      return next;
    }
    if (dist > maxVelocity * dt) {
      // Move maximum allowed amount in direction of the difference
      return current.plus(diff.times(maxVelocity * dt / dist));
    }
    return next;
  }
}
