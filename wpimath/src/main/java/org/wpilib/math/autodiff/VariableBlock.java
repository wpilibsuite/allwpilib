// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.autodiff;

import java.util.Iterator;
import java.util.NoSuchElementException;
import java.util.function.UnaryOperator;
import java.util.stream.Stream;
import java.util.stream.StreamSupport;
import org.ejml.simple.SimpleMatrix;

/** A submatrix of autodiff variables with reference semantics. */
public class VariableBlock implements Iterable<Variable> {
  private final VariableMatrix m_mat;

  private final Slice m_rowSlice;
  private final int m_rowSliceLength;

  private final Slice m_colSlice;
  private final int m_colSliceLength;

  /**
   * Constructs a Variable block pointing to all of the given matrix.
   *
   * @param mat The matrix to which to point.
   */
  public VariableBlock(VariableMatrix mat) {
    this(mat, 0, 0, mat.rows(), mat.cols());
  }

  /**
   * Constructs a Variable block pointing to a subset of the given matrix.
   *
   * @param mat The matrix to which to point.
   * @param rowOffset The block's row offset.
   * @param colOffset The block's column offset.
   * @param blockRows The number of rows in the block.
   * @param blockCols The number of columns in the block.
   */
  public VariableBlock(
      VariableMatrix mat, int rowOffset, int colOffset, int blockRows, int blockCols) {
    m_mat = mat;
    m_rowSlice = new Slice(rowOffset, rowOffset + blockRows, 1);
    m_rowSliceLength = m_rowSlice.adjust(mat.rows());
    m_colSlice = new Slice(colOffset, colOffset + blockCols, 1);
    m_colSliceLength = m_colSlice.adjust(mat.cols());
  }

  /**
   * Constructs a Variable block pointing to a subset of the given matrix.
   *
   * <p>Note that the slices are taken as is rather than adjusted.
   *
   * @param mat The matrix to which to point.
   * @param rowSlice The block's row slice.
   * @param rowSliceLength The block's row length.
   * @param colSlice The block's column slice.
   * @param colSliceLength The block's column length.
   */
  public VariableBlock(
      VariableMatrix mat, Slice rowSlice, int rowSliceLength, Slice colSlice, int colSliceLength) {
    m_mat = mat;
    m_rowSlice = rowSlice;
    m_rowSliceLength = rowSliceLength;
    m_colSlice = colSlice;
    m_colSliceLength = colSliceLength;
  }

  /**
   * Assigns a double to the block.
   *
   * <p>This only works for blocks with one row and one column.
   *
   * @param value Value to assign.
   * @return This VariableBlock.
   */
  public VariableBlock set(double value) {
    assert rows() == 1 && cols() == 1;

    set(0, 0, new Variable(value));

    return this;
  }

  /**
   * Assigns a Variable to the block.
   *
   * <p>This only works for blocks with one row and one column.
   *
   * @param value Value to assign.
   * @return This VariableBlock.
   */
  public VariableBlock set(Variable value) {
    assert rows() == 1 && cols() == 1;

    set(0, 0, value);

    return this;
  }

  /**
   * Assigns a double array to the block.
   *
   * @param values Double array of values to assign.
   * @return This VariableBlock.
   */
  public VariableBlock set(double[][] values) {
    assert rows() == values.length;

    // Assert all column counts are the same
    for (var row : values) {
      assert row.length == cols();
    }

    for (int row = 0; row < rows(); ++row) {
      for (int col = 0; col < cols(); ++col) {
        set(row, col, values[row][col]);
      }
    }

    return this;
  }

  /**
   * Assigns an EJML matrix to the block.
   *
   * @param values EJML matrix of values to assign.
   * @return This VariableBlock.
   */
  public VariableBlock set(SimpleMatrix values) {
    assert rows() == values.getNumRows() && cols() == values.getNumCols();

    for (int row = 0; row < rows(); ++row) {
      for (int col = 0; col < cols(); ++col) {
        set(row, col, values.get(row, col));
      }
    }

    return this;
  }

  /**
   * Assigns a VariableMatrix to the block.
   *
   * @param values VariableMatrix of values.
   * @return This VariableBlock.
   */
  public VariableBlock set(VariableMatrix values) {
    assert rows() == values.rows() && cols() == values.cols();

    for (int row = 0; row < rows(); ++row) {
      for (int col = 0; col < cols(); ++col) {
        set(row, col, values.get(row, col));
      }
    }
    return this;
  }

  /**
   * Assigns a VariableBlock to the block.
   *
   * @param values VariableBlock of values.
   * @return This VariableBlock.
   */
  public VariableBlock set(VariableBlock values) {
    assert rows() == values.rows() && cols() == values.cols();

    for (int row = 0; row < rows(); ++row) {
      for (int col = 0; col < cols(); ++col) {
        set(row, col, values.get(row, col));
      }
    }
    return this;
  }

  /**
   * Sets a scalar subblock at the given row and column.
   *
   * @param row The scalar subblock's row.
   * @param col The scalar subblock's column.
   * @param value The value.
   */
  public void set(int row, int col, Variable value) {
    assert row >= 0 && row < rows();
    assert col >= 0 && col < cols();
    m_mat.set(
        m_rowSlice.start + row * m_rowSlice.step, m_colSlice.start + col * m_colSlice.step, value);
  }

  /**
   * Sets a scalar subblock at the given row and column.
   *
   * @param row The scalar subblock's row.
   * @param col The scalar subblock's column.
   * @param value The value.
   */
  public void set(int row, int col, double value) {
    assert row >= 0 && row < rows();
    assert col >= 0 && col < cols();
    m_mat.set(
        m_rowSlice.start + row * m_rowSlice.step, m_colSlice.start + col * m_colSlice.step, value);
  }

  /**
   * Sets a scalar subblock at the given index.
   *
   * @param index The scalar subblock's index.
   * @param value The value.
   */
  public void set(int index, double value) {
    set(index, new Variable(value));
  }

  /**
   * Sets a scalar subblock at the given index.
   *
   * @param index The scalar subblock's index.
   * @param value The value.
   */
  public void set(int index, Variable value) {
    assert index >= 0 && index < rows() * cols();
    set(index / cols(), index % cols(), value);
  }

  /**
   * Assigns a double to the block.
   *
   * <p>This only works for blocks with one row and one column.
   *
   * @param value Value to assign.
   */
  public void setValue(double value) {
    assert rows() == 1 && cols() == 1;

    get(0, 0).setValue(value);
  }

  /**
   * Sets block's internal values.
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
   * Sets block's internal values.
   *
   * @param values EJML matrix of values.
   */
  public void setValue(SimpleMatrix values) {
    assert rows() == values.getNumRows() && cols() == values.getNumCols();

    for (int row = 0; row < rows(); ++row) {
      for (int col = 0; col < cols(); ++col) {
        get(row, col).setValue(values.get(row, col));
      }
    }
  }

  /**
   * Returns a scalar subblock at the given row and column.
   *
   * @param row The scalar subblock's row.
   * @param col The scalar subblock's column.
   * @return A scalar subblock at the given row and column.
   */
  public Variable get(int row, int col) {
    assert row >= 0 && row < rows();
    assert col >= 0 && col < cols();
    return m_mat.get(
        m_rowSlice.start + row * m_rowSlice.step, m_colSlice.start + col * m_colSlice.step);
  }

  /**
   * Returns a scalar subblock at the given index.
   *
   * @param index The scalar subblock's index.
   * @return A scalar subblock at the given index.
   */
  public Variable get(int index) {
    assert index >= 0 && index < rows() * cols();
    return get(index / cols(), index % cols());
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
    int rowSliceLength = rowSlice.adjust(m_rowSliceLength);
    int colSliceLength = colSlice.adjust(m_colSliceLength);
    return new VariableBlock(
        m_mat,
        new Slice(
            m_rowSlice.start + rowSlice.start * m_rowSlice.step,
            m_rowSlice.start + rowSlice.stop * m_rowSlice.step,
            rowSlice.step * m_rowSlice.step),
        rowSliceLength,
        new Slice(
            m_colSlice.start + colSlice.start * m_colSlice.step,
            m_colSlice.start + colSlice.stop * m_colSlice.step,
            colSlice.step * m_colSlice.step),
        colSliceLength);
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
    return get(
        new Slice(rowOffset, rowOffset + blockRows, 1),
        new Slice(colOffset, colOffset + blockCols, 1));
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
    return m_rowSliceLength;
  }

  /**
   * Returns the number of columns in the matrix.
   *
   * @return The number of columns in the matrix.
   */
  public int cols() {
    return m_colSliceLength;
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
   * Returns an element of the variable block.
   *
   * @param index The index of the element to return.
   * @return An element of the variable block.
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
    return rows() * cols();
  }

  @Override
  public Iterator<Variable> iterator() {
    return new Iterator<>() {
      private int m_index = 0;

      @Override
      public boolean hasNext() {
        return m_index < VariableBlock.this.size();
      }

      @Override
      public Variable next() {
        if (!hasNext()) {
          throw new NoSuchElementException();
        }

        return VariableBlock.this.get(m_index++);
      }
    };
  }

  /**
   * Creates a Stream of VariableBlock elements.
   *
   * @return A Stream of VariableBlock elements.
   */
  public Stream<Variable> stream() {
    return StreamSupport.stream(spliterator(), false);
  }
}
