// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.geometry;

import static org.wpilib.units.Units.Meters;

import java.util.Objects;
import org.wpilib.math.geometry.proto.Transform2dProto;
import org.wpilib.math.geometry.struct.Transform2dStruct;
import org.wpilib.math.linalg.MatBuilder;
import org.wpilib.math.linalg.Matrix;
import org.wpilib.math.numbers.N3;
import org.wpilib.math.util.Nat;
import org.wpilib.units.measure.Distance;
import org.wpilib.util.protobuf.ProtobufSerializable;
import org.wpilib.util.struct.StructSerializable;

/**
 * Represents a transformation for a Pose2d in the pose's frame.
 *
 * <p>Transforms are applied intrinsically, i.e. relative to the pose's own frame rather than the
 * global frame. This is in contrast to the rotation classes, which apply rotations extrinsically.
 */
public final class Transform2d implements ProtobufSerializable, StructSerializable {
  /**
   * A preallocated Transform2d representing no transformation.
   *
   * <p>This exists to avoid allocations for common transformations.
   */
  public static final Transform2d ZERO = new Transform2d();

  /** The translational component of the transform. */
  public final Translation2d translation;

  /** The rotational component of the transform. */
  public final Rotation2d rotation;

  /**
   * Constructs the transform that maps the initial pose to the final pose.
   *
   * @param initial The initial pose for the transformation.
   * @param last The final pose for the transformation.
   */
  public Transform2d(Pose2d initial, Pose2d last) {
    // To transform the global translation delta to be relative to the initial
    // pose, rotate by the inverse of the initial pose's orientation.
    translation =
        last.translation.minus(initial.translation).rotateBy(initial.rotation.unaryMinus());

    rotation = last.rotation.relativeTo(initial.rotation);
  }

  /**
   * Constructs a transform with the given translation and rotation components.
   *
   * @param translation Translational component of the transform.
   * @param rotation Rotational component of the transform.
   */
  public Transform2d(Translation2d translation, Rotation2d rotation) {
    this.translation = translation;
    this.rotation = rotation;
  }

  /**
   * Constructs a transform with x and y translations instead of a separate Translation2d.
   *
   * @param x The x component of the translational component of the transform.
   * @param y The y component of the translational component of the transform.
   * @param rotation The rotational component of the transform.
   */
  public Transform2d(double x, double y, Rotation2d rotation) {
    translation = new Translation2d(x, y);
    this.rotation = rotation;
  }

  /**
   * Constructs a transform with x and y translations instead of a separate Translation2d. The X and
   * Y translations will be converted to and tracked as meters.
   *
   * @param x The x component of the translational component of the transform.
   * @param y The y component of the translational component of the transform.
   * @param rotation The rotational component of the transform.
   */
  public Transform2d(Distance x, Distance y, Rotation2d rotation) {
    this(x.in(Meters), y.in(Meters), rotation);
  }

  /**
   * Constructs a transform with the specified affine transformation matrix.
   *
   * @param matrix The affine transformation matrix.
   * @throws IllegalArgumentException if the affine transformation matrix is invalid.
   */
  public Transform2d(Matrix<N3, N3> matrix) {
    translation = new Translation2d(matrix.get(0, 2), matrix.get(1, 2));
    rotation = new Rotation2d(matrix.block(2, 2, 0, 0));
    if (matrix.get(2, 0) != 0.0 || matrix.get(2, 1) != 0.0 || matrix.get(2, 2) != 1.0) {
      throw new IllegalArgumentException("Affine transformation matrix is invalid");
    }
  }

  /** Constructs the identity transform -- maps an initial pose to itself. */
  public Transform2d() {
    translation = Translation2d.ZERO;
    rotation = Rotation2d.ZERO;
  }

  /**
   * Multiplies the transform by the scalar.
   *
   * @param scalar The scalar.
   * @return The scaled Transform2d.
   */
  public Transform2d times(double scalar) {
    return new Transform2d(translation.times(scalar), rotation.times(scalar));
  }

  /**
   * Divides the transform by the scalar.
   *
   * @param scalar The scalar.
   * @return The scaled Transform2d.
   */
  public Transform2d div(double scalar) {
    return times(1.0 / scalar);
  }

  /**
   * Composes two transformations. The second transform is applied relative to the orientation of
   * the first.
   *
   * @param other The transform to compose with this one.
   * @return The composition of the two transformations.
   */
  public Transform2d plus(Transform2d other) {
    return new Transform2d(Pose2d.ZERO, Pose2d.ZERO.transformBy(this).transformBy(other));
  }

  /**
   * Returns the X component of the transformation's translation.
   *
   * @return The x component of the transformation's translation.
   */
  public double getX() {
    return translation.x;
  }

  /**
   * Returns the Y component of the transformation's translation.
   *
   * @return The y component of the transformation's translation.
   */
  public double getY() {
    return translation.y;
  }

  /**
   * Returns the X component of the transformation's translation in a measure.
   *
   * @return The x component of the transformation's translation in a measure.
   */
  public Distance getMeasureX() {
    return translation.getMeasureX();
  }

  /**
   * Returns the Y component of the transformation's translation in a measure.
   *
   * @return The y component of the transformation's translation in a measure.
   */
  public Distance getMeasureY() {
    return translation.getMeasureY();
  }

  /**
   * Returns an affine transformation matrix representation of this transformation.
   *
   * @return An affine transformation matrix representation of this transformation.
   */
  public Matrix<N3, N3> toMatrix() {
    var vec = translation.toVector();
    var mat = rotation.toMatrix();
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
   * Returns a Twist2d of the current transform (pose delta). If b is the output of {@code a.log()},
   * then {@code b.exp()} would yield a.
   *
   * @return The twist that maps the current transform.
   */
  public Twist2d log() {
    final double dtheta = rotation.getRadians();
    final double halfDtheta = dtheta / 2.0;

    final double cosMinusOne = rotation.cos - 1;

    double halfThetaByTanOfHalfDtheta;
    if (Math.abs(cosMinusOne) < 1E-9) {
      halfThetaByTanOfHalfDtheta = 1.0 - 1.0 / 12.0 * dtheta * dtheta;
    } else {
      halfThetaByTanOfHalfDtheta = -(halfDtheta * rotation.sin) / cosMinusOne;
    }

    Translation2d translationPart =
        translation
            .rotateBy(new Rotation2d(halfThetaByTanOfHalfDtheta, -halfDtheta))
            .times(Math.hypot(halfThetaByTanOfHalfDtheta, halfDtheta));

    return new Twist2d(translationPart.x, translationPart.y, dtheta);
  }

  /**
   * Invert the transformation. This is useful for undoing a transformation.
   *
   * @return The inverted transformation.
   */
  public Transform2d inverse() {
    // We are rotating the difference between the translations
    // using a clockwise rotation matrix. This transforms the global
    // delta into a local delta (relative to the initial pose).
    return new Transform2d(
        translation.unaryMinus().rotateBy(rotation.unaryMinus()), rotation.unaryMinus());
  }

  @Override
  public String toString() {
    return String.format("Transform2d(%s, %s)", translation, rotation);
  }

  /**
   * Checks equality between this Transform2d and another object.
   *
   * @param obj The other object.
   * @return Whether the two objects are equal or not.
   */
  @Override
  public boolean equals(Object obj) {
    return obj instanceof Transform2d other
        && other.translation.equals(translation)
        && other.rotation.equals(rotation);
  }

  @Override
  public int hashCode() {
    return Objects.hash(translation, rotation);
  }

  /** Transform2d protobuf for serialization. */
  public static final Transform2dProto proto = new Transform2dProto();

  /** Transform2d struct for serialization. */
  public static final Transform2dStruct struct = new Transform2dStruct();
}
