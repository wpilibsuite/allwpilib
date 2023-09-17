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
import edu.wpi.first.math.numbers.N3;
import java.util.Objects;

@JsonIgnoreProperties(ignoreUnknown = true)
@JsonAutoDetect(getterVisibility = JsonAutoDetect.Visibility.NONE)
public class Quaternion {
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
   * @return The quaternion sum.
   */
  public Quaternion minus(Quaternion other) {
    return new Quaternion(
        getW() - other.getW(), getX() - other.getX(), getY() - other.getY(), getZ() - other.getZ());
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
   * Divides by a scalar.
   *
   * @param scalar The value to scale each component by.
   * @return The scaled quaternion.
   */
  public Quaternion divide(double scalar) {
    return new Quaternion(getW() / scalar, getX() / scalar, getY() / scalar, getZ() / scalar);
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
    if (obj instanceof Quaternion) {
      var other = (Quaternion) obj;

      return Math.abs(
              getW() * other.getW()
                  + getX() * other.getX()
                  + getY() * other.getY()
                  + getZ() * other.getZ())
          > 1.0 - 1E-9;
    }
    return false;
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
   * Returns the inverse of the quaternion.
   *
   * @return The inverse quaternion.
   */
  public Quaternion inverse() {
    var norm = norm();
    return conjugate().times(1 / (norm * norm));
  }

  /**
   * Calculates the L2 norm of the quaternion.
   *
   * @return The L2 norm.
   */
  public double norm() {
    return Math.sqrt(getW() * getW() + getX() * getX() + getY() * getY() + getZ() * getZ());
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
   * Rational power of a quaternion
   *
   * @param t the power to raise this quaternion to.
   * @return The quaternion power
   */
  public Quaternion pow(double t) {
    // q^t = e^(ln(q^t)) = e^(t * ln(q))
    return this.log().times(t).exp();
  }

  /**
   * Matrix exponential of a quaternion
   *
   * @param adjustment the "Twist" that will be applied to this quaternion.
   * @return The quaternion product of exp(adjustment) * this
   */
  public Quaternion exp(Quaternion adjustment) {
    return adjustment.exp().times(this);
  }

  /**
   * Matrix exponential of a quaternion
   *
   * <p>source: https://en.wikipedia.org/wiki/Quaternion#Exponential,_logarithm,_and_power_functions
   *
   * @return The Matrix exponential of this quaternion.
   */
  public Quaternion exp() {
    // q = s(scalar) + v(vector)
    // exp(s)
    var scalar = Math.exp(m_w);

    // ||v||
    var axial_magnitude = Math.sqrt(getX() * getX() + getY() * getY() + getZ() * getZ());
    // cos(||v||)
    var cosine = Math.cos(axial_magnitude);

    double axial_scalar;

    if (axial_magnitude < 1e-9) {
      // Taylor series of sin(x) / x near x=0: 1 - x^2 / 6 + x^4 / 120 + O(n^6)
      var axial_magnitude_sq = axial_magnitude * axial_magnitude;
      var axial_magnitude_sq_sq = axial_magnitude_sq * axial_magnitude_sq;
      axial_scalar = 1 - axial_magnitude_sq / 6 + axial_magnitude_sq_sq / 120;
    } else {
      axial_scalar = Math.sin(axial_magnitude) / axial_magnitude;
    }

    // exp(s) * (cos(||v||) + v * sin(||v||) / ||v||)
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
   * <p>source: https://github.com/moble/quaternionic/blob/main/quaternionic/algebra.py#L233
   *
   * <p>For unit quaternions, this is equivalent to @see Quaternion#toRotationVector().
   *
   * @return The logarithm of this quaternion.
   */
  public Quaternion log() {
    var vnorm = Math.sqrt(getX() * getX() + getY() * getY() + getZ() * getZ());
    if (vnorm <= 1e-9 * Math.abs(getW())) {
      if (getW() < 0) {
        if (Math.abs(getW() + 1) > 1e-9) {
          return new Quaternion(Math.log(-getW()), Math.PI, 0, 0);
        } else {
          return new Quaternion(0, Math.PI, 0, 0);
        }
      } else {
        return new Quaternion(Math.log(getW()), 0, 0, 0);
      }
    } else {
      var v = Math.atan2(vnorm, getW());
      var f = v / vnorm;
      return new Quaternion(
          Math.log(getW() * getW() + vnorm * vnorm) / 2.0, f * getX(), f * getY(), f * getZ());
    }
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
}
