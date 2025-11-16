// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.geometry;

import static org.wpilib.units.Units.Meters;

import com.fasterxml.jackson.annotation.JsonAutoDetect;
import com.fasterxml.jackson.annotation.JsonCreator;
import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import com.fasterxml.jackson.annotation.JsonProperty;
import java.util.Collection;
import java.util.Collections;
import java.util.Comparator;
import java.util.Objects;
import org.wpilib.math.geometry.proto.Pose2dProto;
import org.wpilib.math.geometry.struct.Pose2dStruct;
import org.wpilib.math.interpolation.Interpolatable;
import org.wpilib.math.linalg.MatBuilder;
import org.wpilib.math.linalg.Matrix;
import org.wpilib.math.numbers.N3;
import org.wpilib.math.util.Nat;
import org.wpilib.units.measure.Distance;
import org.wpilib.util.protobuf.ProtobufSerializable;
import org.wpilib.util.struct.StructSerializable;

/** Represents a 2D pose containing translational and rotational elements. */
@JsonIgnoreProperties(ignoreUnknown = true)
@JsonAutoDetect(getterVisibility = JsonAutoDetect.Visibility.NONE)
public class Pose2d implements Interpolatable<Pose2d>, ProtobufSerializable, StructSerializable {
  /**
   * A preallocated Pose2d representing the origin.
   *
   * <p>This exists to avoid allocations for common poses.
   */
  public static final Pose2d kZero = new Pose2d();

  private final Translation2d m_translation;
  private final Rotation2d m_rotation;

  /** Constructs a pose at the origin facing toward the positive X axis. */
  public Pose2d() {
    m_translation = Translation2d.kZero;
    m_rotation = Rotation2d.kZero;
  }

  /**
   * Constructs a pose with the specified translation and rotation.
   *
   * @param translation The translational component of the pose.
   * @param rotation The rotational component of the pose.
   */
  @JsonCreator
  public Pose2d(
      @JsonProperty(required = true, value = "translation") Translation2d translation,
      @JsonProperty(required = true, value = "rotation") Rotation2d rotation) {
    m_translation = translation;
    m_rotation = rotation;
  }

  /**
   * Constructs a pose with x and y translations instead of a separate Translation2d.
   *
   * @param x The x component of the translational component of the pose.
   * @param y The y component of the translational component of the pose.
   * @param rotation The rotational component of the pose.
   */
  public Pose2d(double x, double y, Rotation2d rotation) {
    m_translation = new Translation2d(x, y);
    m_rotation = rotation;
  }

  /**
   * Constructs a pose with x and y translations instead of a separate Translation2d. The X and Y
   * translations will be converted to and tracked as meters.
   *
   * @param x The x component of the translational component of the pose.
   * @param y The y component of the translational component of the pose.
   * @param rotation The rotational component of the pose.
   */
  public Pose2d(Distance x, Distance y, Rotation2d rotation) {
    this(x.in(Meters), y.in(Meters), rotation);
  }

  /**
   * Constructs a pose with the specified affine transformation matrix.
   *
   * @param matrix The affine transformation matrix.
   * @throws IllegalArgumentException if the affine transformation matrix is invalid.
   */
  public Pose2d(Matrix<N3, N3> matrix) {
    m_translation = new Translation2d(matrix.get(0, 2), matrix.get(1, 2));
    m_rotation = new Rotation2d(matrix.block(2, 2, 0, 0));
    if (matrix.get(2, 0) != 0.0 || matrix.get(2, 1) != 0.0 || matrix.get(2, 2) != 1.0) {
      throw new IllegalArgumentException("Affine transformation matrix is invalid");
    }
  }

  /**
   * Transforms the pose by the given transformation and returns the new transformed pose.
   *
   * <pre>
   * [x_new]    [cos, -sin, 0][transform.x]
   * [y_new] += [sin,  cos, 0][transform.y]
   * [t_new]    [  0,    0, 1][transform.t]
   * </pre>
   *
   * @param other The transform to transform the pose by.
   * @return The transformed pose.
   */
  public Pose2d plus(Transform2d other) {
    return transformBy(other);
  }

  /**
   * Returns the Transform2d that maps the one pose to another.
   *
   * @param other The initial pose of the transformation.
   * @return The transform that maps the other pose to the current pose.
   */
  public Transform2d minus(Pose2d other) {
    final var pose = this.relativeTo(other);
    return new Transform2d(pose.getTranslation(), pose.getRotation());
  }

  /**
   * Returns the translation component of the transformation.
   *
   * @return The translational component of the pose.
   */
  @JsonProperty
  public Translation2d getTranslation() {
    return m_translation;
  }

  /**
   * Returns the X component of the pose's translation.
   *
   * @return The x component of the pose's translation.
   */
  public double getX() {
    return m_translation.getX();
  }

  /**
   * Returns the Y component of the pose's translation.
   *
   * @return The y component of the pose's translation.
   */
  public double getY() {
    return m_translation.getY();
  }

  /**
   * Returns the X component of the pose's translation in a measure.
   *
   * @return The x component of the pose's translation in a measure.
   */
  public Distance getMeasureX() {
    return m_translation.getMeasureX();
  }

  /**
   * Returns the Y component of the pose's translation in a measure.
   *
   * @return The y component of the pose's translation in a measure.
   */
  public Distance getMeasureY() {
    return m_translation.getMeasureY();
  }

  /**
   * Returns the rotational component of the transformation.
   *
   * @return The rotational component of the pose.
   */
  @JsonProperty
  public Rotation2d getRotation() {
    return m_rotation;
  }

  /**
   * Multiplies the current pose by a scalar.
   *
   * @param scalar The scalar.
   * @return The new scaled Pose2d.
   */
  public Pose2d times(double scalar) {
    return new Pose2d(m_translation.times(scalar), m_rotation.times(scalar));
  }

  /**
   * Divides the current pose by a scalar.
   *
   * @param scalar The scalar.
   * @return The new scaled Pose2d.
   */
  public Pose2d div(double scalar) {
    return times(1.0 / scalar);
  }

  /**
   * Rotates the pose around the origin and returns the new pose.
   *
   * @param other The rotation to transform the pose by.
   * @return The transformed pose.
   */
  public Pose2d rotateBy(Rotation2d other) {
    return new Pose2d(m_translation.rotateBy(other), m_rotation.rotateBy(other));
  }

  /**
   * Transforms the pose by the given transformation and returns the new pose. See + operator for
   * the matrix multiplication performed.
   *
   * @param other The transform to transform the pose by.
   * @return The transformed pose.
   */
  public Pose2d transformBy(Transform2d other) {
    return new Pose2d(
        m_translation.plus(other.getTranslation().rotateBy(m_rotation)),
        other.getRotation().plus(m_rotation));
  }

  /**
   * Returns the current pose relative to the given pose.
   *
   * <p>This function can often be used for trajectory tracking or pose stabilization algorithms to
   * get the error between the reference and the current pose.
   *
   * @param other The pose that is the origin of the new coordinate frame that the current pose will
   *     be converted into.
   * @return The current pose relative to the new origin pose.
   */
  public Pose2d relativeTo(Pose2d other) {
    var transform = new Transform2d(other, this);
    return new Pose2d(transform.getTranslation(), transform.getRotation());
  }

  /**
   * Rotates the current pose around a point in 2D space.
   *
   * @param point The point in 2D space to rotate around.
   * @param rot The rotation to rotate the pose by.
   * @return The new rotated pose.
   */
  public Pose2d rotateAround(Translation2d point, Rotation2d rot) {
    return new Pose2d(m_translation.rotateAround(point, rot), m_rotation.rotateBy(rot));
  }

  /**
   * Returns an affine transformation matrix representation of this pose.
   *
   * @return An affine transformation matrix representation of this pose.
   */
  public Matrix<N3, N3> toMatrix() {
    var vec = m_translation.toVector();
    var mat = m_rotation.toMatrix();
    return MatBuilder.fill(
        Nat.N3(),
        Nat.N3(),
        mat.get(0, 0),
        mat.get(0, 1),
        vec.get(0),
        mat.get(1, 0),
        mat.get(1, 1),
        vec.get(1),
        0.0,
        0.0,
        1.0);
  }

  /**
   * Returns the nearest Pose2d from a collection of poses. If two or more poses in the collection
   * have the same distance from this pose, return the one with the closest rotation component.
   *
   * @param poses The collection of poses to find the nearest.
   * @return The nearest Pose2d from the collection.
   */
  public Pose2d nearest(Collection<Pose2d> poses) {
    return Collections.min(
        poses,
        Comparator.comparing(
                (Pose2d other) -> this.getTranslation().getDistance(other.getTranslation()))
            .thenComparing(
                (Pose2d other) ->
                    Math.abs(this.getRotation().minus(other.getRotation()).getRadians())));
  }

  @Override
  public String toString() {
    return String.format("Pose2d(%s, %s)", m_translation, m_rotation);
  }

  /**
   * Checks equality between this Pose2d and another object.
   *
   * @param obj The other object.
   * @return Whether the two objects are equal or not.
   */
  @Override
  public boolean equals(Object obj) {
    return obj instanceof Pose2d pose
        && m_translation.equals(pose.m_translation)
        && m_rotation.equals(pose.m_rotation);
  }

  @Override
  public int hashCode() {
    return Objects.hash(m_translation, m_rotation);
  }

  @Override
  public Pose2d interpolate(Pose2d endValue, double t) {
    if (t < 0) {
      return this;
    } else if (t >= 1) {
      return endValue;
    } else {
      var twist = endValue.minus(this).log();
      var scaledTwist = new Twist2d(twist.dx * t, twist.dy * t, twist.dtheta * t);
      return this.plus(scaledTwist.exp());
    }
  }

  /** Pose2d protobuf for serialization. */
  public static final Pose2dProto proto = new Pose2dProto();

  /** Pose2d struct for serialization. */
  public static final Pose2dStruct struct = new Pose2dStruct();
}
