// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.geometry;

import static edu.wpi.first.units.Units.Radians;

import com.fasterxml.jackson.annotation.JsonAutoDetect;
import com.fasterxml.jackson.annotation.JsonCreator;
import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import com.fasterxml.jackson.annotation.JsonProperty;
import edu.wpi.first.math.MatBuilder;
import edu.wpi.first.math.MathSharedStore;
import edu.wpi.first.math.MathUtil;
import edu.wpi.first.math.Matrix;
import edu.wpi.first.math.Nat;
import edu.wpi.first.math.geometry.proto.Rotation2dProto;
import edu.wpi.first.math.geometry.struct.Rotation2dStruct;
import edu.wpi.first.math.interpolation.Interpolatable;
import edu.wpi.first.math.numbers.N2;
import edu.wpi.first.math.util.Units;
import edu.wpi.first.units.measure.Angle;
import edu.wpi.first.util.protobuf.ProtobufSerializable;
import edu.wpi.first.util.struct.StructSerializable;
import java.util.Objects;

/**
 * A rotation in a 2D coordinate frame represented by a point on the unit circle (cosine and sine).
 *
 * <p>The angle is continuous, that is if a Rotation2d is constructed with 361 degrees, it will
 * return 361 degrees. This allows algorithms that wouldn't want to see a discontinuity in the
 * rotations as it sweeps past from 360 to 0 on the second time around.
 */
@JsonIgnoreProperties(ignoreUnknown = true)
@JsonAutoDetect(getterVisibility = JsonAutoDetect.Visibility.NONE)
public class Rotation2d
    implements Interpolatable<Rotation2d>, ProtobufSerializable, StructSerializable {
  /**
   * A preallocated Rotation2d representing no rotation.
   *
   * <p>This exists to avoid allocations for common rotations.
   */
  public static final Rotation2d kZero = new Rotation2d();

  /**
   * A preallocated Rotation2d representing a clockwise rotation by π/2 rad (90°).
   *
   * <p>This exists to avoid allocations for common rotations.
   */
  public static final Rotation2d kCW_Pi_2 = new Rotation2d(-Math.PI / 2);

  /**
   * A preallocated Rotation2d representing a clockwise rotation by 90° (π/2 rad).
   *
   * <p>This exists to avoid allocations for common rotations.
   */
  public static final Rotation2d kCW_90deg = kCW_Pi_2;

  /**
   * A preallocated Rotation2d representing a counterclockwise rotation by π/2 rad (90°).
   *
   * <p>This exists to avoid allocations for common rotations.
   */
  public static final Rotation2d kCCW_Pi_2 = new Rotation2d(Math.PI / 2);

  /**
   * A preallocated Rotation2d representing a counterclockwise rotation by 90° (π/2 rad).
   *
   * <p>This exists to avoid allocations for common rotations.
   */
  public static final Rotation2d kCCW_90deg = kCCW_Pi_2;

  /**
   * A preallocated Rotation2d representing a counterclockwise rotation by π rad (180°).
   *
   * <p>This exists to avoid allocations for common rotations.
   */
  public static final Rotation2d kPi = new Rotation2d(Math.PI);

  /**
   * A preallocated Rotation2d representing a counterclockwise rotation by 180° (π rad).
   *
   * <p>This exists to avoid allocations for common rotations.
   */
  public static final Rotation2d k180deg = kPi;

  private final double m_value;
  private final double m_cos;
  private final double m_sin;

  /** Constructs a Rotation2d with a default angle of 0 degrees. */
  public Rotation2d() {
    m_value = 0.0;
    m_cos = 1.0;
    m_sin = 0.0;
  }

  /**
   * Constructs a Rotation2d with the given radian value.
   *
   * @param value The value of the angle in radians.
   */
  @JsonCreator
  public Rotation2d(@JsonProperty(required = true, value = "radians") double value) {
    m_value = value;
    m_cos = Math.cos(value);
    m_sin = Math.sin(value);
  }

  /**
   * Constructs a Rotation2d with the given x and y (cosine and sine) components.
   *
   * @param x The x component or cosine of the rotation.
   * @param y The y component or sine of the rotation.
   */
  public Rotation2d(double x, double y) {
    double magnitude = Math.hypot(x, y);
    if (magnitude > 1e-6) {
      m_cos = x / magnitude;
      m_sin = y / magnitude;
    } else {
      m_cos = 1.0;
      m_sin = 0.0;
      MathSharedStore.reportError(
          "x and y components of Rotation2d are zero\n", Thread.currentThread().getStackTrace());
    }
    m_value = Math.atan2(m_sin, m_cos);
  }

  /**
   * Constructs a Rotation2d with the given angle.
   *
   * @param angle The angle of the rotation.
   */
  public Rotation2d(Angle angle) {
    this(angle.in(Radians));
  }

  /**
   * Constructs a Rotation2d from a rotation matrix.
   *
   * @param rotationMatrix The rotation matrix.
   * @throws IllegalArgumentException if the rotation matrix isn't special orthogonal.
   */
  public Rotation2d(Matrix<N2, N2> rotationMatrix) {
    final var R = rotationMatrix;

    // Require that the rotation matrix is special orthogonal. This is true if
    // the matrix is orthogonal (RRᵀ = I) and normalized (determinant is 1).
    if (R.times(R.transpose()).minus(Matrix.eye(Nat.N2())).normF() > 1e-9) {
      var msg = "Rotation matrix isn't orthogonal\n\nR =\n" + R.getStorage().toString() + '\n';
      MathSharedStore.reportError(msg, Thread.currentThread().getStackTrace());
      throw new IllegalArgumentException(msg);
    }
    if (Math.abs(R.det() - 1.0) > 1e-9) {
      var msg =
          "Rotation matrix is orthogonal but not special orthogonal\n\nR =\n"
              + R.getStorage().toString()
              + '\n';
      MathSharedStore.reportError(msg, Thread.currentThread().getStackTrace());
      throw new IllegalArgumentException(msg);
    }

    // R = [cosθ  −sinθ]
    //     [sinθ   cosθ]
    m_cos = R.get(0, 0);
    m_sin = R.get(1, 0);

    m_value = Math.atan2(m_sin, m_cos);
  }

  /**
   * Constructs and returns a Rotation2d with the given radian value.
   *
   * @param radians The value of the angle in radians.
   * @return The rotation object with the desired angle value.
   */
  public static Rotation2d fromRadians(double radians) {
    return new Rotation2d(radians);
  }

  /**
   * Constructs and returns a Rotation2d with the given degree value.
   *
   * @param degrees The value of the angle in degrees.
   * @return The rotation object with the desired angle value.
   */
  public static Rotation2d fromDegrees(double degrees) {
    return new Rotation2d(Math.toRadians(degrees));
  }

  /**
   * Constructs and returns a Rotation2d with the given number of rotations.
   *
   * @param rotations The value of the angle in rotations.
   * @return The rotation object with the desired angle value.
   */
  public static Rotation2d fromRotations(double rotations) {
    return new Rotation2d(Units.rotationsToRadians(rotations));
  }

  /**
   * Adds two rotations together, with the result being bounded between -π and π.
   *
   * <p>For example, <code>Rotation2d.fromDegrees(30).plus(Rotation2d.fromDegrees(60))</code> equals
   * <code>Rotation2d(Math.PI/2.0)</code>
   *
   * @param other The rotation to add.
   * @return The sum of the two rotations.
   */
  public Rotation2d plus(Rotation2d other) {
    return rotateBy(other);
  }

  /**
   * Subtracts the new rotation from the current rotation and returns the new rotation.
   *
   * <p>For example, <code>Rotation2d.fromDegrees(10).minus(Rotation2d.fromDegrees(100))</code>
   * equals <code>Rotation2d(-Math.PI/2.0)</code>
   *
   * @param other The rotation to subtract.
   * @return The difference between the two rotations.
   */
  public Rotation2d minus(Rotation2d other) {
    return rotateBy(other.unaryMinus());
  }

  /**
   * Takes the inverse of the current rotation. This is simply the negative of the current angular
   * value.
   *
   * @return The inverse of the current rotation.
   */
  public Rotation2d unaryMinus() {
    return new Rotation2d(-m_value);
  }

  /**
   * Multiplies the current rotation by a scalar.
   *
   * @param scalar The scalar.
   * @return The new scaled Rotation2d.
   */
  public Rotation2d times(double scalar) {
    return new Rotation2d(m_value * scalar);
  }

  /**
   * Divides the current rotation by a scalar.
   *
   * @param scalar The scalar.
   * @return The new scaled Rotation2d.
   */
  public Rotation2d div(double scalar) {
    return times(1.0 / scalar);
  }

  /**
   * Adds the new rotation to the current rotation using a rotation matrix.
   *
   * <p>The matrix multiplication is as follows:
   *
   * <pre>
   * [cos_new]   [other.cos, -other.sin][cos]
   * [sin_new] = [other.sin,  other.cos][sin]
   * value_new = atan2(sin_new, cos_new)
   * </pre>
   *
   * @param other The rotation to rotate by.
   * @return The new rotated Rotation2d.
   */
  public Rotation2d rotateBy(Rotation2d other) {
    return new Rotation2d(
        m_cos * other.m_cos - m_sin * other.m_sin, m_cos * other.m_sin + m_sin * other.m_cos);
  }

  /**
   * Returns matrix representation of this rotation.
   *
   * @return Matrix representation of this rotation.
   */
  public Matrix<N2, N2> toMatrix() {
    // R = [cosθ  −sinθ]
    //     [sinθ   cosθ]
    return MatBuilder.fill(Nat.N2(), Nat.N2(), m_cos, -m_sin, m_sin, m_cos);
  }

  /**
   * Returns the measure of the Rotation2d.
   *
   * @return The measure of the Rotation2d.
   */
  public Angle getMeasure() {
    return Radians.of(getRadians());
  }

  /**
   * Returns the radian value of the Rotation2d.
   *
   * @return The radian value of the Rotation2d.
   * @see edu.wpi.first.math.MathUtil#angleModulus(double) to constrain the angle within (-π, π]
   */
  @JsonProperty
  public double getRadians() {
    return m_value;
  }

  /**
   * Returns the degree value of the Rotation2d.
   *
   * @return The degree value of the Rotation2d.
   * @see edu.wpi.first.math.MathUtil#inputModulus(double, double, double) to constrain the angle
   *     within (-180, 180]
   */
  public double getDegrees() {
    return Math.toDegrees(m_value);
  }

  /**
   * Returns the number of rotations of the Rotation2d.
   *
   * @return The number of rotations of the Rotation2d.
   */
  public double getRotations() {
    return Units.radiansToRotations(m_value);
  }

  /**
   * Returns the cosine of the Rotation2d.
   *
   * @return The cosine of the Rotation2d.
   */
  public double getCos() {
    return m_cos;
  }

  /**
   * Returns the sine of the Rotation2d.
   *
   * @return The sine of the Rotation2d.
   */
  public double getSin() {
    return m_sin;
  }

  /**
   * Returns the tangent of the Rotation2d.
   *
   * @return The tangent of the Rotation2d.
   */
  public double getTan() {
    return m_sin / m_cos;
  }

  @Override
  public String toString() {
    return String.format("Rotation2d(Rads: %.2f, Deg: %.2f)", m_value, Math.toDegrees(m_value));
  }

  /**
   * Checks equality between this Rotation2d and another object.
   *
   * @param obj The other object.
   * @return Whether the two objects are equal or not.
   */
  @Override
  public boolean equals(Object obj) {
    return obj instanceof Rotation2d other
        && Math.hypot(m_cos - other.m_cos, m_sin - other.m_sin) < 1E-9;
  }

  @Override
  public int hashCode() {
    return Objects.hash(m_value);
  }

  @Override
  public Rotation2d interpolate(Rotation2d endValue, double t) {
    return plus(endValue.minus(this).times(MathUtil.clamp(t, 0, 1)));
  }

  /** Rotation2d protobuf for serialization. */
  public static final Rotation2dProto proto = new Rotation2dProto();

  /** Rotation2d struct for serialization. */
  public static final Rotation2dStruct struct = new Rotation2dStruct();
}
