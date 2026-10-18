// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.geometry;

import static org.wpilib.units.Units.Meters;

import java.util.Objects;
import org.wpilib.math.geometry.proto.Transform3dProto;
import org.wpilib.math.geometry.struct.Transform3dStruct;
import org.wpilib.math.jni.Transform3dJNI;
import org.wpilib.math.linalg.MatBuilder;
import org.wpilib.math.linalg.Matrix;
import org.wpilib.math.numbers.N4;
import org.wpilib.math.util.Nat;
import org.wpilib.units.measure.Distance;
import org.wpilib.util.protobuf.ProtobufSerializable;
import org.wpilib.util.struct.StructSerializable;

/**
 * Represents a transformation for a Pose3d in the pose's frame. Translation is applied before
 * rotation. (The translation is applied in the pose's original frame, not the transformed frame.)
 *
 * <p>Transforms are applied intrinsically, i.e. relative to the pose's own frame rather than the
 * global frame. This is in contrast to the rotation classes, which apply rotations extrinsically.
 */
public final class Transform3d implements ProtobufSerializable, StructSerializable {
  /**
   * A preallocated Transform3d representing no transformation.
   *
   * <p>This exists to avoid allocations for common transformations.
   */
  public static final Transform3d ZERO = new Transform3d();

  /** The translational component of the transform. */
  public final Translation3d translation;

  /** The rotational component of the transform. */
  public final Rotation3d rotation;

  /**
   * Constructs the transform that maps the initial pose to the final pose.
   *
   * @param initial The initial pose for the transformation.
   * @param last The final pose for the transformation.
   */
  public Transform3d(Pose3d initial, Pose3d last) {
    // To transform the global translation delta to be relative to the initial
    // pose, rotate by the inverse of the initial pose's orientation.
    translation = last.translation.minus(initial.translation).rotateBy(initial.rotation.inverse());

    rotation = last.rotation.relativeTo(initial.rotation);
  }

  /**
   * Constructs a transform with the given translation and rotation components.
   *
   * @param translation Translational component of the transform.
   * @param rotation Rotational component of the transform.
   */
  public Transform3d(Translation3d translation, Rotation3d rotation) {
    this.translation = translation;
    this.rotation = rotation;
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
    translation = new Translation3d(x, y, z);
    this.rotation = rotation;
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
    translation = new Translation3d(matrix.get(0, 3), matrix.get(1, 3), matrix.get(2, 3));
    rotation = new Rotation3d(matrix.block(3, 3, 0, 0));
    if (matrix.get(3, 0) != 0.0
        || matrix.get(3, 1) != 0.0
        || matrix.get(3, 2) != 0.0
        || matrix.get(3, 3) != 1.0) {
      throw new IllegalArgumentException("Affine transformation matrix is invalid");
    }
  }

  /** Constructs the identity transform -- maps an initial pose to itself. */
  public Transform3d() {
    translation = Translation3d.ZERO;
    rotation = Rotation3d.ZERO;
  }

  /**
   * Constructs a 3D transform from a 2D transform in the X-Y plane.
   *
   * @param transform The 2D transform.
   * @see Rotation3d#Rotation3d(Rotation2d)
   * @see Translation3d#Translation3d(Translation2d)
   */
  public Transform3d(Transform2d transform) {
    translation = new Translation3d(transform.translation);
    rotation = new Rotation3d(transform.rotation);
  }

  /**
   * Multiplies the transform by the scalar.
   *
   * @param scalar The scalar.
   * @return The scaled Transform3d.
   */
  public Transform3d times(double scalar) {
    return new Transform3d(translation.times(scalar), rotation.times(scalar));
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
    return new Transform3d(Pose3d.ZERO, Pose3d.ZERO.transformBy(this).transformBy(other));
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
   * Returns the Z component of the transformation's translation.
   *
   * @return The z component of the transformation's translation.
   */
  public double getZ() {
    return translation.z;
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
   * Returns the Z component of the transformation's translation in a measure.
   *
   * @return The z component of the transformation's translation in a measure.
   */
  public Distance getMeasureZ() {
    return translation.getMeasureZ();
  }

  /**
   * Returns an affine transformation matrix representation of this transformation.
   *
   * @return An affine transformation matrix representation of this transformation.
   */
  public Matrix<N4, N4> toMatrix() {
    var vec = translation.toVector();
    var mat = rotation.toMatrix();
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
   * Returns a Twist3d of the current transform (pose delta). If b is the output of {@code a.log()},
   * then {@code b.exp()} would yield a.
   *
   * @return The twist that maps the current transform.
   */
  public Twist3d log() {
    var thisQuaternion = rotation.quaternion;
    double[] resultArray =
        Transform3dJNI.log(
            this.getX(),
            this.getY(),
            this.getZ(),
            thisQuaternion.w,
            thisQuaternion.x,
            thisQuaternion.y,
            thisQuaternion.z);
    return new Twist3d(
        resultArray[0],
        resultArray[1],
        resultArray[2],
        resultArray[3],
        resultArray[4],
        resultArray[5]);
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
        translation.unaryMinus().rotateBy(rotation.inverse()), rotation.inverse());
  }

  @Override
  public String toString() {
    return String.format("Transform3d(%s, %s)", translation, rotation);
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
        && other.translation.equals(translation)
        && other.rotation.equals(rotation);
  }

  @Override
  public int hashCode() {
    return Objects.hash(translation, rotation);
  }

  /** Transform3d protobuf for serialization. */
  public static final Transform3dProto proto = new Transform3dProto();

  /** Transform3d struct for serialization. */
  public static final Transform3dStruct struct = new Transform3dStruct();
}
