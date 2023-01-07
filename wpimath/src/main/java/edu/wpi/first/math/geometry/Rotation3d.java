// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.geometry;

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
import edu.wpi.first.math.interpolation.Interpolatable;
import edu.wpi.first.math.numbers.N3;
import java.util.Objects;
import org.ejml.dense.row.factory.DecompositionFactory_DDRM;

/** A rotation in a 3D coordinate frame represented by a quaternion. */
@JsonIgnoreProperties(ignoreUnknown = true)
@JsonAutoDetect(getterVisibility = JsonAutoDetect.Visibility.NONE)
public class Rotation3d implements Interpolatable<Rotation3d> {
  private Quaternion m_q = new Quaternion();

  /** Constructs a Rotation3d with a default angle of 0 degrees. */
  public Rotation3d() {}

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
   * Constructs a Rotation3d with the given axis-angle representation. The axis doesn't have to be
   * normalized.
   *
   * @param axis The rotation axis.
   * @param angleRadians The rotation around the axis in radians.
   */
  public Rotation3d(Vector<N3> axis, double angleRadians) {
    double norm = axis.norm();
    if (norm == 0.0) {
      return;
    }

    // https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles#Definition
    var v = axis.times(1.0 / norm).times(Math.sin(angleRadians / 2.0));
    m_q = new Quaternion(Math.cos(angleRadians / 2.0), v.get(0, 0), v.get(1, 0), v.get(2, 0));
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
      var builder = new StringBuilder("Rotation matrix isn't orthogonal\n\nR =\n");
      builder.append(R.getStorage().toString()).append('\n');

      var msg = builder.toString();
      MathSharedStore.reportError(msg, Thread.currentThread().getStackTrace());
      throw new IllegalArgumentException(msg);
    }
    if (Math.abs(R.det() - 1.0) > 1e-9) {
      var builder =
          new StringBuilder("Rotation matrix is orthogonal but not special orthogonal\n\nR =\n");
      builder.append(R.getStorage().toString()).append('\n');

      var msg = builder.toString();
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
      return;
    } else if (dotNorm < -1.0 + 1E-9) {
      // If the dot product is -1, the two vectors point in opposite directions
      // so a 180 degree rotation is required. Any orthogonal vector can be used
      // for it. Q in the QR decomposition is an orthonormal basis, so it
      // contains orthogonal unit vectors.
      var X =
          new MatBuilder<>(Nat.N3(), Nat.N1())
              .fill(initial.get(0, 0), initial.get(1, 0), initial.get(2, 0));
      final var qr = DecompositionFactory_DDRM.qr(3, 1);
      qr.decompose(X.getStorage().getMatrix());
      final var Q = qr.getQ(null, false);

      // w = cos(θ/2) = cos(90°) = 0
      //
      // For x, y, and z, we use the second column of Q because the first is
      // parallel instead of orthogonal. The third column would also work.
      m_q = new Quaternion(0.0, Q.get(0, 1), Q.get(1, 1), Q.get(2, 1));
    } else {
      // initial x last
      var axis =
          VecBuilder.fill(
              initial.get(1, 0) * last.get(2, 0) - last.get(1, 0) * initial.get(2, 0),
              last.get(0, 0) * initial.get(2, 0) - initial.get(0, 0) * last.get(2, 0),
              initial.get(0, 0) * last.get(1, 0) - last.get(0, 0) * initial.get(1, 0));

      // https://stackoverflow.com/a/11741520
      m_q =
          new Quaternion(normProduct + dot, axis.get(0, 0), axis.get(1, 0), axis.get(2, 0))
              .normalize();
    }
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
   * Adds the new rotation to the current rotation.
   *
   * @param other The rotation to rotate by.
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

    // https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles#Quaternion_to_Euler_angles_conversion
    return Math.atan2(2.0 * (w * x + y * z), 1.0 - 2.0 * (x * x + y * y));
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

    // https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles#Quaternion_to_Euler_angles_conversion
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

    // https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles#Quaternion_to_Euler_angles_conversion
    return Math.atan2(2.0 * (w * z + x * y), 1.0 - 2.0 * (y * y + z * z));
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
    if (obj instanceof Rotation3d) {
      var other = (Rotation3d) obj;
      return m_q.equals(other.m_q);
    }
    return false;
  }

  @Override
  public int hashCode() {
    return Objects.hash(m_q);
  }

  @Override
  public Rotation3d interpolate(Rotation3d endValue, double t) {
    return plus(endValue.minus(this).times(MathUtil.clamp(t, 0, 1)));
  }
}
