// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.geometry;

import edu.wpi.first.math.Matrix;
import edu.wpi.first.math.Nat;

/** A helper class that converts Pose3d objects between different standard coordinate frames. */
public class CoordinateSystem {
  private static final CoordinateSystem m_nwu =
      new CoordinateSystem(CoordinateAxis.N(), CoordinateAxis.W(), CoordinateAxis.U());
  private static final CoordinateSystem m_edn =
      new CoordinateSystem(CoordinateAxis.E(), CoordinateAxis.D(), CoordinateAxis.N());
  private static final CoordinateSystem m_ned =
      new CoordinateSystem(CoordinateAxis.N(), CoordinateAxis.E(), CoordinateAxis.D());

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
    var R = new Matrix<>(Nat.N3(), Nat.N3());
    R.assignBlock(0, 0, positiveX.m_axis);
    R.assignBlock(0, 1, positiveY.m_axis);
    R.assignBlock(0, 2, positiveZ.m_axis);

    // The change of basis matrix should be a pure rotation. The Rotation3d
    // constructor will verify this by checking for special orthogonality.
    m_rotation = new Rotation3d(R);
  }

  /**
   * Returns an instance of the North-West-Up (NWU) coordinate system.
   *
   * <p>The +X axis is north, the +Y axis is west, and the +Z axis is up.
   *
   * @return An instance of the North-West-Up (NWU) coordinate system.
   */
  public static CoordinateSystem NWU() {
    return m_nwu;
  }

  /**
   * Returns an instance of the East-Down-North (EDN) coordinate system.
   *
   * <p>The +X axis is east, the +Y axis is down, and the +Z axis is north.
   *
   * @return An instance of the East-Down-North (EDN) coordinate system.
   */
  public static CoordinateSystem EDN() {
    return m_edn;
  }

  /**
   * Returns an instance of the North-East-Down (NED) coordinate system.
   *
   * <p>The +X axis is north, the +Y axis is east, and the +Z axis is down.
   *
   * @return An instance of the North-East-Down (NED) coordinate system.
   */
  public static CoordinateSystem NED() {
    return m_ned;
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
    return new Pose3d(
        convert(pose.getTranslation(), from, to), convert(pose.getRotation(), from, to));
  }

  /**
   * Converts the given transform from one coordinate system to another.
   *
   * @param transform The transform to convert.
   * @param from The coordinate system the transform starts in.
   * @param to The coordinate system to which to convert.
   * @return The given transform in the desired coordinate system.
   */
  public static Transform3d convert(
      Transform3d transform, CoordinateSystem from, CoordinateSystem to) {
    return new Transform3d(
        convert(transform.getTranslation(), from, to), convert(transform.getRotation(), from, to));
  }
}
