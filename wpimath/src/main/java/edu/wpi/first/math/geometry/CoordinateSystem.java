// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.geometry;

import edu.wpi.first.math.Matrix;
import edu.wpi.first.math.Nat;

/** A helper class that converts Pose3d objects between different standard coordinate frames. */
public class CoordinateSystem {
  // Rotation from this coordinate system to NWU coordinate system
  private final Rotation3d m_rotation;

  /**
   * Constructs a coordinate system with the given cardinal directions for each axis.
   *
   * @param positiveX The cardinal direction of the positive x-axis.
   * @param positiveY The cardinal direction of the positive y-axis.
   * @param positiveZ The cardinal direction of the positive z-axis.
   * @throws IllegalArgumentException if the coordinate system isn't special orthogonal
   */
  public CoordinateSystem(
      CoordinateAxis positiveX, CoordinateAxis positiveY, CoordinateAxis positiveZ) {
    // Construct a change of basis matrix from the source coordinate system to the
    // NWU coordinate system. Each column vector in the change of basis matrix is
    // one of the old basis vectors mapped to its representation in the new basis.
    @SuppressWarnings("LocalVariableName")
    var R = new Matrix<>(Nat.N3(), Nat.N3());
    R.assignBlock(0, 0, positiveX.m_axis);
    R.assignBlock(0, 1, positiveY.m_axis);
    R.assignBlock(0, 2, positiveZ.m_axis);

    // Require that the change of basis matrix is special orthogonal. This is true
    // if the axes used are orthogonal and normalized. The CoordinateAxis class
    // already normalizes itself, so we just need to check for orthogonality.
    if (!R.times(R.transpose()).equals(Matrix.eye(Nat.N3()))) {
      throw new IllegalArgumentException("Coordinate system isn't special orthogonal");
    }

    // Turn change of basis matrix into a quaternion since it's a pure rotation
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

    m_rotation = new Rotation3d(new Quaternion(w, x, y, z));
  }

  /**
   * Returns an instance of the NWU coordinate system.
   *
   * @return An instance of the NWU coordinate system.
   */
  @SuppressWarnings("MethodName")
  public static CoordinateSystem NWU() {
    return new CoordinateSystem(CoordinateAxis.N(), CoordinateAxis.W(), CoordinateAxis.U());
  }

  /**
   * Returns an instance of the EDN coordinate system.
   *
   * @return An instance of the EDN coordinate system.
   */
  @SuppressWarnings("MethodName")
  public static CoordinateSystem EDN() {
    return new CoordinateSystem(CoordinateAxis.E(), CoordinateAxis.D(), CoordinateAxis.N());
  }

  /**
   * Returns an instance of the NED coordinate system.
   *
   * @return An instance of the NED coordinate system.
   */
  @SuppressWarnings("MethodName")
  public static CoordinateSystem NED() {
    return new CoordinateSystem(CoordinateAxis.N(), CoordinateAxis.E(), CoordinateAxis.D());
  }

  /**
   * Converts the given translation from one coordinate system to another.
   *
   * @param translation The translation to convert.
   * @param from The coordinate system the pose starts in.
   * @param to The coordinate system to which to convert.
   * @return The given translation in the desired coordinate system.
   */
  public static Translation3d convert(
      Translation3d translation, CoordinateSystem from, CoordinateSystem to) {
    return translation.rotateBy(from.m_rotation.minus(to.m_rotation));
  }

  /**
   * Converts the given rotation from one coordinate system to another.
   *
   * @param rotation The rotation to convert.
   * @param from The coordinate system the rotation starts in.
   * @param to The coordinate system to which to convert.
   * @return The given rotation in the desired coordinate system.
   */
  public static Rotation3d convert(
      Rotation3d rotation, CoordinateSystem from, CoordinateSystem to) {
    return rotation.rotateBy(from.m_rotation.minus(to.m_rotation));
  }

  /**
   * Converts the given pose from one coordinate system to another.
   *
   * @param pose The pose to convert.
   * @param from The coordinate system the pose starts in.
   * @param to The coordinate system to which to convert.
   * @return The given pose in the desired coordinate system.
   */
  public static Pose3d convert(Pose3d pose, CoordinateSystem from, CoordinateSystem to) {
    return pose.relativeTo(new Pose3d(new Translation3d(), to.m_rotation.minus(from.m_rotation)));
  }
}
