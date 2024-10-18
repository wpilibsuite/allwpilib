// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.geometry;

import com.fasterxml.jackson.annotation.JsonAutoDetect;
import com.fasterxml.jackson.annotation.JsonCreator;
import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import com.fasterxml.jackson.annotation.JsonProperty;
import edu.wpi.first.math.VecBuilder;
import edu.wpi.first.math.Vector;
import edu.wpi.first.math.geometry.proto.QuaternionProto;
import edu.wpi.first.math.geometry.struct.QuaternionStruct;
import edu.wpi.first.math.numbers.N3;
import edu.wpi.first.util.protobuf.ProtobufSerializable;
import edu.wpi.first.util.struct.StructSerializable;
import java.util.Objects;

/** Represents a quaternion. */
@JsonIgnoreProperties(ignoreUnknown = true)
@JsonAutoDetect(getterVisibility = JsonAutoDetect.Visibility.NONE)
public class Quaternion implements ProtobufSerializable, StructSerializable {
  // Scalar r in versor form
  private final double m_w;

  // Vector v in versor form
  private final double m_x;
  private final double m_y;
  private final double m_z;

  /** Constructs a quaternion with a default angle of 0 degrees. */
  public Quaternion() {
    m_w = 1.0;
    m_x = 0.0;
    m_y = 0.0;
    m_z = 0.0;
  }

  /**
   * Constructs a quaternion with the given components.
   *
   * @param w W component of the quaternion.
   * @param x X component of the quaternion.
   * @param y Y component of the quaternion.
   * @param z Z component of the quaternion.
   */
  @JsonCreator
  public Quaternion(
      @JsonProperty(required = true, value = "W") double w,
      @JsonProperty(required = true, value = "X") double x,
      @JsonProperty(required = true, value = "Y") double y,
      @JsonProperty(required = true, value = "Z") double z) {
    m_w = w;
    m_x = x;
    m_y = y;
    m_z = z;
  }

  /**
   * Adds another quaternion to this quaternion entrywise.
   *
   * @param other The other quaternion.
   * @return The quaternion sum.
   */
  public Quaternion plus(Quaternion other) {
    return new Quaternion(
        getW() + other.getW(), getX() + other.getX(), getY() + other.getY(), getZ() + other.getZ());
  }

  /**
   * Subtracts another quaternion from this quaternion entrywise.
   *
   * @param other The other quaternion.
   * @return The quaternion difference.
   */
  public Quaternion minus(Quaternion other) {
    return new Quaternion(
        getW() - other.getW(), getX() - other.getX(), getY() - other.getY(), getZ() - other.getZ());
  }

  /**
   * Divides by a scalar.
   *
   * @param scalar The value to scale each component by.
   * @return The scaled quaternion.
   */
  public Quaternion divide(double scalar) {
    return new Quaternion(getW() / scalar, getX() / scalar, getY() / scalar, getZ() / scalar);
  }

  /**
   * Multiplies with a scalar.
   *
   * @param scalar The value to scale each component by.
   * @return The scaled quaternion.
   */
  public Quaternion times(double scalar) {
    return new Quaternion(getW() * scalar, getX() * scalar, getY() * scalar, getZ() * scalar);
  }

  /**
   * Multiply with another quaternion.
   *
   * @param other The other quaternion.
   * @return The quaternion product.
   */
  public Quaternion times(Quaternion other) {
    // https://en.wikipedia.org/wiki/Quaternion#Scalar_and_vector_parts
    final var r1 = m_w;
    final var r2 = other.m_w;

    // v₁ ⋅ v₂
    double dot = m_x * other.m_x + m_y * other.m_y + m_z * other.m_z;

    // v₁ x v₂
    double cross_x = m_y * other.m_z - other.m_y * m_z;
    double cross_y = other.m_x * m_z - m_x * other.m_z;
    double cross_z = m_x * other.m_y - other.m_x * m_y;

    return new Quaternion(
        // r = r₁r₂ − v₁ ⋅ v₂
        r1 * r2 - dot,
        // v = r₁v₂ + r₂v₁ + v₁ x v₂
        r1 * other.m_x + r2 * m_x + cross_x,
        r1 * other.m_y + r2 * m_y + cross_y,
        r1 * other.m_z + r2 * m_z + cross_z);
  }

  @Override
  public String toString() {
    return String.format("Quaternion(%s, %s, %s, %s)", getW(), getX(), getY(), getZ());
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
    return Objects.hash(m_w, m_x, m_y, m_z);
  }

  /**
   * Returns the conjugate of the quaternion.
   *
   * @return The conjugate quaternion.
   */
  public Quaternion conjugate() {
    return new Quaternion(getW(), -getX(), -getY(), -getZ());
  }

  /**
   * Returns the elementwise product of two quaternions.
   *
   * @param other The other quaternion.
   * @return The dot product of two quaternions.
   */
  public double dot(final Quaternion other) {
    return getW() * other.getW()
        + getX() * other.getX()
        + getY() * other.getY()
        + getZ() * other.getZ();
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
      return new Quaternion(getW() / norm, getX() / norm, getY() / norm, getZ() / norm);
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
   * @param adjustment the "Twist" that will be applied to this quaternion.
   * @return The quaternion product of exp(adjustment) * this
   */
  public Quaternion exp(Quaternion adjustment) {
    return adjustment.exp().times(this);
  }

  /**
   * Matrix exponential of a quaternion.
   *
   * <p>source: wpimath/algorithms.md
   *
   * <p>If this quaternion is in 𝖘𝖔(3) and you are looking for an element of SO(3), use {@link
   * fromRotationVector}
   *
   * @return The Matrix exponential of this quaternion.
   */
  public Quaternion exp() {
    var scalar = Math.exp(getW());

    var axial_magnitude = Math.sqrt(getX() * getX() + getY() * getY() + getZ() * getZ());
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
        getX() * axial_scalar * scalar,
        getY() * axial_scalar * scalar,
        getZ() * axial_scalar * scalar);
  }

  /**
   * Log operator of a quaternion.
   *
   * @param end The quaternion to map this quaternion onto.
   * @return The "Twist" that maps this quaternion to the argument.
   */
  public Quaternion log(Quaternion end) {
    return end.times(this.inverse()).log();
  }

  /**
   * The Log operator of a general quaternion.
   *
   * <p>source: wpimath/algorithms.md
   *
   * <p>If this quaternion is in SO(3) and you are looking for an element of 𝖘𝖔(3), use {@link
   * toRotationVector}
   *
   * @return The logarithm of this quaternion.
   */
  public Quaternion log() {
    var norm = norm();
    var scalar = Math.log(norm);

    var v_norm = Math.sqrt(getX() * getX() + getY() * getY() + getZ() * getZ());

    var s_norm = getW() / norm;

    if (Math.abs(s_norm + 1) < 1e-9) {
      return new Quaternion(scalar, -Math.PI, 0, 0);
    }

    double v_scalar;

    if (v_norm < 1e-9) {
      // Taylor series expansion of atan2(y/x)/y at y = 0:
      //
      //   1/x - 1/3 y²/x³ + O(y⁴)
      v_scalar = 1.0 / getW() - 1.0 / 3.0 * v_norm * v_norm / (getW() * getW() * getW());
    } else {
      v_scalar = Math.atan2(v_norm, getW()) / v_norm;
    }

    return new Quaternion(scalar, v_scalar * getX(), v_scalar * getY(), v_scalar * getZ());
  }

  /**
   * Returns W component of the quaternion.
   *
   * @return W component of the quaternion.
   */
  @JsonProperty(value = "W")
  public double getW() {
    return m_w;
  }

  /**
   * Returns X component of the quaternion.
   *
   * @return X component of the quaternion.
   */
  @JsonProperty(value = "X")
  public double getX() {
    return m_x;
  }

  /**
   * Returns Y component of the quaternion.
   *
   * @return Y component of the quaternion.
   */
  @JsonProperty(value = "Y")
  public double getY() {
    return m_y;
  }

  /**
   * Returns Z component of the quaternion.
   *
   * @return Z component of the quaternion.
   */
  @JsonProperty(value = "Z")
  public double getZ() {
    return m_z;
  }

  /**
   * Returns the quaternion representation of this rotation vector.
   *
   * <p>This is also the exp operator of 𝖘𝖔(3).
   *
   * <p>source: wpimath/algorithms.md
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
    double norm = Math.sqrt(getX() * getX() + getY() * getY() + getZ() * getZ());

    double coeff;
    if (norm < 1e-9) {
      coeff = 2.0 / getW() - 2.0 / 3.0 * norm * norm / (getW() * getW() * getW());
    } else {
      if (getW() < 0.0) {
        coeff = 2.0 * Math.atan2(-norm, -getW()) / norm;
      } else {
        coeff = 2.0 * Math.atan2(norm, getW()) / norm;
      }
    }

    return VecBuilder.fill(coeff * getX(), coeff * getY(), coeff * getZ());
  }

  /** Quaternion protobuf for serialization. */
  public static final QuaternionProto proto = new QuaternionProto();

  /** Quaternion struct for serialization. */
  public static final QuaternionStruct struct = new QuaternionStruct();
}
