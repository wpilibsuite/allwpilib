// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.geometry;

import static edu.wpi.first.units.Units.Radians;

import com.fasterxml.jackson.annotation.JsonAutoDetect;
import com.fasterxml.jackson.annotation.JsonCreator;
import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import com.fasterxml.jackson.annotation.JsonProperty;
import edu.wpi.first.math.MatBuilder;
import edu.wpi.first.math.MathSharedStore;
import edu.wpi.first.math.MathUtil;
import edu.wpi.first.math.Matrix;
import edu.wpi.first.math.Nat;
import edu.wpi.first.math.VecBuilder;
import edu.wpi.first.math.Vector;
import edu.wpi.first.math.geometry.proto.Rotation3dProto;
import edu.wpi.first.math.geometry.struct.Rotation3dStruct;
import edu.wpi.first.math.interpolation.Interpolatable;
import edu.wpi.first.math.numbers.N3;
import edu.wpi.first.units.measure.Angle;
import edu.wpi.first.util.protobuf.ProtobufSerializable;
import edu.wpi.first.util.struct.StructSerializable;
import java.util.Objects;

/** A rotation in a 3D coordinate frame represented by a quaternion. */
@JsonIgnoreProperties(ignoreUnknown = true)
@JsonAutoDetect(getterVisibility = JsonAutoDetect.Visibility.NONE)
public class Rotation3d
    implements Interpolatable<Rotation3d>, ProtobufSerializable, StructSerializable {
  /**
   * A preallocated Rotation3d representing no rotation.
   *
   * <p>This exists to avoid allocations for common rotations.
   */
  public static final Rotation3d kZero = new Rotation3d();

  private final Quaternion m_q;

  /** Constructs a Rotation3d representing no rotation. */
  public Rotation3d() {
    m_q = new Quaternion();
  }

  /**
   * Constructs a Rotation3d from a quaternion.
   *
   * @param q The quaternion.
   */
  @JsonCreator
  public Rotation3d(@JsonProperty(required = true, value = "quaternion") Quaternion q) {
    m_q = q.normalize();
  }

  /**
   * Constructs a Rotation3d from extrinsic roll, pitch, and yaw.
   *
   * <p>Extrinsic rotations occur in that order around the axes in the fixed global frame rather
   * than the body frame.
   *
   * <p>Angles are measured counterclockwise with the rotation axis pointing "out of the page". If
   * you point your right thumb along the positive axis direction, your fingers curl in the
   * direction of positive rotation.
   *
   * @param roll The counterclockwise rotation angle around the X axis (roll) in radians.
   * @param pitch The counterclockwise rotation angle around the Y axis (pitch) in radians.
   * @param yaw The counterclockwise rotation angle around the Z axis (yaw) in radians.
   */
  public Rotation3d(double roll, double pitch, double yaw) {
    // https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles#Euler_angles_to_quaternion_conversion
    double cr = Math.cos(roll * 0.5);
    double sr = Math.sin(roll * 0.5);

    double cp = Math.cos(pitch * 0.5);
    double sp = Math.sin(pitch * 0.5);

    double cy = Math.cos(yaw * 0.5);
    double sy = Math.sin(yaw * 0.5);

    m_q =
        new Quaternion(
            cr * cp * cy + sr * sp * sy,
            sr * cp * cy - cr * sp * sy,
            cr * sp * cy + sr * cp * sy,
            cr * cp * sy - sr * sp * cy);
  }

  /**
   * Constructs a Rotation3d from extrinsic roll, pitch, and yaw.
   *
   * <p>Extrinsic rotations occur in that order around the axes in the fixed global frame rather
   * than the body frame.
   *
   * <p>Angles are measured counterclockwise with the rotation axis pointing "out of the page". If
   * you point your right thumb along the positive axis direction, your fingers curl in the
   * direction of positive rotation.
   *
   * @param roll The counterclockwise rotation angle around the X axis (roll).
   * @param pitch The counterclockwise rotation angle around the Y axis (pitch).
   * @param yaw The counterclockwise rotation angle around the Z axis (yaw).
   */
  public Rotation3d(Angle roll, Angle pitch, Angle yaw) {
    this(roll.in(Radians), pitch.in(Radians), yaw.in(Radians));
  }

  /**
   * Constructs a Rotation3d with the given rotation vector representation. This representation is
   * equivalent to axis-angle, where the normalized axis is multiplied by the rotation around the
   * axis in radians.
   *
   * @param rvec The rotation vector.
   */
  public Rotation3d(Vector<N3> rvec) {
    this(rvec, rvec.norm());
  }

  /**
   * Constructs a Rotation3d with the given axis-angle representation. The axis doesn't have to be
   * normalized.
   *
   * @param axis The rotation axis.
   * @param angleRadians The rotation around the axis in radians.
   */
  public Rotation3d(Vector<N3> axis, double angleRadians) {
    double norm = axis.norm();
    if (norm == 0.0) {
      m_q = new Quaternion();
      return;
    }

    // https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles#Definition
    var v = axis.times(1.0 / norm).times(Math.sin(angleRadians / 2.0));
    m_q = new Quaternion(Math.cos(angleRadians / 2.0), v.get(0, 0), v.get(1, 0), v.get(2, 0));
  }

  /**
   * Constructs a Rotation3d with the given axis-angle representation. The axis doesn't have to be
   * normalized.
   *
   * @param axis The rotation axis.
   * @param angle The rotation around the axis.
   */
  public Rotation3d(Vector<N3> axis, Angle angle) {
    this(axis, angle.in(Radians));
  }

  /**
   * Constructs a Rotation3d from a rotation matrix.
   *
   * @param rotationMatrix The rotation matrix.
   * @throws IllegalArgumentException if the rotation matrix isn't special orthogonal.
   */
  public Rotation3d(Matrix<N3, N3> rotationMatrix) {
    final var R = rotationMatrix;

    // Require that the rotation matrix is special orthogonal. This is true if
    // the matrix is orthogonal (RRᵀ = I) and normalized (determinant is 1).
    if (R.times(R.transpose()).minus(Matrix.eye(Nat.N3())).normF() > 1e-9) {
      var msg = "Rotation matrix isn't orthogonal\n\nR =\n" + R.getStorage().toString() + '\n';
      MathSharedStore.reportError(msg, Thread.currentThread().getStackTrace());
      throw new IllegalArgumentException(msg);
    }
    if (Math.abs(R.det() - 1.0) > 1e-9) {
      var msg =
          "Rotation matrix is orthogonal but not special orthogonal\n\nR =\n"
              + R.getStorage().toString()
              + '\n';
      MathSharedStore.reportError(msg, Thread.currentThread().getStackTrace());
      throw new IllegalArgumentException(msg);
    }

    // Turn rotation matrix into a quaternion
    // https://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/
    double trace = R.get(0, 0) + R.get(1, 1) + R.get(2, 2);
    double w;
    double x;
    double y;
    double z;

    if (trace > 0.0) {
      double s = 0.5 / Math.sqrt(trace + 1.0);
      w = 0.25 / s;
      x = (R.get(2, 1) - R.get(1, 2)) * s;
      y = (R.get(0, 2) - R.get(2, 0)) * s;
      z = (R.get(1, 0) - R.get(0, 1)) * s;
    } else {
      if (R.get(0, 0) > R.get(1, 1) && R.get(0, 0) > R.get(2, 2)) {
        double s = 2.0 * Math.sqrt(1.0 + R.get(0, 0) - R.get(1, 1) - R.get(2, 2));
        w = (R.get(2, 1) - R.get(1, 2)) / s;
        x = 0.25 * s;
        y = (R.get(0, 1) + R.get(1, 0)) / s;
        z = (R.get(0, 2) + R.get(2, 0)) / s;
      } else if (R.get(1, 1) > R.get(2, 2)) {
        double s = 2.0 * Math.sqrt(1.0 + R.get(1, 1) - R.get(0, 0) - R.get(2, 2));
        w = (R.get(0, 2) - R.get(2, 0)) / s;
        x = (R.get(0, 1) + R.get(1, 0)) / s;
        y = 0.25 * s;
        z = (R.get(1, 2) + R.get(2, 1)) / s;
      } else {
        double s = 2.0 * Math.sqrt(1.0 + R.get(2, 2) - R.get(0, 0) - R.get(1, 1));
        w = (R.get(1, 0) - R.get(0, 1)) / s;
        x = (R.get(0, 2) + R.get(2, 0)) / s;
        y = (R.get(1, 2) + R.get(2, 1)) / s;
        z = 0.25 * s;
      }
    }

    m_q = new Quaternion(w, x, y, z);
  }

  /**
   * Constructs a Rotation3d that rotates the initial vector onto the final vector.
   *
   * <p>This is useful for turning a 3D vector (final) into an orientation relative to a coordinate
   * system vector (initial).
   *
   * @param initial The initial vector.
   * @param last The final vector.
   */
  public Rotation3d(Vector<N3> initial, Vector<N3> last) {
    double dot = initial.dot(last);
    double normProduct = initial.norm() * last.norm();
    double dotNorm = dot / normProduct;

    if (dotNorm > 1.0 - 1E-9) {
      // If the dot product is 1, the two vectors point in the same direction so
      // there's no rotation. The default initialization of m_q will work.
      m_q = new Quaternion();
    } else if (dotNorm < -1.0 + 1E-9) {
      // If the dot product is -1, the two vectors are antiparallel, so a 180°
      // rotation is required. Any other vector can be used to generate an
      // orthogonal one.

      double x = Math.abs(initial.get(0, 0));
      double y = Math.abs(initial.get(1, 0));
      double z = Math.abs(initial.get(2, 0));

      // Find vector that is most orthogonal to initial vector
      Vector<N3> other;
      if (x < y) {
        if (x < z) {
          // Use x-axis
          other = VecBuilder.fill(1, 0, 0);
        } else {
          // Use z-axis
          other = VecBuilder.fill(0, 0, 1);
        }
      } else {
        if (y < z) {
          // Use y-axis
          other = VecBuilder.fill(0, 1, 0);
        } else {
          // Use z-axis
          other = VecBuilder.fill(0, 0, 1);
        }
      }

      var axis = Vector.cross(initial, other);

      double axisNorm = axis.norm();
      m_q =
          new Quaternion(
              0.0, axis.get(0, 0) / axisNorm, axis.get(1, 0) / axisNorm, axis.get(2, 0) / axisNorm);
    } else {
      var axis = Vector.cross(initial, last);

      // https://stackoverflow.com/a/11741520
      m_q =
          new Quaternion(normProduct + dot, axis.get(0, 0), axis.get(1, 0), axis.get(2, 0))
              .normalize();
    }
  }

  /**
   * Constructs a 3D rotation from a 2D rotation in the X-Y plane.
   *
   * @param rotation The 2D rotation.
   * @see Pose3d#Pose3d(Pose2d)
   * @see Transform3d#Transform3d(Transform2d)
   */
  public Rotation3d(Rotation2d rotation) {
    this(0.0, 0.0, rotation.getRadians());
  }

  /**
   * Adds two rotations together.
   *
   * @param other The rotation to add.
   * @return The sum of the two rotations.
   */
  public Rotation3d plus(Rotation3d other) {
    return rotateBy(other);
  }

  /**
   * Subtracts the new rotation from the current rotation and returns the new rotation.
   *
   * @param other The rotation to subtract.
   * @return The difference between the two rotations.
   */
  public Rotation3d minus(Rotation3d other) {
    return rotateBy(other.unaryMinus());
  }

  /**
   * Takes the inverse of the current rotation.
   *
   * @return The inverse of the current rotation.
   */
  public Rotation3d unaryMinus() {
    return new Rotation3d(m_q.inverse());
  }

  /**
   * Multiplies the current rotation by a scalar.
   *
   * @param scalar The scalar.
   * @return The new scaled Rotation3d.
   */
  public Rotation3d times(double scalar) {
    // https://en.wikipedia.org/wiki/Slerp#Quaternion_Slerp
    if (m_q.getW() >= 0.0) {
      return new Rotation3d(
          VecBuilder.fill(m_q.getX(), m_q.getY(), m_q.getZ()),
          2.0 * scalar * Math.acos(m_q.getW()));
    } else {
      return new Rotation3d(
          VecBuilder.fill(-m_q.getX(), -m_q.getY(), -m_q.getZ()),
          2.0 * scalar * Math.acos(-m_q.getW()));
    }
  }

  /**
   * Divides the current rotation by a scalar.
   *
   * @param scalar The scalar.
   * @return The new scaled Rotation3d.
   */
  public Rotation3d div(double scalar) {
    return times(1.0 / scalar);
  }

  /**
   * Adds the new rotation to the current rotation. The other rotation is applied extrinsically,
   * which means that it rotates around the global axes. For example, {@code new
   * Rotation3d(Units.degreesToRadians(90), 0, 0).rotateBy(new Rotation3d(0,
   * Units.degreesToRadians(45), 0))} rotates by 90 degrees around the +X axis and then by 45
   * degrees around the global +Y axis. (This is equivalent to {@code new
   * Rotation3d(Units.degreesToRadians(90), Units.degreesToRadians(45), 0)})
   *
   * @param other The extrinsic rotation to rotate by.
   * @return The new rotated Rotation3d.
   */
  public Rotation3d rotateBy(Rotation3d other) {
    return new Rotation3d(other.m_q.times(m_q));
  }

  /**
   * Returns the quaternion representation of the Rotation3d.
   *
   * @return The quaternion representation of the Rotation3d.
   */
  @JsonProperty(value = "quaternion")
  public Quaternion getQuaternion() {
    return m_q;
  }

  /**
   * Returns the counterclockwise rotation angle around the X axis (roll) in radians.
   *
   * @return The counterclockwise rotation angle around the X axis (roll) in radians.
   */
  public double getX() {
    final var w = m_q.getW();
    final var x = m_q.getX();
    final var y = m_q.getY();
    final var z = m_q.getZ();

    // wpimath/algorithms.md
    final var cxcy = 1.0 - 2.0 * (x * x + y * y);
    final var sxcy = 2.0 * (w * x + y * z);
    final var cy_sq = cxcy * cxcy + sxcy * sxcy;
    if (cy_sq > 1e-20) {
      return Math.atan2(sxcy, cxcy);
    } else {
      return 0.0;
    }
  }

  /**
   * Returns the counterclockwise rotation angle around the Y axis (pitch) in radians.
   *
   * @return The counterclockwise rotation angle around the Y axis (pitch) in radians.
   */
  public double getY() {
    final var w = m_q.getW();
    final var x = m_q.getX();
    final var y = m_q.getY();
    final var z = m_q.getZ();

    // https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles#Quaternion_to_Euler_angles_(in_3-2-1_sequence)_conversion
    double ratio = 2.0 * (w * y - z * x);
    if (Math.abs(ratio) >= 1.0) {
      return Math.copySign(Math.PI / 2.0, ratio);
    } else {
      return Math.asin(ratio);
    }
  }

  /**
   * Returns the counterclockwise rotation angle around the Z axis (yaw) in radians.
   *
   * @return The counterclockwise rotation angle around the Z axis (yaw) in radians.
   */
  public double getZ() {
    final var w = m_q.getW();
    final var x = m_q.getX();
    final var y = m_q.getY();
    final var z = m_q.getZ();

    // wpimath/algorithms.md
    final var cycz = 1.0 - 2.0 * (y * y + z * z);
    final var cysz = 2.0 * (w * z + x * y);
    final var cy_sq = cycz * cycz + cysz * cysz;
    if (cy_sq > 1e-20) {
      return Math.atan2(cysz, cycz);
    } else {
      return Math.atan2(2.0 * w * z, w * w - z * z);
    }
  }

  /**
   * Returns the counterclockwise rotation angle around the X axis (roll) in a measure.
   *
   * @return The counterclockwise rotation angle around the x axis (roll) in a measure.
   */
  public Angle getMeasureX() {
    return Radians.of(getX());
  }

  /**
   * Returns the counterclockwise rotation angle around the Y axis (pitch) in a measure.
   *
   * @return The counterclockwise rotation angle around the y axis (pitch) in a measure.
   */
  public Angle getMeasureY() {
    return Radians.of(getY());
  }

  /**
   * Returns the counterclockwise rotation angle around the Z axis (yaw) in a measure.
   *
   * @return The counterclockwise rotation angle around the z axis (yaw) in a measure.
   */
  public Angle getMeasureZ() {
    return Radians.of(getZ());
  }

  /**
   * Returns the axis in the axis-angle representation of this rotation.
   *
   * @return The axis in the axis-angle representation.
   */
  public Vector<N3> getAxis() {
    double norm =
        Math.sqrt(m_q.getX() * m_q.getX() + m_q.getY() * m_q.getY() + m_q.getZ() * m_q.getZ());
    if (norm == 0.0) {
      return VecBuilder.fill(0.0, 0.0, 0.0);
    } else {
      return VecBuilder.fill(m_q.getX() / norm, m_q.getY() / norm, m_q.getZ() / norm);
    }
  }

  /**
   * Returns the angle in radians in the axis-angle representation of this rotation.
   *
   * @return The angle in radians in the axis-angle representation of this rotation.
   */
  public double getAngle() {
    double norm =
        Math.sqrt(m_q.getX() * m_q.getX() + m_q.getY() * m_q.getY() + m_q.getZ() * m_q.getZ());
    return 2.0 * Math.atan2(norm, m_q.getW());
  }

  /**
   * Returns rotation matrix representation of this rotation.
   *
   * @return Rotation matrix representation of this rotation.
   */
  public Matrix<N3, N3> toMatrix() {
    double w = m_q.getW();
    double x = m_q.getX();
    double y = m_q.getY();
    double z = m_q.getZ();

    // https://en.wikipedia.org/wiki/Quaternions_and_spatial_rotation#Quaternion-derived_rotation_matrix
    return MatBuilder.fill(
        Nat.N3(),
        Nat.N3(),
        1.0 - 2.0 * (y * y + z * z),
        2.0 * (x * y - w * z),
        2.0 * (x * z + w * y),
        2.0 * (x * y + w * z),
        1.0 - 2.0 * (x * x + z * z),
        2.0 * (y * z - w * x),
        2.0 * (x * z - w * y),
        2.0 * (y * z + w * x),
        1.0 - 2.0 * (x * x + y * y));
  }

  /**
   * Returns rotation vector representation of this rotation.
   *
   * @return Rotation vector representation of this rotation.
   */
  public Vector<N3> toVector() {
    return m_q.toRotationVector();
  }

  /**
   * Returns the angle in a measure in the axis-angle representation of this rotation.
   *
   * @return The angle in a measure in the axis-angle representation of this rotation.
   */
  public Angle getMeasureAngle() {
    return Radians.of(getAngle());
  }

  /**
   * Returns a Rotation2d representing this Rotation3d projected into the X-Y plane.
   *
   * @return A Rotation2d representing this Rotation3d projected into the X-Y plane.
   */
  public Rotation2d toRotation2d() {
    return new Rotation2d(getZ());
  }

  @Override
  public String toString() {
    return String.format("Rotation3d(%s)", m_q);
  }

  /**
   * Checks equality between this Rotation3d and another object.
   *
   * @param obj The other object.
   * @return Whether the two objects are equal or not.
   */
  @Override
  public boolean equals(Object obj) {
    return obj instanceof Rotation3d other
        && Math.abs(Math.abs(m_q.dot(other.m_q)) - m_q.norm() * other.m_q.norm()) < 1e-9;
  }

  @Override
  public int hashCode() {
    return Objects.hash(m_q);
  }

  @Override
  public Rotation3d interpolate(Rotation3d endValue, double t) {
    return plus(endValue.minus(this).times(MathUtil.clamp(t, 0, 1)));
  }

  /** Rotation3d protobuf for serialization. */
  public static final Rotation3dProto proto = new Rotation3dProto();

  /** Rotation3d struct for serialization. */
  public static final Rotation3dStruct struct = new Rotation3dStruct();
}
