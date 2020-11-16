/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.estimator;


import java.util.function.BiFunction;

import org.ejml.simple.SimpleMatrix;

import edu.wpi.first.wpiutil.math.Matrix;
import edu.wpi.first.wpiutil.math.Num;
import edu.wpi.first.wpiutil.math.numbers.N1;

public final class AngleStatistics {
  private AngleStatistics() {
    // Utility class
  }

  /**
   * Subtracts a and b while normalizing the resulting value in the selected row as if it were an
   * angle.
   *
   * @param a A vector to subtract from.
   * @param b A vector to subtract with.
   * @param angleStateIdx The row containing angles to be normalized.
   */
  @SuppressWarnings("checkstyle:ParameterName")
  public static <S extends Num> Matrix<S, N1> angleResidual(Matrix<S, N1> a, Matrix<S, N1> b,
                                                            int angleStateIdx) {
    Matrix<S, N1> ret = a.minus(b);
    ret.set(angleStateIdx, 0, normalizeAngle(ret.get(angleStateIdx, 0)));

    return ret;
  }

  /**
   * Returns a function that subtracts two vectors while normalizing the resulting value in the
   * selected row as if it were an angle.
   *
   * @param angleStateIdx The row containing angles to be normalized.
   */
  @SuppressWarnings("checkstyle:ParameterName")
  public static <S extends Num> BiFunction<Matrix<S, N1>, Matrix<S, N1>, Matrix<S, N1>>
      angleResidual(int angleStateIdx) {
    return (a, b) -> angleResidual(a, b, angleStateIdx);
  }

  /**
   * Adds a and b while normalizing the resulting value in the selected row as an angle.
   *
   * @param a A vector to add with.
   * @param b A vector to add with.
   * @param angleStateIdx The row containing angles to be normalized.
   */
  @SuppressWarnings("checkstyle:ParameterName")
  public static <S extends Num> Matrix<S, N1> angleAdd(Matrix<S, N1> a, Matrix<S, N1> b,
                                                       int angleStateIdx) {
    Matrix<S, N1> ret = a.plus(b);
    ret.set(angleStateIdx, 0, normalizeAngle(ret.get(angleStateIdx, 0)));

    return ret;
  }

  /**
   * Returns a function that adds two vectors while normalizing the resulting value in the selected
   * row as an angle.
   *
   * @param angleStateIdx The row containing angles to be normalized.
   */
  @SuppressWarnings("checkstyle:ParameterName")
  public static <S extends Num> BiFunction<Matrix<S, N1>, Matrix<S, N1>, Matrix<S, N1>>
      angleAdd(int angleStateIdx) {
    return (a, b) -> angleAdd(a, b, angleStateIdx);
  }

  static double normalizeAngle(double angle) {
    final double tau = 2 * Math.PI;

    angle -= Math.floor(angle / tau) * tau;
    if (angle > Math.PI) {
      angle -= tau;
    }
    return angle;
  }

  /**
   * Computes the mean of sigmas with the weights Wm while computing a special angle mean for a
   * select row.
   *
   * @param sigmas Sigma points.
   * @param Wm Weights for the mean.
   * @param angleStateIdx The row containing the angles.
   */
  @SuppressWarnings("checkstyle:ParameterName")
  public static <S extends Num> Matrix<S, N1> angleMean(Matrix<S, ?> sigmas, Matrix<?, N1> Wm,
                                                        int angleStateIdx) {
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
   * @param angleStateIdx The row containing the angles.
   */
  @SuppressWarnings("checkstyle:ParameterName")
  public static <S extends Num> BiFunction<Matrix<S, ?>, Matrix<?, N1>, Matrix<S, N1>>
      angleMean(int angleStateIdx) {
    return (sigmas, Wm) -> angleMean(sigmas, Wm, angleStateIdx);
  }
}
