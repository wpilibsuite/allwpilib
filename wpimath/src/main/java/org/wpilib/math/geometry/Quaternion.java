// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.geometry;

import io.avaje.jsonb.Json;
import java.util.Objects;
import org.wpilib.math.geometry.proto.QuaternionProto;
import org.wpilib.math.geometry.struct.QuaternionStruct;
import org.wpilib.math.linalg.VecBuilder;
import org.wpilib.math.linalg.Vector;
import org.wpilib.math.numbers.N3;
import org.wpilib.util.protobuf.ProtobufSerializable;
import org.wpilib.util.struct.StructSerializable;

/** Represents a quaternion. */
@Json
public final class Quaternion implements ProtobufSerializable, StructSerializable {
  /** W component of the quaternion (scalar r in versor form). */
  @Json.Property("W")
  public final double w;

  /** X component of the quaternion (vector v in versor form). */
  @Json.Property("X")
  public final double x;

  /** Y component of the quaternion (vector v in versor form). */
  @Json.Property("Y")
  public final double y;

  /** Z component of the quaternion (vector v in versor form). */
  @Json.Property("Z")
  public final double z;

  /** Constructs a quaternion with a default angle of 0 degrees. */
  public Quaternion() {
    this(1.0, 0.0, 0.0, 0.0);
  }

  /**
   * Constructs a quaternion with the given components.
   *
   * @param w W component of the quaternion.
   * @param x X component of the quaternion.
   * @param y Y component of the quaternion.
   * @param z Z component of the quaternion.
   */
  @Json.Creator
  public Quaternion(
      @Json.Alias("W") double w,
      @Json.Alias("X") double x,
      @Json.Alias("Y") double y,
      @Json.Alias("Z") double z) {
    this.w = w;
    this.x = x;
    this.y = y;
    this.z = z;
  }

  /**
   * Adds another quaternion to this quaternion entrywise.
   *
   * @param other The other quaternion.
   * @return The quaternion sum.
   */
  public Quaternion plus(Quaternion other) {
    return new Quaternion(w + other.w, x + other.x, y + other.y, z + other.z);
  }

  /**
   * Subtracts another quaternion from this quaternion entrywise.
   *
   * @param other The other quaternion.
   * @return The quaternion difference.
   */
  public Quaternion minus(Quaternion other) {
    return new Quaternion(w - other.w, x - other.x, y - other.y, z - other.z);
  }

  /**
   * Divides by a scalar.
   *
   * @param scalar The value to scale each component by.
   * @return The scaled quaternion.
   */
  public Quaternion divide(double scalar) {
    return new Quaternion(w / scalar, x / scalar, y / scalar, z / scalar);
  }

  /**
   * Multiplies with a scalar.
   *
   * @param scalar The value to scale each component by.
   * @return The scaled quaternion.
   */
  public Quaternion times(double scalar) {
    return new Quaternion(w * scalar, x * scalar, y * scalar, z * scalar);
  }

  /**
   * Multiply with another quaternion.
   *
   * @param other The other quaternion.
   * @return The quaternion product.
   */
  public Quaternion times(Quaternion other) {
    // https://en.wikipedia.org/wiki/Quaternion#Scalar_and_vector_parts
    final var r1 = w;
    final var r2 = other.w;

    // v₁ ⋅ v₂
    double dot = x * other.x + y * other.y + z * other.z;

    // v₁ x v₂
    double cross_x = y * other.z - other.y * z;
    double cross_y = other.x * z - x * other.z;
    double cross_z = x * other.y - other.x * y;

    return new Quaternion(
        // r = r₁r₂ − v₁ ⋅ v₂
        r1 * r2 - dot,
        // v = r₁v₂ + r₂v₁ + v₁ x v₂
        r1 * other.x + r2 * x + cross_x,
        r1 * other.y + r2 * y + cross_y,
        r1 * other.z + r2 * z + cross_z);
  }

  @Override
  public String toString() {
    return String.format("Quaternion(%s, %s, %s, %s)", w, x, y, z);
  }

  /**
   * Checks equality between this Quaternion and another object.
   *
   * @param obj The other object.
   * @return Whether the two objects are equal or not.
   */
  @Override
  public boolean equals(Object obj) {
    return obj instanceof Quaternion other
        && Math.abs(dot(other) - norm() * other.norm()) < 1e-9
        && Math.abs(norm() - other.norm()) < 1e-9;
  }

  @Override
  public int hashCode() {
    return Objects.hash(w, x, y, z);
  }

  /**
   * Returns the conjugate of the quaternion.
   *
   * @return The conjugate quaternion.
   */
  public Quaternion conjugate() {
    return new Quaternion(w, -x, -y, -z);
  }

  /**
   * Returns the elementwise product of two quaternions.
   *
   * @param other The other quaternion.
   * @return The dot product of two quaternions.
   */
  public double dot(final Quaternion other) {
    return w * other.w + x * other.x + y * other.y + z * other.z;
  }

  /**
   * Returns the inverse of the quaternion.
   *
   * @return The inverse quaternion.
   */
  public Quaternion inverse() {
    var norm = norm();
    return conjugate().divide(norm * norm);
  }

  /**
   * Calculates the L2 norm of the quaternion.
   *
   * @return The L2 norm.
   */
  public double norm() {
    return Math.sqrt(dot(this));
  }

  /**
   * Normalizes the quaternion.
   *
   * @return The normalized quaternion.
   */
  public Quaternion normalize() {
    double norm = norm();
    if (norm == 0.0) {
      return new Quaternion();
    } else {
      return new Quaternion(w / norm, x / norm, y / norm, z / norm);
    }
  }

  /**
   * Rational power of a quaternion.
   *
   * @param t the power to raise this quaternion to.
   * @return The quaternion power
   */
  public Quaternion pow(double t) {
    // q^t = e^(ln(q^t)) = e^(t * ln(q))
    return this.log().times(t).exp();
  }

  /**
   * Matrix exponential of a quaternion.
   *
   * <p>source: wpimath/docs/Quaternion.md
   *
   * <p>If this quaternion is in 𝖘𝖔(3) and you are looking for an element of SO(3), use {@link
   * fromRotationVector}
   *
   * @return The Matrix exponential of this quaternion.
   */
  public Quaternion exp() {
    var scalar = Math.exp(w);

    var axial_magnitude = Math.sqrt(x * x + y * y + z * z);
    var cosine = Math.cos(axial_magnitude);

    double axial_scalar;

    if (axial_magnitude < 1e-9) {
      // Taylor series of sin(θ) / θ near θ = 0: 1 − θ²/6 + θ⁴/120 + O(n⁶)
      var axial_magnitude_sq = axial_magnitude * axial_magnitude;
      var axial_magnitude_sq_sq = axial_magnitude_sq * axial_magnitude_sq;
      axial_scalar = 1.0 - axial_magnitude_sq / 6.0 + axial_magnitude_sq_sq / 120.0;
    } else {
      axial_scalar = Math.sin(axial_magnitude) / axial_magnitude;
    }

    return new Quaternion(
        cosine * scalar,
        x * axial_scalar * scalar,
        y * axial_scalar * scalar,
        z * axial_scalar * scalar);
  }

  /**
   * The Log operator of a general quaternion.
   *
   * <p>source: wpimath/docs/Quaternion.md
   *
   * <p>If this quaternion is in SO(3) and you are looking for an element of 𝖘𝖔(3), use {@link
   * toRotationVector}
   *
   * @return The logarithm of this quaternion.
   */
  public Quaternion log() {
    var norm = norm();
    if (norm == 0.0) {
      return new Quaternion(0.0, 0.0, 0.0, 0.0);
    }

    var scalar = Math.log(norm);

    var v_norm = Math.sqrt(x * x + y * y + z * z);

    var s_norm = w / norm;

    if (Math.abs(s_norm + 1) < 1e-9) {
      return new Quaternion(scalar, -Math.PI, 0, 0);
    }

    double v_scalar;

    if (v_norm < 1e-9 && w != 0.0) {
      // Taylor series expansion of atan2(y/x)/y at y = 0:
      //
      //   1/x - 1/3 y²/x³ + O(y⁴)
      v_scalar = 1.0 / w - 1.0 / 3.0 * v_norm * v_norm / (w * w * w);
    } else if (v_norm == 0.0) {
      v_scalar = 0.0;
    } else {
      v_scalar = Math.atan2(v_norm, w) / v_norm;
    }

    return new Quaternion(scalar, v_scalar * x, v_scalar * y, v_scalar * z);
  }

  /**
   * Returns the quaternion representation of this rotation vector.
   *
   * <p>This is also the exp operator of 𝖘𝖔(3).
   *
   * <p>source: wpimath/docs/Quaternion.md
   *
   * @param rvec The rotation vector.
   * @return The quaternion representation of this rotation vector.
   */
  public static Quaternion fromRotationVector(Vector<N3> rvec) {
    double theta = rvec.norm();

    double cos = Math.cos(theta / 2);

    double axial_scalar;

    if (theta < 1e-9) {
      // taylor series expansion of sin(θ/2) / θ = 1/2 - θ²/48 + O(θ⁴)
      axial_scalar = 1.0 / 2.0 - theta * theta / 48.0;
    } else {
      axial_scalar = Math.sin(theta / 2) / theta;
    }

    return new Quaternion(
        cos,
        axial_scalar * rvec.get(0, 0),
        axial_scalar * rvec.get(1, 0),
        axial_scalar * rvec.get(2, 0));
  }

  /**
   * Returns the rotation vector representation of this quaternion.
   *
   * <p>This is also the log operator of SO(3).
   *
   * @return The rotation vector representation of this quaternion.
   */
  public Vector<N3> toRotationVector() {
    // See equation (31) in "Integrating Generic Sensor Fusion Algorithms with
    // Sound State Representation through Encapsulation of Manifolds"
    //
    // https://arxiv.org/pdf/1107.1119.pdf
    double norm = Math.sqrt(x * x + y * y + z * z);

    double coeff;
    if (norm < 1e-9) {
      coeff = 2.0 / w - 2.0 / 3.0 * norm * norm / (w * w * w);
    } else {
      if (w < 0.0) {
        coeff = 2.0 * Math.atan2(-norm, -w) / norm;
      } else {
        coeff = 2.0 * Math.atan2(norm, w) / norm;
      }
    }

    return VecBuilder.fill(coeff * x, coeff * y, coeff * z);
  }

  /** Quaternion protobuf for serialization. */
  public static final QuaternionProto proto = new QuaternionProto();

  /** Quaternion struct for serialization. */
  public static final QuaternionStruct struct = new QuaternionStruct();
}
