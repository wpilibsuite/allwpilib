// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math;

import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import org.ejml.dense.row.MatrixFeatures_DDRM;
import org.ejml.simple.SimpleMatrix;

public final class MatrixAssertions {
  private MatrixAssertions() {
    // Utility class.
  }

  /**
   * Asserts that two SimpleMatrices are equal.
   *
   * @param expected Expected value.
   * @param actual The value to check against expected.
   */
  public static void assertEquals(SimpleMatrix expected, SimpleMatrix actual) {
    assertFalse(MatrixFeatures_DDRM.hasUncountable(expected.getDDRM()));
    assertTrue(MatrixFeatures_DDRM.isEquals(expected.getDDRM(), actual.getDDRM()));
  }

  /**
   * Asserts that two SimpleMatrices are equal to within a positive delta.
   *
   * @param expected Expected value.
   * @param actual The value to check against expected.
   * @param delta The maximum delta between expected and actual for which both values are still
   *     considered equal.
   */
  public static void assertEquals(SimpleMatrix expected, SimpleMatrix actual, double delta) {
    assertFalse(MatrixFeatures_DDRM.hasUncountable(expected.getDDRM()));
    assertTrue(MatrixFeatures_DDRM.isEquals(expected.getDDRM(), actual.getDDRM(), delta));
  }
}
