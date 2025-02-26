// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.geometry;

import static edu.wpi.first.units.Units.Meters;

import edu.wpi.first.math.MatBuilder;
import edu.wpi.first.math.Matrix;
import edu.wpi.first.math.Nat;
import edu.wpi.first.math.geometry.proto.Transform2dProto;
import edu.wpi.first.math.numbers.N3;
import edu.wpi.first.units.measure.Distance;
import edu.wpi.first.util.protobuf.ProtobufSerializable;
import edu.wpi.first.util.struct.Struct;
import edu.wpi.first.util.struct.StructGenerator;
import edu.wpi.first.util.struct.StructSerializable;

/** Represents a transformation for a Pose2d in the pose's frame. */
public record Transform2d(Translation2d translation, Rotation2d rotation)
    implements ProtobufSerializable, StructSerializable {
  /**
   * A preallocated Transform2d representing no transformation.
   *
   * <p>This exists to avoid allocations for common transformations.
   */
  public static final Transform2d kZero = new Transform2d();

  /**
   * Constructs the transform that maps the initial pose to the final pose.
   *
   * @param initial The initial pose for the transformation.
   * @param last The final pose for the transformation.
   */
  public Transform2d(Pose2d initial, Pose2d last) {
    // We are rotating the difference between the translations
    // using a clockwise rotation matrix. This transforms the global
    // delta into a local delta (relative to the initial pose).
    this(
        last.getTranslation()
            .minus(initial.getTranslation())
            .rotateBy(initial.getRotation().unaryMinus()),
        last.getRotation().minus(initial.getRotation()));
  }

  /**
   * Constructs a transform with x and y translations instead of a separate Translation2d.
   *
   * @param x The x component of the translational component of the transform.
   * @param y The y component of the translational component of the transform.
   * @param rotation The rotational component of the transform.
   */
  public Transform2d(double x, double y, Rotation2d rotation) {
    this(new Translation2d(x, y), rotation);
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
    this(
        new Translation2d(matrix.get(0, 2), matrix.get(1, 2)),
        new Rotation2d(matrix.block(2, 2, 0, 0)));
    if (matrix.get(2, 0) != 0.0 || matrix.get(2, 1) != 0.0 || matrix.get(2, 2) != 1.0) {
      throw new IllegalArgumentException("Affine transformation matrix is invalid");
    }
  }

  /** Constructs the identity transform -- maps an initial pose to itself. */
  public Transform2d() {
    this(Translation2d.kZero, Rotation2d.kZero);
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
    return new Transform2d(Pose2d.kZero, Pose2d.kZero.transformBy(this).transformBy(other));
  }

  /**
   * Returns the X component of the transformation's translation.
   *
   * @return The x component of the transformation's translation.
   */
  public double getX() {
    return translation().x();
  }

  /**
   * Returns the Y component of the transformation's translation.
   *
   * @return The y component of the transformation's translation.
   */
  public double getY() {
    return translation().y();
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
   * Invert the transformation. This is useful for undoing a transformation.
   *
   * @return The inverted transformation.
   */
  public Transform2d inverse() {
    // We are rotating the difference between the translations
    // using a clockwise rotation matrix. This transforms the global
    // delta into a local delta (relative to the initial pose).
    return new Transform2d(
        translation().unaryMinus().rotateBy(rotation().unaryMinus()), rotation().unaryMinus());
  }

  @Override
  public String toString() {
    return String.format("Transform2d(%s, %s)", translation, rotation);
  }

  /** Transform2d protobuf for serialization. */
  public static final Transform2dProto proto = new Transform2dProto();

  /** Transform2d struct for serialization. */
  public static final Struct<Transform2d> struct = StructGenerator.genRecord(Transform2d.class);
}
