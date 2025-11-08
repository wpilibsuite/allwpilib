// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math;

import java.util.Objects;
import org.ejml.simple.SimpleMatrix;

/** A class for constructing arbitrary RxC matrices. */
public final class MatBuilder {
  private MatBuilder() {
    throw new UnsupportedOperationException("This is a utility class!");
  }

  /**
   * Fills the matrix with the given data, encoded in row major form. (The matrix is filled row by
   * row, left to right with the given data).
   *
   * @param rows The number of rows of the matrix.
   * @param cols The number of columns of the matrix.
   * @param data The data to fill the matrix with.
   * @param <R> The number of rows of the matrix.
   * @param <C> The number of columns of the matrix.
   * @return The constructed matrix.
   */
  public static <R extends Num, C extends Num> Matrix<R, C> fill(
      Nat<R> rows, Nat<C> cols, double... data) {
    if (Objects.requireNonNull(data).length != rows.getNum() * cols.getNum()) {
      throw new IllegalArgumentException(
          "Invalid matrix data provided. Wanted "
              + rows.getNum()
              + " x "
              + cols.getNum()
              + " matrix, but got "
              + data.length
              + " elements");
    }
    return new Matrix<>(new SimpleMatrix(rows.getNum(), cols.getNum(), true, data));
  }
}
