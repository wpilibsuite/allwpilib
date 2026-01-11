// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.autodiff;

import java.util.Iterator;
import java.util.NoSuchElementException;
import java.util.function.BinaryOperator;
import java.util.function.UnaryOperator;
import java.util.stream.Stream;
import java.util.stream.StreamSupport;
import org.ejml.simple.SimpleMatrix;

/** A matrix of autodiff variables. */
public class VariableMatrix implements AutoCloseable, Iterable<Variable> {
  private final Variable[] m_storage;
  private int m_rows;
  private int m_cols;

  /**
   * Constructs a VariableMatrix from Variable internal handles.
   *
   * <p>This constructor is for internal use only.
   *
   * @param rows The number of matrix rows.
   * @param cols The number of matrix columns.
   * @param handles Variable handles.
   */
  public VariableMatrix(int rows, int cols, long[] handles) {
    assert handles.length == rows * cols;

    m_rows = rows;
    m_cols = cols;
    m_storage = new Variable[rows * cols];
    for (int index = 0; index < m_storage.length; ++index) {
      m_storage[index] = new Variable(Variable.HANDLE, handles[index]);
    }
  }

  /**
   * Constructs a zero-initialized VariableMatrix column vector with the given rows.
   *
   * @param rows The number of matrix rows.
   */
  public VariableMatrix(int rows) {
    this(rows, 1);
  }

  /**
   * Constructs a zero-initialized VariableMatrix with the given dimensions.
   *
   * @param rows The number of matrix rows.
   * @param cols The number of matrix columns.
   */
  public VariableMatrix(int rows, int cols) {
    m_rows = rows;
    m_cols = cols;
    m_storage = new Variable[rows * cols];
    for (int index = 0; index < m_storage.length; ++index) {
      m_storage[index] = new Variable();
    }
  }

  /**
   * Constructs a scalar VariableMatrix from a nested list of doubles.
   *
   * @param list The nested list of Variables.
   */
  public VariableMatrix(double[][] list) {
    // Get row and column counts for destination matrix
    m_rows = list.length;
    m_cols = 0;
    if (list.length > 0) {
      m_cols = list[0].length;
    }

    // Assert all column counts are the same
    for (var row : list) {
      assert row.length == m_cols;
    }

    m_storage = new Variable[m_rows * m_cols];
    int index = 0;
    for (var row : list) {
      for (var elem : row) {
        m_storage[index] = new Variable(elem);
        ++index;
      }
    }
  }

  /**
   * Constructs a scalar VariableMatrix from a nested list of Variables.
   *
   * @param list The nested list of Variables.
   */
  public VariableMatrix(Variable[][] list) {
    // Get row and column counts for destination matrix
    m_rows = list.length;
    m_cols = 0;
    if (list.length > 0) {
      m_cols = list[0].length;
    }

    // Assert all column counts are the same
    for (var row : list) {
      assert row.length == m_cols;
    }

    m_storage = new Variable[m_rows * m_cols];
    int index = 0;
    for (var row : list) {
      for (var elem : row) {
        m_storage[index] = elem;
        ++index;
      }
    }
  }

  /**
   * Constructs a VariableMatrix from an EJML matrix.
   *
   * @param values EJML matrix of values.
   */
  public VariableMatrix(SimpleMatrix values) {
    m_rows = values.getNumRows();
    m_cols = values.getNumCols();
    m_storage = new Variable[m_rows * m_cols];
    for (int row = 0; row < values.getNumRows(); ++row) {
      for (int col = 0; col < values.getNumCols(); ++col) {
        m_storage[row * m_cols + col] = new Variable(values.get(row, col));
      }
    }
  }

  /**
   * Constructs a scalar VariableMatrix from a Variable.
   *
   * @param variable Variable.
   */
  public VariableMatrix(Variable variable) {
    m_rows = 1;
    m_cols = 1;
    m_storage = new Variable[] {variable};
  }

  /**
   * Constructs a VariableMatrix from a VariableBlock.
   *
   * @param values VariableBlock of values.
   */
  public VariableMatrix(VariableBlock values) {
    m_rows = values.rows();
    m_cols = values.cols();
    m_storage = new Variable[m_rows * m_cols];
    for (int row = 0; row < m_rows; ++row) {
      for (int col = 0; col < m_cols; ++col) {
        m_storage[row * m_cols + col] = values.get(row, col);
      }
    }
  }

  @Override
  public void close() {
    for (int index = 0; index < rows() * cols(); ++index) {
      m_storage[index].close();
    }
  }

  /**
   * Assigns a double array to a VariableMatrix.
   *
   * @param values Double array of values.
   * @return This VariableMatrix.
   */
  public VariableMatrix set(double[][] values) {
    assert rows() == values.length;

    // Assert all column counts are the same
    for (var row : values) {
      assert row.length == cols();
    }

    for (int row = 0; row < values.length; ++row) {
      for (int col = 0; col < values[0].length; ++col) {
        set(row, col, values[row][col]);
      }
    }

    return this;
  }

  /**
   * Assigns an EJML matrix to a VariableMatrix.
   *
   * @param values EJML matrix of values.
   * @return This VariableMatrix.
   */
  public VariableMatrix set(SimpleMatrix values) {
    assert rows() == values.getNumRows() && cols() == values.getNumCols();

    for (int row = 0; row < values.getNumRows(); ++row) {
      for (int col = 0; col < values.getNumCols(); ++col) {
        set(row, col, values.get(row, col));
      }
    }

    return this;
  }

  /**
   * Assigns a VariableMatrix to a VariableMatrix.
   *
   * @param values VariableMatrix of values.
   * @return This VariableMatrix.
   */
  public VariableMatrix set(VariableMatrix values) {
    assert rows() == values.rows() && cols() == values.cols();

    for (int row = 0; row < values.rows(); ++row) {
      for (int col = 0; col < values.cols(); ++col) {
        set(row, col, values.get(row, col));
      }
    }

    return this;
  }

  /**
   * Assigns a VariableBlock to a VariableMatrix.
   *
   * @param values VariableBlock of values.
   * @return This VariableMatrix.
   */
  public VariableMatrix set(VariableBlock values) {
    assert rows() == values.rows() && cols() == values.cols();

    for (int row = 0; row < values.rows(); ++row) {
      for (int col = 0; col < values.cols(); ++col) {
        set(row, col, values.get(row, col));
      }
    }

    return this;
  }

  /**
   * Assigns a double to the matrix.
   *
   * <p>This only works for matrices with one row and one column.
   *
   * @param value Value to assign.
   * @return This VariableMatrix.
   */
  public VariableMatrix set(double value) {
    return set(new Variable(value));
  }

  /**
   * Assigns a Variable to the matrix.
   *
   * <p>This only works for matrices with one row and one column.
   *
   * @param value Value to assign.
   * @return This VariableMatrix.
   */
  public VariableMatrix set(Variable value) {
    assert rows() == 1 && cols() == 1;

    m_storage[0] = value;

    return this;
  }

  /**
   * Sets an element to the given value.
   *
   * @param row The row.
   * @param col The column.
   * @param value The value.
   */
  public void set(int row, int col, Variable value) {
    assert row >= 0 && row < rows();
    assert col >= 0 && col < cols();
    m_storage[row * cols() + col] = value;
  }

  /**
   * Sets an element to the given value.
   *
   * @param row The row.
   * @param col The column.
   * @param value The value.
   */
  public void set(int row, int col, double value) {
    assert row >= 0 && row < rows();
    assert col >= 0 && col < cols();
    m_storage[row * cols() + col] = new Variable(value);
  }

  /**
   * Sets an element to the given value.
   *
   * @param index The index of the element.
   * @param value The value.
   */
  public void set(int index, double value) {
    set(index, new Variable(value));
  }

  /**
   * Sets an element to the given value.
   *
   * @param index The index of the element.
   * @param value The value.
   */
  public void set(int index, Variable value) {
    assert index >= 0 && index < rows() * cols();
    m_storage[index] = value;
  }

  /**
   * Sets the VariableMatrix's internal values.
   *
   * @param values Double array of values.
   */
  public void setValue(double[][] values) {
    assert rows() == values.length;

    // Assert all column counts are the same
    for (var row : values) {
      assert row.length == cols();
    }

    for (int row = 0; row < rows(); ++row) {
      for (int col = 0; col < cols(); ++col) {
        get(row, col).setValue(values[row][col]);
      }
    }
  }

  /**
   * Sets the VariableMatrix's internal values.
   *
   * @param values EJML matrix of values.
   */
  public void setValue(SimpleMatrix values) {
    assert rows() == values.getNumRows() && cols() == values.getNumCols();

    for (int row = 0; row < values.getNumRows(); ++row) {
      for (int col = 0; col < values.getNumCols(); ++col) {
        get(row, col).setValue(values.get(row, col));
      }
    }
  }

  /**
   * Returns the element at the given row and column.
   *
   * @param row The row.
   * @param col The column.
   * @return The element at the given row and column.
   */
  public Variable get(int row, int col) {
    assert row >= 0 && row < rows();
    assert col >= 0 && col < cols();
    return m_storage[row * cols() + col];
  }

  /**
   * Returns the element at the given index.
   *
   * @param index The index.
   * @return The element at the given index.
   */
  public Variable get(int index) {
    assert index >= 0 && index < rows() * cols();
    return m_storage[index];
  }

  /**
   * Returns a slice of the variable matrix.
   *
   * @param row The row.
   * @param colSlice The column slice.
   * @return A slice of the variable matrix.
   */
  public VariableBlock get(int row, Slice.None colSlice) {
    return get(new Slice(row), new Slice(colSlice));
  }

  /**
   * Returns a slice of the variable matrix.
   *
   * @param row The row.
   * @param colSlice The column slice.
   * @return A slice of the variable matrix.
   */
  public VariableBlock get(int row, Slice colSlice) {
    return get(new Slice(row), colSlice);
  }

  /**
   * Returns a slice of the variable matrix.
   *
   * @param rowSlice The row slice.
   * @param col The column.
   * @return A slice of the variable matrix.
   */
  public VariableBlock get(Slice.None rowSlice, int col) {
    return get(new Slice(rowSlice), new Slice(col));
  }

  /**
   * Returns a slice of the variable matrix.
   *
   * @param rowSlice The row slice.
   * @param col The column.
   * @return A slice of the variable matrix.
   */
  public VariableBlock get(Slice rowSlice, int col) {
    return get(rowSlice, new Slice(col));
  }

  /**
   * Returns a slice of the variable matrix.
   *
   * @param rowSlice The row slice.
   * @param colSlice The column slice.
   * @return A slice of the variable matrix.
   */
  public VariableBlock get(Slice.None rowSlice, Slice.None colSlice) {
    return get(new Slice(rowSlice), new Slice(colSlice));
  }

  /**
   * Returns a slice of the variable matrix.
   *
   * @param rowSlice The row slice.
   * @param colSlice The column slice.
   * @return A slice of the variable matrix.
   */
  public VariableBlock get(Slice.None rowSlice, Slice colSlice) {
    return get(new Slice(rowSlice), colSlice);
  }

  /**
   * Returns a slice of the variable matrix.
   *
   * @param rowSlice The row slice.
   * @param colSlice The column slice.
   * @return A slice of the variable matrix.
   */
  public VariableBlock get(Slice rowSlice, Slice.None colSlice) {
    return get(rowSlice, new Slice(colSlice));
  }

  /**
   * Returns a slice of the variable matrix.
   *
   * @param rowSlice The row slice.
   * @param colSlice The column slice.
   * @return A slice of the variable matrix.
   */
  public VariableBlock get(Slice rowSlice, Slice colSlice) {
    int rowSliceLength = rowSlice.adjust(rows());
    int colSliceLength = colSlice.adjust(cols());
    return new VariableBlock(this, rowSlice, rowSliceLength, colSlice, colSliceLength);
  }

  /**
   * Returns a block of the variable matrix.
   *
   * @param rowOffset The row offset of the block selection.
   * @param colOffset The column offset of the block selection.
   * @param blockRows The number of rows in the block selection.
   * @param blockCols The number of columns in the block selection.
   * @return A block of the variable matrix.
   */
  public VariableBlock block(int rowOffset, int colOffset, int blockRows, int blockCols) {
    assert rowOffset >= 0 && rowOffset <= rows();
    assert colOffset >= 0 && colOffset <= cols();
    assert blockRows >= 0 && blockRows <= rows() - rowOffset;
    assert blockCols >= 0 && blockCols <= cols() - colOffset;
    return new VariableBlock(this, rowOffset, colOffset, blockRows, blockCols);
  }

  /**
   * Returns a segment of the variable vector.
   *
   * @param offset The offset of the segment.
   * @param length The length of the segment.
   * @return A segment of the variable vector.
   */
  public VariableBlock segment(int offset, int length) {
    assert cols() == 1;
    assert offset >= 0 && offset < rows();
    assert length >= 0 && length <= rows() - offset;
    return block(offset, 0, length, 1);
  }

  /**
   * Returns a row slice of the variable matrix.
   *
   * @param row The row to slice.
   * @return A row slice of the variable matrix.
   */
  public VariableBlock row(int row) {
    assert row >= 0 && row < rows();
    return block(row, 0, 1, cols());
  }

  /**
   * Returns a column slice of the variable matrix.
   *
   * @param col The column to slice.
   * @return A column slice of the variable matrix.
   */
  public VariableBlock col(int col) {
    assert col >= 0 && col < cols();
    return block(0, col, rows(), 1);
  }

  /**
   * Matrix multiplication operator.
   *
   * @param rhs Operator right-hand side.
   * @return Result of matrix multiplication.
   */
  public VariableMatrix times(VariableMatrix rhs) {
    assert cols() == rhs.rows();

    var result = new VariableMatrix(rows(), rhs.cols());

    for (int i = 0; i < rows(); ++i) {
      for (int j = 0; j < rhs.cols(); ++j) {
        var sum = new Variable(0.0);
        for (int k = 0; k < cols(); ++k) {
          sum = sum.plus(get(i, k).times(rhs.get(k, j)));
        }
        result.set(i, j, sum);
      }
    }

    return result;
  }

  /**
   * Matrix multiplication operator.
   *
   * @param rhs Operator right-hand side.
   * @return Result of matrix multiplication.
   */
  public VariableMatrix times(VariableBlock rhs) {
    assert cols() == rhs.rows();

    var result = new VariableMatrix(rows(), rhs.cols());

    for (int i = 0; i < rows(); ++i) {
      for (int j = 0; j < rhs.cols(); ++j) {
        var sum = new Variable(0.0);
        for (int k = 0; k < cols(); ++k) {
          sum = sum.plus(get(i, k).times(rhs.get(k, j)));
        }
        result.set(i, j, sum);
      }
    }

    return result;
  }

  /**
   * Matrix-scalar multiplication operator.
   *
   * @param rhs Operator right-hand side.
   * @return Result of matrix-scalar multiplication.
   */
  public VariableMatrix times(double rhs) {
    return times(new Variable(rhs));
  }

  /**
   * Matrix-scalar multiplication operator.
   *
   * @param rhs Operator right-hand side.
   * @return Result of matrix-scalar multiplication.
   */
  public VariableMatrix times(Variable rhs) {
    var result = new VariableMatrix(rows(), cols());

    for (int row = 0; row < result.rows(); ++row) {
      for (int col = 0; col < result.cols(); ++col) {
        result.set(row, col, get(row, col).times(rhs));
      }
    }

    return result;
  }

  /**
   * Binary division operator.
   *
   * @param rhs Operator right-hand side.
   * @return Result of division.
   */
  public VariableMatrix div(double rhs) {
    return div(new Variable(rhs));
  }

  /**
   * Binary division operator.
   *
   * @param rhs Operator right-hand side.
   * @return Result of division.
   */
  public VariableMatrix div(Variable rhs) {
    var result = new VariableMatrix(rows(), cols());

    for (int row = 0; row < result.rows(); ++row) {
      for (int col = 0; col < result.cols(); ++col) {
        result.set(row, col, get(row, col).div(rhs));
      }
    }

    return result;
  }

  /**
   * Binary addition operator.
   *
   * @param rhs Operator right-hand side.
   * @return Result of addition.
   */
  public VariableMatrix plus(VariableMatrix rhs) {
    assert rows() == rhs.rows() && cols() == rhs.cols();

    var result = new VariableMatrix(rows(), cols());

    for (int row = 0; row < result.rows(); ++row) {
      for (int col = 0; col < result.cols(); ++col) {
        result.set(row, col, get(row, col).plus(rhs.get(row, col)));
      }
    }

    return result;
  }

  /**
   * Binary addition operator.
   *
   * @param rhs Operator right-hand side.
   * @return Result of addition.
   */
  public VariableMatrix plus(VariableBlock rhs) {
    assert rows() == rhs.rows() && cols() == rhs.cols();

    var result = new VariableMatrix(rows(), cols());

    for (int row = 0; row < result.rows(); ++row) {
      for (int col = 0; col < result.cols(); ++col) {
        result.set(row, col, get(row, col).plus(rhs.get(row, col)));
      }
    }

    return result;
  }

  /**
   * Binary addition operator.
   *
   * @param rhs Operator right-hand side.
   * @return Result of addition.
   */
  public VariableMatrix plus(SimpleMatrix rhs) {
    assert rows() == rhs.getNumRows() && cols() == rhs.getNumCols();

    var result = new VariableMatrix(rows(), cols());

    for (int row = 0; row < result.rows(); ++row) {
      for (int col = 0; col < result.cols(); ++col) {
        result.set(row, col, get(row, col).plus(rhs.get(row, col)));
      }
    }

    return result;
  }

  /**
   * Binary subtraction operator.
   *
   * @param rhs Operator right-hand side.
   * @return Result of subtraction.
   */
  public VariableMatrix minus(VariableMatrix rhs) {
    assert rows() == rhs.rows() && cols() == rhs.cols();

    var result = new VariableMatrix(rows(), cols());

    for (int row = 0; row < result.rows(); ++row) {
      for (int col = 0; col < result.cols(); ++col) {
        result.set(row, col, get(row, col).minus(rhs.get(row, col)));
      }
    }

    return result;
  }

  /**
   * Binary subtraction operator.
   *
   * @param rhs Operator right-hand side.
   * @return Result of subtraction.
   */
  public VariableMatrix minus(VariableBlock rhs) {
    assert rows() == rhs.rows() && cols() == rhs.cols();

    var result = new VariableMatrix(rows(), cols());

    for (int row = 0; row < result.rows(); ++row) {
      for (int col = 0; col < result.cols(); ++col) {
        result.set(row, col, get(row, col).minus(rhs.get(row, col)));
      }
    }

    return result;
  }

  /**
   * Binary subtraction operator.
   *
   * @param rhs Operator right-hand side.
   * @return Result of subtraction.
   */
  public VariableMatrix minus(SimpleMatrix rhs) {
    assert rows() == rhs.getNumRows() && cols() == rhs.getNumCols();

    var result = new VariableMatrix(rows(), cols());

    for (int row = 0; row < result.rows(); ++row) {
      for (int col = 0; col < result.cols(); ++col) {
        result.set(row, col, get(row, col).minus(rhs.get(row, col)));
      }
    }

    return result;
  }

  /**
   * Unary minus operator.
   *
   * @return Result of unary minus.
   */
  public VariableMatrix unaryMinus() {
    var result = new VariableMatrix(rows(), cols());

    for (int row = 0; row < result.rows(); ++row) {
      for (int col = 0; col < result.cols(); ++col) {
        result.set(row, col, get(row, col).unaryMinus());
      }
    }

    return result;
  }

  /**
   * Returns the transpose of the variable matrix.
   *
   * @return The transpose of the variable matrix.
   */
  public VariableMatrix T() {
    var result = new VariableMatrix(cols(), rows());

    for (int row = 0; row < rows(); ++row) {
      for (int col = 0; col < cols(); ++col) {
        result.set(col, row, get(row, col));
      }
    }

    return result;
  }

  /**
   * Returns the number of rows in the matrix.
   *
   * @return The number of rows in the matrix.
   */
  public int rows() {
    return m_rows;
  }

  /**
   * Returns the number of columns in the matrix.
   *
   * @return The number of columns in the matrix.
   */
  public int cols() {
    return m_cols;
  }

  /**
   * Returns an element of the variable matrix.
   *
   * @param row The row of the element to return.
   * @param col The column of the element to return.
   * @return An element of the variable matrix.
   */
  public double value(int row, int col) {
    return get(row, col).value();
  }

  /**
   * Returns an element of the variable matrix.
   *
   * @param index The index of the element to return.
   * @return An element of the variable matrix.
   */
  public double value(int index) {
    return get(index).value();
  }

  /**
   * Returns the contents of the variable matrix.
   *
   * @return The contents of the variable matrix.
   */
  public SimpleMatrix value() {
    var result = new SimpleMatrix(rows(), cols());

    for (int row = 0; row < rows(); ++row) {
      for (int col = 0; col < cols(); ++col) {
        result.set(row, col, value(row, col));
      }
    }

    return result;
  }

  /**
   * Maps the matrix coefficient-wise with an unary operator.
   *
   * @param unaryOp The unary operator to use for the map operation.
   * @return Result of the unary operator.
   */
  public VariableMatrix cwiseMap(UnaryOperator<Variable> unaryOp) {
    var result = new VariableMatrix(rows(), cols());

    for (int row = 0; row < rows(); ++row) {
      for (int col = 0; col < cols(); ++col) {
        result.set(row, col, unaryOp.apply(get(row, col)));
      }
    }

    return result;
  }

  /**
   * Returns number of elements in matrix.
   *
   * @return Number of elements in matrix.
   */
  public int size() {
    return m_storage.length;
  }

  @Override
  public Iterator<Variable> iterator() {
    return new Iterator<>() {
      private int m_index = 0;

      @Override
      public boolean hasNext() {
        return m_index < VariableMatrix.this.size();
      }

      @Override
      public Variable next() {
        if (!hasNext()) {
          throw new NoSuchElementException();
        }

        return VariableMatrix.this.get(m_index++);
      }
    };
  }

  /**
   * Creates a Stream of VariableMatrix elements.
   *
   * @return A Stream of VariableMatrix elements.
   */
  public Stream<Variable> stream() {
    return StreamSupport.stream(spliterator(), false);
  }

  /**
   * Returns a variable matrix filled with zeroes.
   *
   * @param rows The number of matrix rows.
   * @param cols The number of matrix columns.
   * @return A variable matrix filled with zeroes.
   */
  public static VariableMatrix zero(int rows, int cols) {
    return new VariableMatrix(new SimpleMatrix(rows, cols));
  }

  /**
   * Returns a variable matrix filled with ones.
   *
   * @param rows The number of matrix rows.
   * @param cols The number of matrix columns.
   * @return A variable matrix filled with ones.
   */
  public static VariableMatrix ones(int rows, int cols) {
    return new VariableMatrix(SimpleMatrix.ones(rows, cols));
  }

  /**
   * Applies a coefficient-wise reduce operation to two matrices.
   *
   * @param lhs The left-hand side of the binary operator.
   * @param rhs The right-hand side of the binary operator.
   * @param binaryOp The binary operator to use for the reduce operation.
   * @return Result of binary operator.
   */
  public static VariableMatrix cwiseReduce(
      VariableMatrix lhs, VariableMatrix rhs, BinaryOperator<Variable> binaryOp) {
    assert lhs.rows() == rhs.rows() && lhs.cols() == rhs.cols();

    var result = new VariableMatrix(lhs.rows(), lhs.cols());

    for (int row = 0; row < lhs.rows(); ++row) {
      for (int col = 0; col < lhs.cols(); ++col) {
        result.set(row, col, binaryOp.apply(lhs.get(row, col), rhs.get(row, col)));
      }
    }

    return result;
  }

  /**
   * Assemble a VariableMatrix from a nested list of blocks.
   *
   * <p>Each row's blocks must have the same height, and the assembled block rows must have the same
   * width. For example, for the block matrix [[A, B], [C]] to be constructible, the number of rows
   * in A and B must match, and the number of columns in [A, B] and [C] must match.
   *
   * @param list The nested list of blocks.
   * @return Block matrix.
   */
  @SuppressWarnings("OverloadMethodsDeclarationOrder")
  public static VariableMatrix block(VariableMatrix[][] list) {
    // Get row and column counts for destination matrix
    int rows = 0;
    int cols = -1;
    for (var row : list) {
      if (row.length > 0) {
        rows += row[0].rows();
      }

      // Get number of columns in this row
      int latestCols = 0;
      for (var elem : row) {
        // Assert the first and latest row have the same height
        assert row[0].rows() == elem.rows();

        latestCols += elem.cols();
      }

      // If this is the first row, record the column count. Otherwise, assert the
      // first and latest column counts are the same.
      if (cols == -1) {
        cols = latestCols;
      } else {
        assert cols == latestCols;
      }
    }

    var result = new VariableMatrix(rows, cols);

    int rowOffset = 0;
    for (var row : list) {
      int colOffset = 0;
      for (var elem : row) {
        result.block(rowOffset, colOffset, elem.rows(), elem.cols()).set(elem);
        colOffset += elem.cols();
      }
      if (row.length > 0) {
        rowOffset += row[0].rows();
      }
    }

    return result;
  }

  /**
   * Solves the VariableMatrix equation AX = B for X.
   *
   * @param A The left-hand side.
   * @param B The right-hand side.
   * @return The solution X.
   */
  public static VariableMatrix solve(VariableMatrix A, VariableMatrix B) {
    // m x n * n x p = m x p
    assert A.rows() == B.rows();

    return new VariableMatrix(
        A.cols(),
        B.cols(),
        VariableMatrixJNI.solve(A.getHandles(), A.cols(), B.getHandles(), B.cols()));
  }

  /**
   * Returns an array of VariableMatrix internal handles in row-major order.
   *
   * @return Array of VariableMatrix internal handles in row-major order.
   */
  long[] getHandles() {
    var handles = new long[size()];
    for (int index = 0; index < size(); ++index) {
      handles[index] = m_storage[index].getHandle();
    }
    return handles;
  }
}
