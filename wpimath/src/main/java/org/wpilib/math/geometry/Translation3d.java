// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.geometry;

import static org.wpilib.units.Units.Meters;

import io.avaje.jsonb.Json;
import java.util.Collection;
import java.util.Collections;
import java.util.Comparator;
import java.util.Objects;
import org.wpilib.math.geometry.proto.Translation3dProto;
import org.wpilib.math.geometry.struct.Translation3dStruct;
import org.wpilib.math.interpolation.Interpolatable;
import org.wpilib.math.linalg.VecBuilder;
import org.wpilib.math.linalg.Vector;
import org.wpilib.math.numbers.N3;
import org.wpilib.math.util.MathUtil;
import org.wpilib.units.measure.Distance;
import org.wpilib.util.protobuf.ProtobufSerializable;
import org.wpilib.util.struct.StructSerializable;

/**
 * Represents a translation in 3D space. This object can be used to represent a point or a vector.
 *
 * <p>This assumes that you are using conventional mathematical axes. When the robot is at the
 * origin facing in the positive X direction, forward is positive X, left is positive Y, and up is
 * positive Z.
 */
@Json
public final class Translation3d
    implements Interpolatable<Translation3d>, ProtobufSerializable, StructSerializable {
  /**
   * A preallocated Translation3d representing the origin.
   *
   * <p>This exists to avoid allocations for common translations.
   */
  public static final Translation3d ZERO = new Translation3d();

  /** The X component of the translation. */
  @Json.Property("x")
  public final double x;

  /** The Y component of the translation. */
  @Json.Property("y")
  public final double y;

  /** The Z component of the translation. */
  @Json.Property("z")
  public final double z;

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
  @Json.Creator
  public Translation3d(double x, double y, double z) {
    this.x = x;
    this.y = y;
    this.z = z;
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
    x = rectangular.x;
    y = rectangular.y;
    z = rectangular.z;
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
    this(translation.x, translation.y, 0.0);
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
    double dx = other.x - x;
    double dy = other.y - y;
    double dz = other.z - z;
    return Math.sqrt(dx * dx + dy * dy + dz * dz);
  }

  /**
   * Calculates the squared distance between two translations in 3D space. This is equivalent to
   * squaring the result of {@link #getDistance(Translation3d)}, but avoids computing a square root.
   *
   * <p>The squared distance between translations is defined as (x₂−x₁)²+(y₂−y₁)²+(z₂−z₁)².
   *
   * @param other The translation to compute the squared distance to.
   * @return The squared distance between the two translations.
   */
  public double getSquaredDistance(Translation3d other) {
    double dx = other.x - x;
    double dy = other.y - y;
    double dz = other.z - z;
    return dx * dx + dy * dy + dz * dz;
  }

  /**
   * Returns the X component of the translation in a measure.
   *
   * @return The x component of the translation in a measure.
   */
  public Distance getMeasureX() {
    return Meters.of(x);
  }

  /**
   * Returns the Y component of the translation in a measure.
   *
   * @return The y component of the translation in a measure.
   */
  public Distance getMeasureY() {
    return Meters.of(y);
  }

  /**
   * Returns the Z component of the translation in a measure.
   *
   * @return The z component of the translation in a measure.
   */
  public Distance getMeasureZ() {
    return Meters.of(z);
  }

  /**
   * Returns a 2D translation vector representation of this translation.
   *
   * @return A 2D translation vector representation of this translation.
   */
  public Vector<N3> toVector() {
    return VecBuilder.fill(x, y, z);
  }

  /**
   * Returns the norm, or distance from the origin to the translation.
   *
   * @return The norm of the translation.
   */
  public double getNorm() {
    return Math.sqrt(x * x + y * y + z * z);
  }

  /**
   * Returns the squared norm, or squared distance from the origin to the translation. This is
   * equivalent to squaring the result of {@link #getNorm()}, but avoids computing a square root.
   *
   * @return The squared norm of the translation.
   */
  public double getSquaredNorm() {
    return x * x + y * y + z * z;
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
    final var p = new Quaternion(0.0, x, y, z);
    final var qprime = other.quaternion.times(p).times(other.quaternion.inverse());
    return new Translation3d(qprime.x, qprime.y, qprime.z);
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
   * Computes the dot product between this translation and another translation in 3D space.
   *
   * <p>The dot product between two translations is defined as x₁x₂+y₁y₂+z₁z₂.
   *
   * @param other The translation to compute the dot product with.
   * @return The dot product between the two translations, in square meters.
   */
  public double dot(Translation3d other) {
    return x * other.x + y * other.y + z * other.z;
  }

  /**
   * Computes the cross product between this translation and another translation in 3D space. The
   * resulting translation will be perpendicular to both translations.
   *
   * <p>The 3D cross product between two translations is defined as &lt;y₁z₂-y₂z₁, z₁x₂-z₂x₁,
   * x₁y₂-x₂y₁&gt;.
   *
   * @param other The translation to compute the cross product with.
   * @return The cross product between the two translations.
   */
  public Vector<N3> cross(Translation3d other) {
    return VecBuilder.fill(
        y * other.z - other.y * z, z * other.x - other.z * x, x * other.y - other.x * y);
  }

  /**
   * Returns a Translation2d representing this Translation3d projected into the X-Y plane.
   *
   * @return A Translation2d representing this Translation3d projected into the X-Y plane.
   */
  public Translation2d toTranslation2d() {
    return new Translation2d(x, y);
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
    return new Translation3d(x + other.x, y + other.y, z + other.z);
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
    return new Translation3d(x - other.x, y - other.y, z - other.z);
  }

  /**
   * Returns the inverse of the current translation. This is equivalent to negating all components
   * of the translation.
   *
   * @return The inverse of the current translation.
   */
  public Translation3d unaryMinus() {
    return new Translation3d(-x, -y, -z);
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
    return new Translation3d(x * scalar, y * scalar, z * scalar);
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
    return new Translation3d(x / scalar, y / scalar, z / scalar);
  }

  /**
   * Returns the nearest Translation3d from a collection of translations.
   *
   * @param translations The collection of translations to find the nearest.
   * @return The nearest Translation3d from the collection.
   */
  public Translation3d nearest(Collection<Translation3d> translations) {
    return Collections.min(translations, Comparator.comparing(this::getDistance));
  }

  @Override
  public String toString() {
    return String.format("Translation3d(X: %.2f, Y: %.2f, Z: %.2f)", x, y, z);
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
        && Math.abs(other.x - x) < 1E-9
        && Math.abs(other.y - y) < 1E-9
        && Math.abs(other.z - z) < 1E-9;
  }

  @Override
  public int hashCode() {
    return Objects.hash(x, y, z);
  }

  @Override
  public Translation3d interpolate(Translation3d endValue, double t) {
    return new Translation3d(
        MathUtil.lerp(this.x, endValue.x, t),
        MathUtil.lerp(this.y, endValue.y, t),
        MathUtil.lerp(this.z, endValue.z, t));
  }

  /** Translation3d protobuf for serialization. */
  public static final Translation3dProto proto = new Translation3dProto();

  /** Translation3d struct for serialization. */
  public static final Translation3dStruct struct = new Translation3dStruct();
}
