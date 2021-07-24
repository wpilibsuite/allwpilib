// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math;

import java.util.Objects;
import org.ejml.simple.SimpleMatrix;

/**
 * A class for constructing arbitrary RxC matrices.
 *
 * @param <R> The number of rows of the desired matrix.
 * @param <C> The number of columns of the desired matrix.
 */
public class MatBuilder<R extends Num, C extends Num> {
  final Nat<R> m_rows;
  final Nat<C> m_cols;

  /**
   * Fills the matrix with the given data, encoded in row major form. (The matrix is filled row by
   * row, left to right with the given data).
   *
   * @param data The data to fill the matrix with.
   * @return The constructed matrix.
   */
  public final Matrix<R, C> fill(double... data) {
    if (Objects.requireNonNull(data).length != this.m_rows.getNum() * this.m_cols.getNum()) {
      throw new IllegalArgumentException(
          "Invalid matrix data provided. Wanted "
              + this.m_rows.getNum()
              + " x "
              + this.m_cols.getNum()
              + " matrix, but got "
              + data.length
              + " elements");
    } else {
      return new Matrix<>(new SimpleMatrix(this.m_rows.getNum(), this.m_cols.getNum(), true, data));
    }
  }

  /**
   * Creates a new {@link MatBuilder} with the given dimensions.
   *
   * @param rows The number of rows of the matrix.
   * @param cols The number of columns of the matrix.
   */
  public MatBuilder(Nat<R> rows, Nat<C> cols) {
    this.m_rows = Objects.requireNonNull(rows);
    this.m_cols = Objects.requireNonNull(cols);
  }
}
