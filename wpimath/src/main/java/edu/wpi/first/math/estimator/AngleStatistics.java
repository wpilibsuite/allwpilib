// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.estimator;

import edu.wpi.first.math.MathUtil;
import edu.wpi.first.math.Matrix;
import edu.wpi.first.math.Num;
import edu.wpi.first.math.numbers.N1;
import java.util.function.BiFunction;
import org.ejml.simple.SimpleMatrix;

public final class AngleStatistics {
  private AngleStatistics() {
    // Utility class
  }

  /**
   * Subtracts a and b while normalizing the resulting value in the selected row as if it were an
   * angle.
   *
   * @param <S> Number of rows in vector.
   * @param a A vector to subtract from.
   * @param b A vector to subtract with.
   * @param angleStateIdx The row containing angles to be normalized.
   * @return Difference of two vectors with angle at the given index normalized.
   */
  public static <S extends Num> Matrix<S, N1> angleResidual(
      Matrix<S, N1> a, Matrix<S, N1> b, int angleStateIdx) {
    Matrix<S, N1> ret = a.minus(b);
    ret.set(angleStateIdx, 0, MathUtil.angleModulus(ret.get(angleStateIdx, 0)));

    return ret;
  }

  /**
   * Returns a function that subtracts two vectors while normalizing the resulting value in the
   * selected row as if it were an angle.
   *
   * @param <S> Number of rows in vector.
   * @param angleStateIdx The row containing angles to be normalized.
   * @return Function returning difference of two vectors with angle at the given index normalized.
   */
  public static <S extends Num>
      BiFunction<Matrix<S, N1>, Matrix<S, N1>, Matrix<S, N1>> angleResidual(int angleStateIdx) {
    return (a, b) -> angleResidual(a, b, angleStateIdx);
  }

  /**
   * Adds a and b while normalizing the resulting value in the selected row as an angle.
   *
   * @param <S> Number of rows in vector.
   * @param a A vector to add with.
   * @param b A vector to add with.
   * @param angleStateIdx The row containing angles to be normalized.
   * @return Sum of two vectors with angle at the given index normalized.
   */
  public static <S extends Num> Matrix<S, N1> angleAdd(
      Matrix<S, N1> a, Matrix<S, N1> b, int angleStateIdx) {
    Matrix<S, N1> ret = a.plus(b);
    ret.set(angleStateIdx, 0, MathUtil.angleModulus(ret.get(angleStateIdx, 0)));

    return ret;
  }

  /**
   * Returns a function that adds two vectors while normalizing the resulting value in the selected
   * row as an angle.
   *
   * @param <S> Number of rows in vector.
   * @param angleStateIdx The row containing angles to be normalized.
   * @return Function returning of two vectors with angle at the given index normalized.
   */
  public static <S extends Num> BiFunction<Matrix<S, N1>, Matrix<S, N1>, Matrix<S, N1>> angleAdd(
      int angleStateIdx) {
    return (a, b) -> angleAdd(a, b, angleStateIdx);
  }

  /**
   * Computes the mean of sigmas with the weights Wm while computing a special angle mean for a
   * select row.
   *
   * @param <S> Number of rows in sigma point matrix.
   * @param sigmas Sigma points.
   * @param Wm Weights for the mean.
   * @param angleStateIdx The row containing the angles.
   * @return Mean of sigma points.
   */
  public static <S extends Num> Matrix<S, N1> angleMean(
      Matrix<S, ?> sigmas, Matrix<?, N1> Wm, int angleStateIdx) {
    double[] angleSigmas = sigmas.extractRowVector(angleStateIdx).getData();
    Matrix<N1, ?> sinAngleSigmas = new Matrix<>(new SimpleMatrix(1, sigmas.getNumCols()));
    Matrix<N1, ?> cosAngleSigmas = new Matrix<>(new SimpleMatrix(1, sigmas.getNumCols()));
    for (int i = 0; i < angleSigmas.length; i++) {
      sinAngleSigmas.set(0, i, Math.sin(angleSigmas[i]));
      cosAngleSigmas.set(0, i, Math.cos(angleSigmas[i]));
    }

    double sumSin = sinAngleSigmas.times(Matrix.changeBoundsUnchecked(Wm)).elementSum();
    double sumCos = cosAngleSigmas.times(Matrix.changeBoundsUnchecked(Wm)).elementSum();

    Matrix<S, N1> ret = sigmas.times(Matrix.changeBoundsUnchecked(Wm));
    ret.set(angleStateIdx, 0, Math.atan2(sumSin, sumCos));

    return ret;
  }

  /**
   * Returns a function that computes the mean of sigmas with the weights Wm while computing a
   * special angle mean for a select row.
   *
   * @param <S> Number of rows in sigma point matrix.
   * @param angleStateIdx The row containing the angles.
   * @return Function returning mean of sigma points.
   */
  public static <S extends Num> BiFunction<Matrix<S, ?>, Matrix<?, N1>, Matrix<S, N1>> angleMean(
      int angleStateIdx) {
    return (sigmas, Wm) -> angleMean(sigmas, Wm, angleStateIdx);
  }
}
