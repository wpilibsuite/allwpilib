/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpiutil.math;

import java.util.Objects;

import org.ejml.dense.row.CommonOps_DDRM;
import org.ejml.dense.row.NormOps_DDRM;
import org.ejml.simple.SimpleMatrix;

/**
 * A shape-safe wrapper over Efficient Java Matrix Library (EJML) matrices.
 *
 * <p>This class is intended to be used alongside the state space library.
 *
 * @param <R> The number of rows in this matrix.
 * @param <C> The number of columns in this matrix.
 */
@SuppressWarnings("PMD.TooManyMethods")
public class Matrix<R extends Num, C extends Num> {

  private final SimpleMatrix m_storage;

  /**
   * Gets the number of columns in this matrix.
   *
   * @return The number of columns, according to the internal storage.
   */
  public final int getNumCols() {
    return this.m_storage.numCols();
  }

  /**
   * Gets the number of rows in this matrix.
   *
   * @return The number of rows, according to the internal storage.
   */
  public final int getNumRows() {
    return this.m_storage.numRows();
  }

  /**
   * Get an element of this matrix.
   *
   * @param row The row of the element.
   * @param col The column of the element.
   * @return The element in this matrix at row,col.
   */
  public final double get(int row, int col) {
    return this.m_storage.get(row, col);
  }

  /**
   * Sets the value at the given indices.
   *
   * @param row     The row of the element.
   * @param col     The column of the element.
   * @param value The value to insert at the given location.
   */
  public final void set(int row, int col, double value) {
    this.m_storage.set(row, col, value);
  }

  /**
   * If a vector then a square matrix is returned
   * if a matrix then a vector of diagonal elements is returned.
   *
   * @return Diagonal elements inside a vector or a square matrix with the same diagonal elements.
   */
  public final Matrix<R, C> diag() {
    return new Matrix<>(this.m_storage.diag());
  }

  /**
   * Returns the largest element of this matrix.
   *
   * @return The largest element of this matrix.
   */
  public final double maxInternal() {
    return CommonOps_DDRM.elementMax(this.m_storage.getDDRM());
  }

  /**
   * Returns the smallest element of this matrix.
   *
   * @return The smallest element of this matrix.
   */
  public final double minInternal() {
    return CommonOps_DDRM.elementMin(this.m_storage.getDDRM());
  }

  /**
   * Calculates the mean of the elements in this matrix.
   *
   * @return The mean value of this matrix.
   */
  public final double mean() {
    return this.elementSum() / (double) this.m_storage.getNumElements();
  }

  /**
   * Multiplies this matrix with another that has C rows.
   *
   * <p>As matrix multiplication is only defined if the number of columns
   * in the first matrix matches the number of rows in the second,
   * this operation will fail to compile under any other circumstances.
   *
   * @param other The other matrix to multiply by.
   * @param <C2>  The number of columns in the second matrix.
   * @return The result of the matrix multiplication between this and the given matrix.
   */
  public final <C2 extends Num> Matrix<R, C2> times(Matrix<C, C2> other) {
    return new Matrix<>(this.m_storage.mult(other.m_storage));
  }

  /**
   * Multiplies all the elements of this matrix by the given scalar.
   *
   * @param value The scalar value to multiply by.
   * @return A new matrix with all the elements multiplied by the given value.
   */
  public final Matrix<R, C> times(double value) {
    return new Matrix<>(this.m_storage.scale(value));
  }

  /**
   * <p>
   * Returns a matrix which is the result of an element by element multiplication of 'this' and 'b'.
   * c<sub>i,j</sub> = a<sub>i,j</sub>*b<sub>i,j</sub>
   * </p>
   *
   * @param other A matrix.
   * @return The element by element multiplication of 'this' and 'b'.
   */
  public final Matrix<R, C> elementTimes(Matrix<R, C> other) {
    return new Matrix<>(this.m_storage.elementMult(Objects.requireNonNull(other).m_storage));
  }

  /**
   * Subtracts the given value from all the elements of this matrix.
   *
   * @param value The value to subtract.
   * @return The resultant matrix.
   */
  public final Matrix<R, C> minus(double value) {
    return new Matrix<>(this.m_storage.minus(value));
  }


  /**
   * Subtracts the given matrix from this matrix.
   *
   * @param value The matrix to subtract.
   * @return The resultant matrix.
   */
  public final Matrix<R, C> minus(Matrix<R, C> value) {
    return new Matrix<>(this.m_storage.minus(Objects.requireNonNull(value).m_storage));
  }


  /**
   * Adds the given value to all the elements of this matrix.
   *
   * @param value The value to add.
   * @return The resultant matrix.
   */
  public final Matrix<R, C> plus(double value) {
    return new Matrix<>(this.m_storage.plus(value));
  }

  /**
   * Adds the given matrix to this matrix.
   *
   * @param value The matrix to add.
   * @return The resultant matrix.
   */
  public final Matrix<R, C> plus(Matrix<R, C> value) {
    return new Matrix<>(this.m_storage.plus(value.m_storage));
  }

  /**
   * Divides all elements of this matrix by the given value.
   *
   * @param value The value to divide by.
   * @return The resultant matrix.
   */
  public final Matrix<R, C> div(int value) {
    return new Matrix<>(this.m_storage.divide((double) value));
  }

  /**
   * Divides all elements of this matrix by the given value.
   *
   * @param value The value to divide by.
   * @return The resultant matrix.
   */
  public final Matrix<R, C> div(double value) {
    return new Matrix<>(this.m_storage.divide(value));
  }

  /**
   * Calculates the transpose, M^T of this matrix.
   *
   * @return The tranpose matrix.
   */
  public final Matrix<C, R> transpose() {
    return new Matrix<>(this.m_storage.transpose());
  }


  /**
   * Returns a copy of this matrix.
   *
   * @return A copy of this matrix.
   */
  public final Matrix<R, C> copy() {
    return new Matrix<>(this.m_storage.copy());
  }


  /**
   * Returns the inverse matrix of this matrix.
   *
   * @return The inverse of this matrix.
   * @throws org.ejml.data.SingularMatrixException If this matrix is non-invertable.
   */
  public final Matrix<R, C> inv() {
    return new Matrix<>(this.m_storage.invert());
  }

  /**
   * Returns the determinant of this matrix.
   *
   * @return The determinant of this matrix.
   */
  public final double det() {
    return this.m_storage.determinant();
  }

  /**
   * Computes the Frobenius normal of the matrix.<br>
   * <br>
   * normF = Sqrt{  &sum;<sub>i=1:m</sub> &sum;<sub>j=1:n</sub> { a<sub>ij</sub><sup>2</sup>}   }
   *
   * @return The matrix's Frobenius normal.
   */
  public final double normF() {
    return this.m_storage.normF();
  }

  /**
   * Computes the induced p = 1 matrix norm.<br>
   * <br>
   * ||A||<sub>1</sub>= max(j=1 to n; sum(i=1 to m; |a<sub>ij</sub>|))
   *
   * @return The norm.
   */
  public final double normIndP1() {
    return NormOps_DDRM.inducedP1(this.m_storage.getDDRM());
  }

  /**
   * Computes the sum of all the elements in the matrix.
   *
   * @return Sum of all the elements.
   */
  public final double elementSum() {
    return this.m_storage.elementSum();
  }

  /**
   * Computes the trace of the matrix.
   *
   * @return The trace of the matrix.
   */
  public final double trace() {
    return this.m_storage.trace();
  }

  /**
   * Returns a matrix which is the result of an element by element power of 'this' and 'b':
   * c<sub>i,j</sub> = a<sub>i,j</sub> ^ b.
   *
   * @param b Scalar
   * @return The element by element power of 'this' and 'b'.
   */
  @SuppressWarnings("ParameterName")
  public final Matrix<R, C> epow(double b) {
    return new Matrix<>(this.m_storage.elementPower(b));
  }

  /**
   * Returns a matrix which is the result of an element by element power of 'this' and 'b':
   * c<sub>i,j</sub> = a<sub>i,j</sub> ^ b.
   *
   * @param b Scalar.
   * @return The element by element power of 'this' and 'b'.
   */
  @SuppressWarnings("ParameterName")
  public final Matrix<R, C> epow(int b) {
    return new Matrix<>(this.m_storage.elementPower((double) b));
  }

  /**
   * Returns the EJML {@link SimpleMatrix} backing this wrapper.
   *
   * @return The untyped EJML {@link SimpleMatrix}.
   */
  public final SimpleMatrix getStorage() {
    return this.m_storage;
  }

  /**
   * Constructs a new matrix with the given storage.
   * Caller should make sure that the provided generic bounds match the shape of the provided matrix
   *
   * @param storage The {@link SimpleMatrix} to back this value
   */
  public Matrix(SimpleMatrix storage) {
    this.m_storage = Objects.requireNonNull(storage);
  }
}
