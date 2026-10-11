// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.linalg;

import java.util.Objects;
import org.ejml.MatrixDimensionException;
import org.ejml.data.DMatrixRMaj;
import org.ejml.dense.row.CommonOps_DDRM;
import org.ejml.dense.row.MatrixFeatures_DDRM;
import org.ejml.dense.row.NormOps_DDRM;
import org.ejml.dense.row.factory.DecompositionFactory_DDRM;
import org.ejml.dense.row.misc.TransposeAlgs_DDRM;
import org.ejml.interfaces.decomposition.CholeskyDecomposition_F64;
import org.ejml.simple.SimpleMatrix;
import org.wpilib.math.jni.EigenJNI;
import org.wpilib.math.linalg.proto.MatrixProto;
import org.wpilib.math.linalg.struct.MatrixStruct;
import org.wpilib.math.numbers.N1;
import org.wpilib.math.util.Nat;
import org.wpilib.math.util.Num;
import org.wpilib.util.protobuf.Protobuf;
import org.wpilib.util.protobuf.ProtobufSerializable;
import org.wpilib.util.struct.Struct;
import org.wpilib.util.struct.StructSerializable;

/**
 * A shape-safe wrapper over Efficient Java Matrix Library (EJML) matrices.
 *
 * <p>This class is intended to be used alongside the state space library.
 *
 * @param <R> The number of rows in this matrix.
 * @param <C> The number of columns in this matrix.
 */
public class Matrix<R extends Num, C extends Num>
    implements ProtobufSerializable, StructSerializable {
  /** Storage for underlying EJML matrix. */
  protected final SimpleMatrix m_storage;

  /**
   * Constructs an empty zero matrix of the given dimensions.
   *
   * @param rows The number of rows of the matrix.
   * @param columns The number of columns of the matrix.
   */
  public Matrix(Nat<R> rows, Nat<C> columns) {
    this.m_storage =
        new SimpleMatrix(
            Objects.requireNonNull(rows).getNum(), Objects.requireNonNull(columns).getNum());
  }

  /**
   * Constructs a new {@link Matrix} with the given storage. Caller should make sure that the
   * provided generic bounds match the shape of the provided {@link Matrix}.
   *
   * @param rows The number of rows of the matrix.
   * @param columns The number of columns of the matrix.
   * @param storage The double array to back this value.
   */
  public Matrix(Nat<R> rows, Nat<C> columns, double[] storage) {
    this.m_storage = new SimpleMatrix(rows.getNum(), columns.getNum(), true, storage);
  }

  /**
   * Constructs a new {@link Matrix} with the given storage. Caller should make sure that the
   * provided generic bounds match the shape of the provided {@link Matrix}.
   *
   * <p>NOTE:It is not recommend to use this constructor unless the {@link SimpleMatrix} API is
   * absolutely necessary due to the desired function not being accessible through the {@link
   * Matrix} wrapper.
   *
   * @param storage The {@link SimpleMatrix} to back this value.
   */
  public Matrix(SimpleMatrix storage) {
    this.m_storage = Objects.requireNonNull(storage);
  }

  /**
   * Constructs a new matrix with the storage of the supplied matrix.
   *
   * @param other The {@link Matrix} to copy the storage of.
   */
  public Matrix(Matrix<R, C> other) {
    this.m_storage = Objects.requireNonNull(other).getStorage().copy();
  }

  /**
   * Gets the underlying {@link SimpleMatrix} that this {@link Matrix} wraps.
   *
   * <p>NOTE:The use of this method is heavily discouraged as this removes any guarantee of type
   * safety. This should only be called if the {@link SimpleMatrix} API is absolutely necessary due
   * to the desired function not being accessible through the {@link Matrix} wrapper.
   *
   * @return The underlying {@link SimpleMatrix} storage.
   */
  public SimpleMatrix getStorage() {
    return m_storage;
  }

  /**
   * Gets the number of columns in this matrix.
   *
   * @return The number of columns, according to the internal storage.
   */
  public final int getNumCols() {
    return this.m_storage.getNumCols();
  }

  /**
   * Gets the number of rows in this matrix.
   *
   * @return The number of rows, according to the internal storage.
   */
  public final int getNumRows() {
    return this.m_storage.getNumRows();
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
   * @param row The row of the element.
   * @param col The column of the element.
   * @param value The value to insert at the given location.
   */
  public final void set(int row, int col, double value) {
    this.m_storage.set(row, col, value);
  }

  /**
   * Sets a row to a given row vector.
   *
   * @param row The row to set.
   * @param val The row vector to set the given row to.
   */
  public final void setRow(int row, Matrix<N1, C> val) {
    this.m_storage.setRow(row, 0, Objects.requireNonNull(val).m_storage.getDDRM().getData());
  }

  /**
   * Sets a column to a given column vector.
   *
   * @param column The column to set.
   * @param val The column vector to set the given row to.
   */
  public final void setColumn(int column, Matrix<R, N1> val) {
    this.m_storage.setColumn(column, 0, Objects.requireNonNull(val).m_storage.getDDRM().getData());
  }

  /**
   * Sets all the elements in "this" matrix equal to the specified value.
   *
   * @param value The value each element is set to.
   */
  public void fill(double value) {
    this.m_storage.fill(value);
  }

  /**
   * Returns the largest element of this matrix.
   *
   * @return The largest element of this matrix.
   */
  public final double max() {
    return CommonOps_DDRM.elementMax(this.m_storage.getDDRM());
  }

  /**
   * Returns the absolute value of the element in this matrix with the largest absolute value.
   *
   * @return The absolute value of the element with the largest absolute value.
   */
  public final double maxAbs() {
    return CommonOps_DDRM.elementMaxAbs(this.m_storage.getDDRM());
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
    return this.elementSum() / this.m_storage.getNumElements();
  }

  /**
   * Multiplies this matrix with another that has C rows.
   *
   * <p>As matrix multiplication is only defined if the number of columns in the first matrix
   * matches the number of rows in the second, this operation will fail to compile under any other
   * circumstances.
   *
   * @param other The other matrix to multiply by.
   * @param <C2> The number of columns in the second matrix.
   * @return The result of the matrix multiplication between "this" and the given matrix.
   */
  public final <C2 extends Num> Matrix<R, C2> times(Matrix<C, C2> other) {
    return new Matrix<>(this.m_storage.mult(Objects.requireNonNull(other).m_storage));
  }

  /**
   * Multiplies all the elements of this matrix by the given scalar.
   *
   * @param value The scalar value to multiply by.
   * @return A new matrix with all the elements multiplied by the given value.
   */
  public Matrix<R, C> times(double value) {
    return new Matrix<>(this.m_storage.scale(value));
  }

  /**
   * Returns a matrix which is the result of an element by element multiplication of "this" and
   * other.
   *
   * <p>c<sub>i,j</sub> = a<sub>i,j</sub>*other<sub>i,j</sub>
   *
   * @param other The other {@link Matrix} to perform element multiplication on.
   * @return The element by element multiplication of "this" and other.
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
    return new Matrix<>(this.m_storage.plus(Objects.requireNonNull(value).m_storage));
  }

  /**
   * Divides all elements of this matrix by the given value.
   *
   * @param value The value to divide by.
   * @return The resultant matrix.
   */
  public Matrix<R, C> div(int value) {
    return new Matrix<>(this.m_storage.divide(value));
  }

  /**
   * Divides all elements of this matrix by the given value.
   *
   * @param value The value to divide by.
   * @return The resultant matrix.
   */
  public Matrix<R, C> div(double value) {
    return new Matrix<>(this.m_storage.divide(value));
  }

  /**
   * Calculates the transpose, Mᵀ of this matrix.
   *
   * @return The transpose matrix.
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
   * Returns the inverse matrix of "this" matrix.
   *
   * @return The inverse of "this" matrix.
   * @throws org.ejml.data.SingularMatrixException If "this" matrix is non-invertable.
   */
  public final Matrix<R, C> inv() {
    return new Matrix<>(this.m_storage.invert());
  }

  /**
   * Returns the Moore-Penrose pseudoinverse of this matrix.
   *
   * @return The pseudoinverse of this matrix.
   */
  public final Matrix<C, R> pseudoInverse() {
    return new Matrix<>(this.m_storage.pseudoInverse());
  }

  /**
   * Returns the solution x to the equation Ax = b, where A is "this" matrix.
   *
   * <p>The matrix equation could also be written as x = A<sup>-1</sup>b. Where the pseudo inverse
   * is used if A is not square.
   *
   * @param <C2> Columns in b.
   * @param b The right-hand side of the equation to solve.
   * @return The solution to the linear system.
   */
  public final <C2 extends Num> Matrix<C, C2> solve(Matrix<R, C2> b) {
    return new Matrix<>(this.m_storage.solve(Objects.requireNonNull(b).m_storage));
  }

  /**
   * Computes the matrix exponential using Eigen's solver. This method only works for square
   * matrices, and will otherwise throw an {@link MatrixDimensionException}.
   *
   * @return The exponential of A.
   */
  public final Matrix<R, C> exp() {
    if (this.getNumRows() != this.getNumCols()) {
      throw new MatrixDimensionException(
          "Non-square matrices cannot be exponentiated! "
              + "This matrix is "
              + this.getNumRows()
              + " x "
              + this.getNumCols());
    }
    Matrix<R, C> toReturn = new Matrix<>(new SimpleMatrix(this.getNumRows(), this.getNumCols()));
    EigenJNI.exp(
        this.m_storage.getDDRM().getData(),
        this.getNumRows(),
        toReturn.m_storage.getDDRM().getData());
    return toReturn;
  }

  /**
   * Computes the matrix power using Eigen's solver. This method only works for square matrices, and
   * will otherwise throw an {@link MatrixDimensionException}.
   *
   * @param exponent The exponent.
   * @return The power of A.
   */
  public final Matrix<R, C> pow(double exponent) {
    if (this.getNumRows() != this.getNumCols()) {
      throw new MatrixDimensionException(
          "Non-square matrices cannot be raised to a power! "
              + "This matrix is "
              + this.getNumRows()
              + " x "
              + this.getNumCols());
    }
    Matrix<R, C> toReturn = new Matrix<>(new SimpleMatrix(this.getNumRows(), this.getNumCols()));
    EigenJNI.pow(
        this.m_storage.getDDRM().getData(),
        this.getNumRows(),
        exponent,
        toReturn.m_storage.getDDRM().getData());
    return toReturn;
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
   * Computes the Frobenius normal of the matrix.
   *
   * <p>normF = Sqrt{ &sum;<sub>i=1:m</sub> &sum;<sub>j=1:n</sub> { a<sub>ij</sub><sup>2</sup>} }
   *
   * @return The matrix's Frobenius normal.
   */
  public final double normF() {
    return this.m_storage.normF();
  }

  /**
   * Computes the induced p = 1 matrix norm.
   *
   * <p>||A||<sub>1</sub>= max(j=1 to n; sum(i=1 to m; |a<sub>ij</sub>|))
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
   * Returns a matrix which is the result of an element by element power of "this" and b.
   *
   * <p>c<sub>i,j</sub> = a<sub>i,j</sub> ^ b
   *
   * @param b Scalar.
   * @return The element by element power of "this" and b.
   */
  public final Matrix<R, C> elementPower(double b) {
    return new Matrix<>(this.m_storage.elementPower(b));
  }

  /**
   * Returns a matrix which is the result of an element by element power of "this" and b.
   *
   * <p>c<sub>i,j</sub> = a<sub>i,j</sub> ^ b
   *
   * @param b Scalar.
   * @return The element by element power of "this" and b.
   */
  public final Matrix<R, C> elementPower(int b) {
    return new Matrix<>(this.m_storage.elementPower(b));
  }

  /**
   * Extracts a given row into a row vector with new underlying storage.
   *
   * @param row The row to extract a vector from.
   * @return A row vector from the given row.
   */
  public final Matrix<N1, C> extractRowVector(int row) {
    return new Matrix<>(this.m_storage.extractVector(true, row));
  }

  /**
   * Extracts a given column into a column vector with new underlying storage.
   *
   * @param column The column to extract a vector from.
   * @return A column vector from the given column.
   */
  public final Matrix<R, N1> extractColumnVector(int column) {
    return new Matrix<>(this.m_storage.extractVector(false, column));
  }

  /**
   * Extracts a matrix of a given size and start position with new underlying storage.
   *
   * @param <R2> Number of rows to extract.
   * @param <C2> Number of columns to extract.
   * @param height The number of rows of the extracted matrix.
   * @param width The number of columns of the extracted matrix.
   * @param startingRow The starting row of the extracted matrix.
   * @param startingCol The starting column of the extracted matrix.
   * @return The extracted matrix.
   */
  public final <R2 extends Num, C2 extends Num> Matrix<R2, C2> block(
      Nat<R2> height, Nat<C2> width, int startingRow, int startingCol) {
    return new Matrix<>(
        this.m_storage.extractMatrix(
            startingRow,
            startingRow + Objects.requireNonNull(height).getNum(),
            startingCol,
            startingCol + Objects.requireNonNull(width).getNum()));
  }

  /**
   * Extracts a matrix of a given size and start position with new underlying storage.
   *
   * @param <R2> Number of rows to extract.
   * @param <C2> Number of columns to extract.
   * @param height The number of rows of the extracted matrix.
   * @param width The number of columns of the extracted matrix.
   * @param startingRow The starting row of the extracted matrix.
   * @param startingCol The starting column of the extracted matrix.
   * @return The extracted matrix.
   */
  public final <R2 extends Num, C2 extends Num> Matrix<R2, C2> block(
      int height, int width, int startingRow, int startingCol) {
    return new Matrix<>(
        this.m_storage.extractMatrix(
            startingRow, startingRow + height, startingCol, startingCol + width));
  }

  /**
   * Assign a matrix of a given size and start position.
   *
   * @param <R2> Rows in block assignment.
   * @param <C2> Columns in block assignment.
   * @param startingRow The row to start at.
   * @param startingCol The column to start at.
   * @param other The matrix to assign the block to.
   */
  public <R2 extends Num, C2 extends Num> void assignBlock(
      int startingRow, int startingCol, Matrix<R2, C2> other) {
    this.m_storage.insertIntoThis(
        startingRow, startingCol, Objects.requireNonNull(other).m_storage);
  }

  /**
   * Extracts a submatrix from the supplied matrix and inserts it in a submatrix in "this". The
   * shape of "this" is used to determine the size of the matrix extracted.
   *
   * @param <R2> Number of rows to extract.
   * @param <C2> Number of columns to extract.
   * @param startingRow The starting row in the supplied matrix to extract the submatrix.
   * @param startingCol The starting column in the supplied matrix to extract the submatrix.
   * @param other The matrix to extract the submatrix from.
   */
  public <R2 extends Num, C2 extends Num> void extractFrom(
      int startingRow, int startingCol, Matrix<R2, C2> other) {
    CommonOps_DDRM.extract(
        other.m_storage.getDDRM(), startingRow, startingCol, this.m_storage.getDDRM());
  }

  /**
   * Decompose "this" matrix using Cholesky Decomposition. If the "this" matrix is zeros, it will
   * return the zero matrix.
   *
   * @param lowerTriangular Whether we want to decompose to the lower triangular Cholesky matrix.
   * @return The decomposed matrix.
   * @throws RuntimeException if the matrix could not be decomposed(i.e. is not positive
   *     semidefinite).
   */
  public Matrix<R, C> lltDecompose(boolean lowerTriangular) {
    SimpleMatrix temp = m_storage.copy();

    CholeskyDecomposition_F64<DMatrixRMaj> chol =
        DecompositionFactory_DDRM.chol(temp.getNumRows(), lowerTriangular);
    if (!chol.decompose(temp.getMatrix())) {
      // check that the input is not all zeros -- if they are, we special case and return all
      // zeros.
      var matData = temp.getDDRM().data;
      var isZeros = true;
      for (double matDatum : matData) {
        isZeros &= Math.abs(matDatum) < 1e-6;
      }
      if (isZeros) {
        return new Matrix<>(new SimpleMatrix(temp.getNumRows(), temp.getNumCols()));
      }

      throw new RuntimeException("Cholesky decomposition failed! Input matrix:\n" + m_storage);
    }

    return new Matrix<>(SimpleMatrix.wrap(chol.getT(null)));
  }

  /**
   * Returns the row major data of this matrix as a double array.
   *
   * @return The row major data of this matrix as a double array.
   */
  public double[] getData() {
    return m_storage.getDDRM().getData();
  }

  /**
   * Creates the identity matrix of the given dimension.
   *
   * @param dim The dimension of the desired matrix as a {@link Nat}.
   * @param <D> The dimension of the desired matrix as a generic.
   * @return The DxD identity matrix.
   */
  public static <D extends Num> Matrix<D, D> eye(Nat<D> dim) {
    return new Matrix<>(SimpleMatrix.identity(Objects.requireNonNull(dim).getNum()));
  }

  /**
   * Creates the identity matrix of the given dimension.
   *
   * @param dim The dimension of the desired matrix as a {@link Num}.
   * @param <D> The dimension of the desired matrix as a generic.
   * @return The DxD identity matrix.
   */
  public static <D extends Num> Matrix<D, D> eye(D dim) {
    return new Matrix<>(SimpleMatrix.identity(Objects.requireNonNull(dim).getNum()));
  }

  /**
   * Reassigns dimensions of a {@link Matrix} to allow for operations with other matrices that have
   * wildcard dimensions.
   *
   * @param <R1> Row dimension to assign.
   * @param <C1> Column dimension to assign.
   * @param mat The {@link Matrix} to remove the dimensions from.
   * @return The matrix with reassigned dimensions.
   */
  public static <R1 extends Num, C1 extends Num> Matrix<R1, C1> changeBoundsUnchecked(
      Matrix<?, ?> mat) {
    return new Matrix<>(mat.m_storage);
  }

  /**
   * Checks if another {@link Matrix} is identical to "this" one within a specified tolerance.
   *
   * <p>This will check if each element is in tolerance of the corresponding element from the other
   * {@link Matrix} or if the elements have the same symbolic meaning. For two elements to have the
   * same symbolic meaning they both must be either Double.NaN, Double.POSITIVE_INFINITY, or
   * Double.NEGATIVE_INFINITY.
   *
   * <p>NOTE:It is recommended to use {@link Matrix#isEqual(Matrix, double)} over this method when
   * checking if two matrices are equal as {@link Matrix#isEqual(Matrix, double)} will return false
   * if an element is uncountable. This method should only be used when uncountable elements need to
   * be compared.
   *
   * @param other The {@link Matrix} to check against this one.
   * @param tolerance The tolerance to check equality with.
   * @return true if this matrix is identical to the one supplied.
   */
  public boolean isIdentical(Matrix<?, ?> other, double tolerance) {
    return MatrixFeatures_DDRM.isIdentical(
        this.m_storage.getDDRM(), other.m_storage.getDDRM(), tolerance);
  }

  /**
   * Checks if another {@link Matrix} is equal to "this" within a specified tolerance.
   *
   * <p>This will check if each element is in tolerance of the corresponding element from the other
   * {@link Matrix}.
   *
   * <p>tol &ge; |a<sub>ij</sub> - b<sub>ij</sub>|
   *
   * @param other The {@link Matrix} to check against this one.
   * @param tolerance The tolerance to check equality with.
   * @return true if this matrix is equal to the one supplied.
   */
  public boolean isEqual(Matrix<?, ?> other, double tolerance) {
    return MatrixFeatures_DDRM.isEquals(
        this.m_storage.getDDRM(), other.m_storage.getDDRM(), tolerance);
  }

  /**
   * Givens rotation.
   *
   * <p>Given a and b, find c = cosθ and s = sinθ such that
   *
   * <pre>
   * [c  -s][a] = [r]
   * [s   c][b]   [0]
   * </pre>
   *
   * <p>where r = √(a² + b²) is the length of the vector (a, b).
   */
  private class GivensRotation {
    public final double c;
    public final double s;
    public final double r;

    /**
     * Construct a Givens rotation.
     *
     * @param a a.
     * @param b b.
     */
    GivensRotation(double a, double b) {
      if (b == 0.0) {
        c = a < 0.0 ? -1.0 : 1.0;
        s = 0.0;
        r = Math.abs(a);
        return;
      }

      if (a == 0.0) {
        c = 0.0;
        s = b < 0.0 ? 1.0 : -1.0;
        r = Math.abs(b);
        return;
      }

      // Safe-range thresholds following [1].
      //
      // When both |a| and |b| lie in (rtmin, rtmax), the direct formula
      // r = a * √(1 + (b/a)²) cannot over- or underflow before the true result
      // would. Outside that range we prescale by max(|a|, |b|) (clamped into
      // [safe_min, safe_max]) so that the squared sum stays in the
      // representable range. This preserves the existing Eigen sign convention
      // (r ≥ 0, sign carried in c).
      //
      // [1] Anderson, "Algorithm 978: Safe Scaling in the Level 1 BLAS",
      //     ACM TOMS 44(1), 2017.
      final double safe_min = Double.MIN_VALUE;
      final double safe_max = 1.0 / safe_min;
      final double rtmin = Math.sqrt(safe_min);
      final double rtmax = Math.sqrt(safe_max / 2.0);
      final double abs_a = Math.abs(a);
      final double abs_b = Math.abs(b);
      final double mx = Math.max(abs_a, abs_b);
      final double mn = Math.min(abs_a, abs_b);

      if (mx < rtmax && mn > rtmin) {
        // Safe range: existing direct formulas are stable.
        if (abs_a > abs_b) {
          double t = b / a;
          double u = Math.sqrt(1.0 + t * t);
          if (a < 0.0) {
            u = -u;
          }
          c = 1.0 / u;
          s = -t * c;
          r = a * u;
        } else {
          double t = a / b;
          double u = Math.sqrt(1.0 + t * t);
          if (b < 0.0) {
            u = -u;
          }
          s = -1.0 / u;
          c = -t * s;
          r = b * u;
        }
      } else {
        // Out of safe range: prescale by max(|a|, |b|) clamped into
        // [safe_min, safe_max].
        final double scale = Math.clamp(mx, safe_min, safe_max);
        final double as = a / scale;
        final double bs = b / scale;
        if (abs_a > abs_b) {
          double t = bs / as;
          double u = Math.sqrt(1.0 + t * t);
          if (as < 0.0) {
            u = -u;
          }
          c = 1.0 / u;
          s = -t * c;
          r = (as * u) * scale;
        } else {
          double t = as / bs;
          double u = Math.sqrt(1.0 + t * t);
          if (bs < 0.0) {
            u = -u;
          }
          s = -1.0 / u;
          c = -t * s;
          r = (bs * u) * scale;
        }
      }
    }
  }

  /**
   * Performs an in-place Cholesky rank update (or downdate).
   *
   * <p>If this matrix contains L where A = LLᵀ before the update, it will contain L where LLᵀ = A +
   * σvvᵀ after the update.
   *
   * @param v Vector to use for the update.
   * @param sigma Sigma to use for the update.
   * @param lowerTriangular Whether this matrix is lower triangular.
   * @return True if rank update (or downdate) succeeded.
   */
  public boolean rankUpdate(Matrix<R, N1> v, double sigma, boolean lowerTriangular) {
    if (sigma == 0.0) {
      return true;
    }

    int n = getNumCols();

    if (!lowerTriangular) {
      TransposeAlgs_DDRM.square(this.m_storage.getDDRM());
    }

    final double[] ω = v.getStorage().getDDRM().data.clone();

    if (sigma > 0.0) {
      // This algorithm based on Givens rotations only works for updates

      final double sqrt_sigma = Math.sqrt(sigma);
      for (int i = 0; i < n; ++i) {
        ω[i] *= sqrt_sigma;
      }

      for (int j = 0; j < n; ++j) {
        final var g = new GivensRotation(get(j, j), -ω[j]);
        set(j, j, g.r);

        // Apply rotation in the plane
        if (g.c == 1.0 && g.s == 0.0) {
          continue;
        }
        for (int k = j + 1; k < n; ++k) {
          final double l_kj = get(k, j);
          final double ω_k = ω[k];
          set(k, j, g.c * l_kj + g.s * ω_k);
          ω[k] = -g.s * l_kj + g.c * ω_k;
        }
      }
    } else {
      // Algorithm 3.1 from https://christian-igel.github.io/paper/AMERCMAUfES.pdf
      // where α = 1 and their β is our σ
      double b = 1.0;
      for (int j = 0; j < n; ++j) {
        final double l_jj = get(j, j);
        if (l_jj == 0.0) {
          if (!lowerTriangular) {
            TransposeAlgs_DDRM.square(this.m_storage.getDDRM());
          }
          return false;
        }

        final double l_jj2 = l_jj * l_jj;
        final double σω_j2 = sigma * ω[j] * ω[j];
        final double x = l_jj2 + σω_j2 / b;
        if (x < 0.0) {
          if (!lowerTriangular) {
            TransposeAlgs_DDRM.square(this.m_storage.getDDRM());
          }
          return false;
        }
        final double lp_jj = Math.sqrt(x);
        set(j, j, lp_jj);

        final double γ = l_jj2 * b + σω_j2;
        if (γ == 0.0) {
          if (!lowerTriangular) {
            TransposeAlgs_DDRM.square(this.m_storage.getDDRM());
          }
          return false;
        }

        for (int k = j + 1; k < n; ++k) {
          final double l_kj = get(k, j);
          ω[k] -= ω[j] / l_jj * l_kj;
          set(k, j, lp_jj / l_jj * l_kj + lp_jj * sigma * ω[j] / γ * ω[k]);
        }

        b += σω_j2 / l_jj2;
      }
    }

    if (!lowerTriangular) {
      TransposeAlgs_DDRM.square(this.m_storage.getDDRM());
    }

    return true;
  }

  @Override
  public String toString() {
    return m_storage.toString();
  }

  /**
   * Checks if an object is equal to this {@link Matrix}.
   *
   * <p>a<sub>ij</sub> == b<sub>ij</sub>
   *
   * @param other The Object to check against this {@link Matrix}.
   * @return true if the object supplied is a {@link Matrix} and is equal to this matrix.
   */
  @Override
  public boolean equals(Object other) {
    return this == other
        || other instanceof Matrix<?, ?> matrix
            && !MatrixFeatures_DDRM.hasUncountable(matrix.m_storage.getDDRM())
            && MatrixFeatures_DDRM.isEquals(this.m_storage.getDDRM(), matrix.m_storage.getDDRM());
  }

  @Override
  public int hashCode() {
    return Objects.hash(m_storage);
  }

  /**
   * Creates an implementation of the {@link Protobuf} interface for matrices.
   *
   * @param <R> The number of rows of the matrices this serializer processes.
   * @param <C> The number of cols of the matrices this serializer processes.
   * @param rows The number of rows of the matrices this serializer processes.
   * @param cols The number of cols of the matrices this serializer processes.
   * @return The protobuf implementation.
   */
  public static <R extends Num, C extends Num> MatrixProto<R, C> getProto(
      Nat<R> rows, Nat<C> cols) {
    return new MatrixProto<>(rows, cols);
  }

  /**
   * Creates an implementation of the {@link Struct} interfaces for matrices.
   *
   * @param <R> The number of rows of the matrices this serializer processes.
   * @param <C> The number of cols of the matrices this serializer processes.
   * @param rows The number of rows of the matrices this serializer processes.
   * @param cols The number of cols of the matrices this serializer processes.
   * @return The struct implementation.
   */
  public static <R extends Num, C extends Num> MatrixStruct<R, C> getStruct(
      Nat<R> rows, Nat<C> cols) {
    return new MatrixStruct<>(rows, cols);
  }
}
