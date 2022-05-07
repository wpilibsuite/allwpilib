// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.geometry;

import edu.wpi.first.math.VecBuilder;
import edu.wpi.first.math.Vector;
import edu.wpi.first.math.numbers.N3;
import java.util.Objects;

public class Quaternion {
  private final double m_r;
  private final Vector<N3> m_v;

  /** Constructs a quaternion with a default angle of 0 degrees. */
  public Quaternion() {
    m_r = 1.0;
    m_v = VecBuilder.fill(0.0, 0.0, 0.0);
  }

  /**
   * Constructs a quaternion with the given components.
   *
   * @param w W component of the quaternion.
   * @param x X component of the quaternion.
   * @param y Y component of the quaternion.
   * @param z Z component of the quaternion.
   */
  public Quaternion(double w, double x, double y, double z) {
    m_r = w;
    m_v = VecBuilder.fill(x, y, z);
  }

  /**
   * Multiply with another quaternion.
   *
   * @param other The other quaternion.
   * @return The quaternion product.
   */
  public Quaternion times(Quaternion other) {
    // https://en.wikipedia.org/wiki/Quaternion#Scalar_and_vector_parts
    final var r1 = m_r;
    final var v1 = m_v;
    final var r2 = other.m_r;
    final var v2 = other.m_v;

    final var v1x = v1.get(0, 0);
    final var v1y = v1.get(1, 0);
    final var v1z = v1.get(2, 0);

    final var v2x = v2.get(0, 0);
    final var v2y = v2.get(1, 0);
    final var v2z = v2.get(2, 0);

    var cross =
        VecBuilder.fill(v1y * v2z - v2y * v1z, v2x * v1z - v1x * v2z, v1x * v2y - v2x * v1y);
    double dot = v1x * v2x + v1y * v2y + v1z * v2z;

    final var v = v2.times(r1).plus(v1.times(r2)).plus(cross);
    return new Quaternion(r1 * r2 - dot, v.get(0, 0), v.get(1, 0), v.get(2, 0));
  }

  @Override
  public String toString() {
    return String.format(
        "Quaternion(%s, %s, %s, %s)", m_r, m_v.get(0, 0), m_v.get(1, 0), m_v.get(2, 0));
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

      final var r1 = m_r;
      final var v1 = m_v;
      final var r2 = other.m_r;
      final var v2 = other.m_v;

      final var v1x = v1.get(0, 0);
      final var v1y = v1.get(1, 0);
      final var v1z = v1.get(2, 0);

      final var v2x = v2.get(0, 0);
      final var v2y = v2.get(1, 0);
      final var v2z = v2.get(2, 0);

      return Math.abs(r1 * r2 + v1x * v2x + v1y * v2y + v1z * v2z) > 1.0 - 1E-9;
    }
    return false;
  }

  @Override
  public int hashCode() {
    return Objects.hash(m_r, m_v);
  }

  /**
   * Returns the inverse of the quaternion.
   *
   * @return The inverse quaternion.
   */
  public Quaternion inverse() {
    return new Quaternion(m_r, -m_v.get(0, 0), -m_v.get(1, 0), -m_v.get(2, 0));
  }

  /**
   * Normalizes the quaternion.
   *
   * @return The normalized quaternion.
   */
  public Quaternion normalize() {
    double norm = Math.sqrt(getW() * getW() + getX() * getX() + getY() * getY() + getZ() * getZ());
    if (norm == 0.0) {
      return new Quaternion();
    } else {
      return new Quaternion(getW() / norm, getX() / norm, getY() / norm, getZ() / norm);
    }
  }

  /**
   * Returns W component of the quaternion.
   *
   * @return W component of the quaternion.
   */
  public double getW() {
    return m_r;
  }

  /**
   * Returns X component of the quaternion.
   *
   * @return X component of the quaternion.
   */
  public double getX() {
    return m_v.get(0, 0);
  }

  /**
   * Returns Y component of the quaternion.
   *
   * @return Y component of the quaternion.
   */
  public double getY() {
    return m_v.get(1, 0);
  }

  /**
   * Returns Z component of the quaternion.
   *
   * @return Z component of the quaternion.
   */
  public double getZ() {
    return m_v.get(2, 0);
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
    double norm = m_v.normF();

    if (norm < 1e-9) {
      return m_v.times(2.0 / getW() - 2.0 / 3.0 * norm * norm / (getW() * getW() * getW()));
    } else {
      if (getW() < 0.0) {
        return m_v.times(2.0 * Math.atan2(-norm, -getW()) / norm);
      } else {
        return m_v.times(2.0 * Math.atan2(norm, getW()) / norm);
      }
    }
  }
}
