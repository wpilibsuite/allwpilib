// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.random;

import java.util.Random;
import org.ejml.simple.SimpleMatrix;
import org.wpilib.math.linalg.Matrix;
import org.wpilib.math.numbers.N1;
import org.wpilib.math.util.Num;

/** Utilities for generating normally distributed random values. */
public final class Normal {
  private static Random rand = new Random();

  private Normal() {
    throw new UnsupportedOperationException("This is a utility class!");
  }

  /**
   * Creates a vector of normally distributed random values with the given standard deviations for
   * each element.
   *
   * @param <N> Num representing the dimensionality of the noise vector to create.
   * @param stdDevs A matrix whose elements are the standard deviations of each element of the
   *     random vector.
   * @return Vector of normally distributed values.
   */
  public static <N extends Num> Matrix<N, N1> normal(Matrix<N, N1> stdDevs) {
    Matrix<N, N1> result = new Matrix<>(new SimpleMatrix(stdDevs.getNumRows(), 1));
    for (int i = 0; i < stdDevs.getNumRows(); i++) {
      result.set(i, 0, rand.nextGaussian() * stdDevs.get(i, 0));
    }
    return result;
  }
}
