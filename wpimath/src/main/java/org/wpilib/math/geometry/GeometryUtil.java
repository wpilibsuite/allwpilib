// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.geometry;

import org.wpilib.math.linalg.MatBuilder;
import org.wpilib.math.linalg.Matrix;
import org.wpilib.math.linalg.Vector;
import org.wpilib.math.numbers.N3;
import org.wpilib.math.util.Nat;

/** Geometry utilities. */
final class GeometryUtil {
  private GeometryUtil() {
    throw new AssertionError("utility class");
  }

  /**
   * Converts a rotation vector to a rotation matrix.
   *
   * @param rotation The rotation vector.
   * @return The rotation matrix.
   */
  static Matrix<N3, N3> rotationVectorToMatrix(Vector<N3> rotation) {
    return MatBuilder.fill(
        Nat.N3(),
        Nat.N3(),
        0.0,
        -rotation.get(2, 0),
        rotation.get(1, 0),
        rotation.get(2, 0),
        0.0,
        -rotation.get(0, 0),
        -rotation.get(1, 0),
        rotation.get(0, 0),
        0.0);
  }
}
