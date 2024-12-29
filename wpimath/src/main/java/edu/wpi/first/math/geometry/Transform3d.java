// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.geometry;

import static edu.wpi.first.units.Units.Meters;

import edu.wpi.first.math.MatBuilder;
import edu.wpi.first.math.Matrix;
import edu.wpi.first.math.Nat;
import edu.wpi.first.math.geometry.proto.Transform3dProto;
import edu.wpi.first.math.geometry.struct.Transform3dStruct;
import edu.wpi.first.math.numbers.N4;
import edu.wpi.first.units.measure.Distance;
import edu.wpi.first.util.protobuf.ProtobufSerializable;
import edu.wpi.first.util.struct.StructSerializable;
import java.util.Objects;

/** Represents a transformation for a Pose3d in the pose's frame. */
public class Transform3d implements ProtobufSerializable, StructSerializable {
  /**
   * A preallocated Transform3d representing no transformation.
   *
   * <p>This exists to avoid allocations for common transformations.
   */
  public static final Transform3d kZero = new Transform3d();

  private final Translation3d m_translation;
  private final Rotation3d m_rotation;

  /**
   * Constructs the transform that maps the initial pose to the final pose.
   *
   * @param initial The initial pose for the transformation.
   * @param last The final pose for the transformation.
   */
  public Transform3d(Pose3d initial, Pose3d last) {
    // We are rotating the difference between the translations
    // using a clockwise rotation matrix. This transforms the global
    // delta into a local delta (relative to the initial pose).
    m_translation =
        last.getTranslation()
            .minus(initial.getTranslation())
            .rotateBy(initial.getRotation().unaryMinus());

    m_rotation = last.getRotation().minus(initial.getRotation());
  }

  /**
   * Constructs a transform with the given translation and rotation components.
   *
   * @param translation Translational component of the transform.
   * @param rotation Rotational component of the transform.
   */
  public Transform3d(Translation3d translation, Rotation3d rotation) {
    m_translation = translation;
    m_rotation = rotation;
  }

  /**
   * Constructs a transform with x, y, and z translations instead of a separate Translation3d.
   *
   * @param x The x component of the translational component of the transform.
   * @param y The y component of the translational component of the transform.
   * @param z The z component of the translational component of the transform.
   * @param rotation The rotational component of the transform.
   */
  public Transform3d(double x, double y, double z, Rotation3d rotation) {
    m_translation = new Translation3d(x, y, z);
    m_rotation = rotation;
  }

  /**
   * Constructs a transform with x, y, and z translations instead of a separate Translation3d. The
   * X, Y, and Z translations will be converted to and tracked as meters.
   *
   * @param x The x component of the translational component of the transform.
   * @param y The y component of the translational component of the transform.
   * @param z The z component of the translational component of the transform.
   * @param rotation The rotational component of the transform.
   */
  public Transform3d(Distance x, Distance y, Distance z, Rotation3d rotation) {
    this(x.in(Meters), y.in(Meters), z.in(Meters), rotation);
  }

  /**
   * Constructs a transform with the specified affine transformation matrix.
   *
   * @param matrix The affine transformation matrix.
   * @throws IllegalArgumentException if the affine transformation matrix is invalid.
   */
  public Transform3d(Matrix<N4, N4> matrix) {
    m_translation = new Translation3d(matrix.get(0, 3), matrix.get(1, 3), matrix.get(2, 3));
    m_rotation = new Rotation3d(matrix.block(3, 3, 0, 0));
    if (matrix.get(3, 0) != 0.0
        || matrix.get(3, 1) != 0.0
        || matrix.get(3, 2) != 0.0
        || matrix.get(3, 3) != 1.0) {
      throw new IllegalArgumentException("Affine transformation matrix is invalid");
    }
  }

  /** Constructs the identity transform -- maps an initial pose to itself. */
  public Transform3d() {
    m_translation = Translation3d.kZero;
    m_rotation = Rotation3d.kZero;
  }

  /**
   * Constructs a 3D transform from a 2D transform in the X-Y plane.
   *
   * @param transform The 2D transform.
   * @see Rotation3d#Rotation3d(Rotation2d)
   * @see Translation3d#Translation3d(Translation2d)
   */
  public Transform3d(Transform2d transform) {
    m_translation = new Translation3d(transform.getTranslation());
    m_rotation = new Rotation3d(transform.getRotation());
  }

  /**
   * Multiplies the transform by the scalar.
   *
   * @param scalar The scalar.
   * @return The scaled Transform3d.
   */
  public Transform3d times(double scalar) {
    return new Transform3d(m_translation.times(scalar), m_rotation.times(scalar));
  }

  /**
   * Divides the transform by the scalar.
   *
   * @param scalar The scalar.
   * @return The scaled Transform3d.
   */
  public Transform3d div(double scalar) {
    return times(1.0 / scalar);
  }

  /**
   * Composes two transformations. The second transform is applied relative to the orientation of
   * the first.
   *
   * @param other The transform to compose with this one.
   * @return The composition of the two transformations.
   */
  public Transform3d plus(Transform3d other) {
    return new Transform3d(Pose3d.kZero, Pose3d.kZero.transformBy(this).transformBy(other));
  }

  /**
   * Returns the translation component of the transformation.
   *
   * @return The translational component of the transform.
   */
  public Translation3d getTranslation() {
    return m_translation;
  }

  /**
   * Returns the X component of the transformation's translation.
   *
   * @return The x component of the transformation's translation.
   */
  public double getX() {
    return m_translation.getX();
  }

  /**
   * Returns the Y component of the transformation's translation.
   *
   * @return The y component of the transformation's translation.
   */
  public double getY() {
    return m_translation.getY();
  }

  /**
   * Returns the Z component of the transformation's translation.
   *
   * @return The z component of the transformation's translation.
   */
  public double getZ() {
    return m_translation.getZ();
  }

  /**
   * Returns the X component of the transformation's translation in a measure.
   *
   * @return The x component of the transformation's translation in a measure.
   */
  public Distance getMeasureX() {
    return m_translation.getMeasureX();
  }

  /**
   * Returns the Y component of the transformation's translation in a measure.
   *
   * @return The y component of the transformation's translation in a measure.
   */
  public Distance getMeasureY() {
    return m_translation.getMeasureY();
  }

  /**
   * Returns the Z component of the transformation's translation in a measure.
   *
   * @return The z component of the transformation's translation in a measure.
   */
  public Distance getMeasureZ() {
    return m_translation.getMeasureZ();
  }

  /**
   * Returns an affine transformation matrix representation of this transformation.
   *
   * @return An affine transformation matrix representation of this transformation.
   */
  public Matrix<N4, N4> toMatrix() {
    var vec = m_translation.toVector();
    var mat = m_rotation.toMatrix();
    return MatBuilder.fill(
        Nat.N4(),
        Nat.N4(),
        mat.get(0, 0),
        mat.get(0, 1),
        mat.get(0, 2),
        vec.get(0),
        mat.get(1, 0),
        mat.get(1, 1),
        mat.get(1, 2),
        vec.get(1),
        mat.get(2, 0),
        mat.get(2, 1),
        mat.get(2, 2),
        vec.get(2),
        0.0,
        0.0,
        0.0,
        1.0);
  }

  /**
   * Returns the rotational component of the transformation.
   *
   * @return Reference to the rotational component of the transform.
   */
  public Rotation3d getRotation() {
    return m_rotation;
  }

  /**
   * Invert the transformation. This is useful for undoing a transformation.
   *
   * @return The inverted transformation.
   */
  public Transform3d inverse() {
    // We are rotating the difference between the translations
    // using a clockwise rotation matrix. This transforms the global
    // delta into a local delta (relative to the initial pose).
    return new Transform3d(
        getTranslation().unaryMinus().rotateBy(getRotation().unaryMinus()),
        getRotation().unaryMinus());
  }

  @Override
  public String toString() {
    return String.format("Transform3d(%s, %s)", m_translation, m_rotation);
  }

  /**
   * Checks equality between this Transform3d and another object.
   *
   * @param obj The other object.
   * @return Whether the two objects are equal or not.
   */
  @Override
  public boolean equals(Object obj) {
    return obj instanceof Transform3d other
        && other.m_translation.equals(m_translation)
        && other.m_rotation.equals(m_rotation);
  }

  @Override
  public int hashCode() {
    return Objects.hash(m_translation, m_rotation);
  }

  /** Transform3d protobuf for serialization. */
  public static final Transform3dProto proto = new Transform3dProto();

  /** Transform3d struct for serialization. */
  public static final Transform3dStruct struct = new Transform3dStruct();
}
