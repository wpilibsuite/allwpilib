/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpiutil.math;

import java.util.Objects;

import org.ejml.simple.SimpleMatrix;

import edu.wpi.first.wpiutil.math.numbers.N1;

@Deprecated
public final class MatrixUtils {
  private MatrixUtils() {
    throw new AssertionError("utility class");
  }

  /**
   * Creates a new matrix of zeros.
   *
   * @param rows The number of rows in the matrix.
   * @param cols The number of columns in the matrix.
   * @param <R> The number of rows in the matrix as a generic.
   * @param <C> The number of columns in the matrix as a generic.
   * @return An RxC matrix filled with zeros.
   */
  @SuppressWarnings("LineLength")
  public static <R extends Num, C extends Num> Matrix<R, C> zeros(Nat<R> rows, Nat<C> cols) {
    return new Matrix<>(
        new SimpleMatrix(Objects.requireNonNull(rows).getNum(), Objects.requireNonNull(cols).getNum()));
  }

  /**
   * Creates a new vector of zeros.
   *
   * @param nums The size of the desired vector.
   * @param <N> The size of the desired vector as a generic.
   * @return A vector of size N filled with zeros.
   */
  public static <N extends Num> Matrix<N, N1> zeros(Nat<N> nums) {
    return new Matrix<>(new SimpleMatrix(Objects.requireNonNull(nums).getNum(), 1));
  }

  /**
   * Creates the identity matrix of the given dimension.
   *
   * @param dim The dimension of the desired matrix.
   * @param <D> The dimension of the desired matrix as a generic.
   * @return The DxD identity matrix.
   */
  public static <D extends Num> Matrix<D, D> eye(Nat<D> dim) {
    return new Matrix<>(SimpleMatrix.identity(Objects.requireNonNull(dim).getNum()));
  }

  /**
   * Entrypoint to the MatBuilder class for creation
   * of custom matrices with the given dimensions and contents.
   *
   * @param rows The number of rows of the desired matrix.
   * @param cols The number of columns of the desired matrix.
   * @param <R> The number of rows of the desired matrix as a generic.
   * @param <C> The number of columns of the desired matrix as a generic.
   * @return A builder to construct the matrix.
   */
  public static <R extends Num, C extends Num> MatBuilder<R, C> mat(Nat<R> rows, Nat<C> cols) {
    return new MatBuilder<>(rows, cols);
  }

  /**
   * Entrypoint to the VecBuilder class for creation
   * of custom vectors with the given size and contents.
   *
   * @param dim The dimension of the vector.
   * @param <D> The dimension of the vector as a generic.
   * @return A builder to construct the vector.
   */
  public static <D extends Num> VecBuilder<D> vec(Nat<D> dim) {
    return new VecBuilder<>(dim);
  }
}
