// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.geometry;

import static edu.wpi.first.units.Units.Meters;

import com.fasterxml.jackson.annotation.JsonAutoDetect;
import com.fasterxml.jackson.annotation.JsonCreator;
import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import com.fasterxml.jackson.annotation.JsonProperty;
import edu.wpi.first.math.MathUtil;
import edu.wpi.first.math.VecBuilder;
import edu.wpi.first.math.Vector;
import edu.wpi.first.math.geometry.proto.Translation3dProto;
import edu.wpi.first.math.geometry.struct.Translation3dStruct;
import edu.wpi.first.math.interpolation.Interpolatable;
import edu.wpi.first.math.numbers.N3;
import edu.wpi.first.units.measure.Distance;
import edu.wpi.first.util.protobuf.ProtobufSerializable;
import edu.wpi.first.util.struct.StructSerializable;
import java.util.Objects;

/**
 * Represents a translation in 3D space. This object can be used to represent a point or a vector.
 *
 * <p>This assumes that you are using conventional mathematical axes. When the robot is at the
 * origin facing in the positive X direction, forward is positive X, left is positive Y, and up is
 * positive Z.
 */
@JsonIgnoreProperties(ignoreUnknown = true)
@JsonAutoDetect(getterVisibility = JsonAutoDetect.Visibility.NONE)
public class Translation3d
    implements Interpolatable<Translation3d>, ProtobufSerializable, StructSerializable {
  /**
   * A preallocated Translation3d representing the origin.
   *
   * <p>This exists to avoid allocations for common translations.
   */
  public static final Translation3d kZero = new Translation3d();

  private final double m_x;
  private final double m_y;
  private final double m_z;

  /** Constructs a Translation3d with X, Y, and Z components equal to zero. */
  public Translation3d() {
    this(0.0, 0.0, 0.0);
  }

  /**
   * Constructs a Translation3d with the X, Y, and Z components equal to the provided values.
   *
   * @param x The x component of the translation.
   * @param y The y component of the translation.
   * @param z The z component of the translation.
   */
  @JsonCreator
  public Translation3d(
      @JsonProperty(required = true, value = "x") double x,
      @JsonProperty(required = true, value = "y") double y,
      @JsonProperty(required = true, value = "z") double z) {
    m_x = x;
    m_y = y;
    m_z = z;
  }

  /**
   * Constructs a Translation3d with the provided distance and angle. This is essentially converting
   * from polar coordinates to Cartesian coordinates.
   *
   * @param distance The distance from the origin to the end of the translation.
   * @param angle The angle between the x-axis and the translation vector.
   */
  public Translation3d(double distance, Rotation3d angle) {
    final var rectangular = new Translation3d(distance, 0.0, 0.0).rotateBy(angle);
    m_x = rectangular.getX();
    m_y = rectangular.getY();
    m_z = rectangular.getZ();
  }

  /**
   * Constructs a Translation3d with the X, Y, and Z components equal to the provided values. The
   * components will be converted to and tracked as meters.
   *
   * @param x The x component of the translation.
   * @param y The y component of the translation.
   * @param z The z component of the translation.
   */
  public Translation3d(Distance x, Distance y, Distance z) {
    this(x.in(Meters), y.in(Meters), z.in(Meters));
  }

  /**
   * Constructs a 3D translation from a 2D translation in the X-Y plane.
   *
   * @param translation The 2D translation.
   * @see Pose3d#Pose3d(Pose2d)
   * @see Transform3d#Transform3d(Transform2d)
   */
  public Translation3d(Translation2d translation) {
    this(translation.getX(), translation.getY(), 0.0);
  }

  /**
   * Constructs a Translation3d from a 3D translation vector. The values are assumed to be in
   * meters.
   *
   * @param vector The translation vector.
   */
  public Translation3d(Vector<N3> vector) {
    this(vector.get(0), vector.get(1), vector.get(2));
  }

  /**
   * Calculates the distance between two translations in 3D space.
   *
   * <p>The distance between translations is defined as √((x₂−x₁)²+(y₂−y₁)²+(z₂−z₁)²).
   *
   * @param other The translation to compute the distance to.
   * @return The distance between the two translations.
   */
  public double getDistance(Translation3d other) {
    return Math.sqrt(
        Math.pow(other.m_x - m_x, 2) + Math.pow(other.m_y - m_y, 2) + Math.pow(other.m_z - m_z, 2));
  }

  /**
   * Returns the X component of the translation.
   *
   * @return The X component of the translation.
   */
  @JsonProperty
  public double getX() {
    return m_x;
  }

  /**
   * Returns the Y component of the translation.
   *
   * @return The Y component of the translation.
   */
  @JsonProperty
  public double getY() {
    return m_y;
  }

  /**
   * Returns the Z component of the translation.
   *
   * @return The Z component of the translation.
   */
  @JsonProperty
  public double getZ() {
    return m_z;
  }

  /**
   * Returns the X component of the translation in a measure.
   *
   * @return The x component of the translation in a measure.
   */
  public Distance getMeasureX() {
    return Meters.of(m_x);
  }

  /**
   * Returns the Y component of the translation in a measure.
   *
   * @return The y component of the translation in a measure.
   */
  public Distance getMeasureY() {
    return Meters.of(m_y);
  }

  /**
   * Returns the Z component of the translation in a measure.
   *
   * @return The z component of the translation in a measure.
   */
  public Distance getMeasureZ() {
    return Meters.of(m_z);
  }

  /**
   * Returns a 2D translation vector representation of this translation.
   *
   * @return A 2D translation vector representation of this translation.
   */
  public Vector<N3> toVector() {
    return VecBuilder.fill(m_x, m_y, m_z);
  }

  /**
   * Returns the norm, or distance from the origin to the translation.
   *
   * @return The norm of the translation.
   */
  public double getNorm() {
    return Math.sqrt(m_x * m_x + m_y * m_y + m_z * m_z);
  }

  /**
   * Applies a rotation to the translation in 3D space.
   *
   * <p>For example, rotating a Translation3d of &lt;2, 0, 0&gt; by 90 degrees around the Z axis
   * will return a Translation3d of &lt;0, 2, 0&gt;.
   *
   * @param other The rotation to rotate the translation by.
   * @return The new rotated translation.
   */
  public Translation3d rotateBy(Rotation3d other) {
    final var p = new Quaternion(0.0, m_x, m_y, m_z);
    final var qprime = other.getQuaternion().times(p).times(other.getQuaternion().inverse());
    return new Translation3d(qprime.getX(), qprime.getY(), qprime.getZ());
  }

  /**
   * Rotates this translation around another translation in 3D space.
   *
   * @param other The other translation to rotate around.
   * @param rot The rotation to rotate the translation by.
   * @return The new rotated translation.
   */
  public Translation3d rotateAround(Translation3d other, Rotation3d rot) {
    return this.minus(other).rotateBy(rot).plus(other);
  }

  /**
   * Returns a Translation2d representing this Translation3d projected into the X-Y plane.
   *
   * @return A Translation2d representing this Translation3d projected into the X-Y plane.
   */
  public Translation2d toTranslation2d() {
    return new Translation2d(m_x, m_y);
  }

  /**
   * Returns the sum of two translations in 3D space.
   *
   * <p>For example, Translation3d(1.0, 2.5, 3.5) + Translation3d(2.0, 5.5, 7.5) =
   * Translation3d{3.0, 8.0, 11.0).
   *
   * @param other The translation to add.
   * @return The sum of the translations.
   */
  public Translation3d plus(Translation3d other) {
    return new Translation3d(m_x + other.m_x, m_y + other.m_y, m_z + other.m_z);
  }

  /**
   * Returns the difference between two translations.
   *
   * <p>For example, Translation3d(5.0, 4.0, 3.0) - Translation3d(1.0, 2.0, 3.0) =
   * Translation3d(4.0, 2.0, 0.0).
   *
   * @param other The translation to subtract.
   * @return The difference between the two translations.
   */
  public Translation3d minus(Translation3d other) {
    return new Translation3d(m_x - other.m_x, m_y - other.m_y, m_z - other.m_z);
  }

  /**
   * Returns the inverse of the current translation. This is equivalent to negating all components
   * of the translation.
   *
   * @return The inverse of the current translation.
   */
  public Translation3d unaryMinus() {
    return new Translation3d(-m_x, -m_y, -m_z);
  }

  /**
   * Returns the translation multiplied by a scalar.
   *
   * <p>For example, Translation3d(2.0, 2.5, 4.5) * 2 = Translation3d(4.0, 5.0, 9.0).
   *
   * @param scalar The scalar to multiply by.
   * @return The scaled translation.
   */
  public Translation3d times(double scalar) {
    return new Translation3d(m_x * scalar, m_y * scalar, m_z * scalar);
  }

  /**
   * Returns the translation divided by a scalar.
   *
   * <p>For example, Translation3d(2.0, 2.5, 4.5) / 2 = Translation3d(1.0, 1.25, 2.25).
   *
   * @param scalar The scalar to multiply by.
   * @return The reference to the new mutated object.
   */
  public Translation3d div(double scalar) {
    return new Translation3d(m_x / scalar, m_y / scalar, m_z / scalar);
  }

  @Override
  public String toString() {
    return String.format("Translation3d(X: %.2f, Y: %.2f, Z: %.2f)", m_x, m_y, m_z);
  }

  /**
   * Checks equality between this Translation3d and another object.
   *
   * @param obj The other object.
   * @return Whether the two objects are equal or not.
   */
  @Override
  public boolean equals(Object obj) {
    return obj instanceof Translation3d other
        && Math.abs(other.m_x - m_x) < 1E-9
        && Math.abs(other.m_y - m_y) < 1E-9
        && Math.abs(other.m_z - m_z) < 1E-9;
  }

  @Override
  public int hashCode() {
    return Objects.hash(m_x, m_y, m_z);
  }

  @Override
  public Translation3d interpolate(Translation3d endValue, double t) {
    return new Translation3d(
        MathUtil.interpolate(this.getX(), endValue.getX(), t),
        MathUtil.interpolate(this.getY(), endValue.getY(), t),
        MathUtil.interpolate(this.getZ(), endValue.getZ(), t));
  }

  /** Translation3d protobuf for serialization. */
  public static final Translation3dProto proto = new Translation3dProto();

  /** Translation3d struct for serialization. */
  public static final Translation3dStruct struct = new Translation3dStruct();
}
