// Copyright (c) Sleipnir contributors

#pragma once

#include <algorithm>
#include <concepts>
#include <initializer_list>
#include <iterator>
#include <span>
#include <utility>
#include <vector>

#include <Eigen/Core>

#include "sleipnir/autodiff/Variable.hpp"
#include "sleipnir/autodiff/VariableBlock.hpp"
#include "sleipnir/util/Assert.hpp"
#include "sleipnir/util/FunctionRef.hpp"
#include "sleipnir/util/SymbolExports.hpp"

namespace sleipnir {

/**
 * A matrix of autodiff variables.
 */
class SLEIPNIR_DLLEXPORT VariableMatrix {
 public:
  /**
   * Constructs an empty VariableMatrix.
   */
  VariableMatrix() = default;

  /**
   * Constructs a VariableMatrix column vector with the given rows.
   *
   * @param rows The number of matrix rows.
   */
  explicit VariableMatrix(int rows) : m_rows{rows}, m_cols{1} {
    for (int row = 0; row < rows; ++row) {
      m_storage.emplace_back();
    }
  }

  /**
   * Constructs a VariableMatrix with the given dimensions.
   *
   * @param rows The number of matrix rows.
   * @param cols The number of matrix columns.
   */
  VariableMatrix(int rows, int cols) : m_rows{rows}, m_cols{cols} {
    for (int row = 0; row < rows; ++row) {
      for (int col = 0; col < cols; ++col) {
        m_storage.emplace_back();
      }
    }
  }

  /**
   * Constructs a scalar VariableMatrix from a nested list of Variables.
   *
   * @param list The nested list of Variables.
   */
  VariableMatrix(  // NOLINT
      std::initializer_list<std::initializer_list<Variable>> list) {
    // Get row and column counts for destination matrix
    m_rows = list.size();
    m_cols = 0;
    if (list.size() > 0) {
      m_cols = list.begin()->size();
    }

    // Assert the first and latest column counts are the same
    for ([[maybe_unused]]
         const auto& row : list) {
      Assert(list.begin()->size() == row.size());
    }

    m_storage.reserve(Rows() * Cols());
    for (const auto& row : list) {
      std::copy(row.begin(), row.end(), std::back_inserter(m_storage));
    }
  }

  /**
   * Constructs a scalar VariableMatrix from a nested list of doubles.
   *
   * This overload is for Python bindings only.
   *
   * @param list The nested list of Variables.
   */
  VariableMatrix(std::vector<std::vector<double>> list) {  // NOLINT
    // Get row and column counts for destination matrix
    m_rows = list.size();
    m_cols = 0;
    if (list.size() > 0) {
      m_cols = list.begin()->size();
    }

    // Assert the first and latest column counts are the same
    for ([[maybe_unused]]
         const auto& row : list) {
      Assert(list.begin()->size() == row.size());
    }

    m_storage.reserve(Rows() * Cols());
    for (const auto& row : list) {
      std::copy(row.begin(), row.end(), std::back_inserter(m_storage));
    }
  }

  /**
   * Constructs a scalar VariableMatrix from a nested list of Variables.
   *
   * This overload is for Python bindings only.
   *
   * @param list The nested list of Variables.
   */
  VariableMatrix(std::vector<std::vector<Variable>> list) {  // NOLINT
    // Get row and column counts for destination matrix
    m_rows = list.size();
    m_cols = 0;
    if (list.size() > 0) {
      m_cols = list.begin()->size();
    }

    // Assert the first and latest column counts are the same
    for ([[maybe_unused]]
         const auto& row : list) {
      Assert(list.begin()->size() == row.size());
    }

    m_storage.reserve(Rows() * Cols());
    for (const auto& row : list) {
      std::copy(row.begin(), row.end(), std::back_inserter(m_storage));
    }
  }

  /**
   * Constructs a VariableMatrix from an Eigen matrix.
   *
   * @param values Eigen matrix of values.
   */
  template <typename Derived>
  VariableMatrix(const Eigen::MatrixBase<Derived>& values)  // NOLINT
      : m_rows{static_cast<int>(values.rows())},
        m_cols{static_cast<int>(values.cols())} {
    m_storage.reserve(values.rows() * values.cols());
    for (int row = 0; row < values.rows(); ++row) {
      for (int col = 0; col < values.cols(); ++col) {
        m_storage.emplace_back(values(row, col));
      }
    }
  }

  /**
   * Constructs a VariableMatrix from an Eigen diagonal matrix.
   *
   * @param values Diagonal matrix of values.
   */
  template <typename Derived>
  VariableMatrix(const Eigen::DiagonalBase<Derived>& values)  // NOLINT
      : m_rows{static_cast<int>(values.rows())},
        m_cols{static_cast<int>(values.cols())} {
    m_storage.reserve(values.rows() * values.cols());
    for (int row = 0; row < values.rows(); ++row) {
      for (int col = 0; col < values.cols(); ++col) {
        if (row == col) {
          m_storage.emplace_back(values.diagonal()(row));
        } else {
          m_storage.emplace_back(0.0);
        }
      }
    }
  }

  /**
   * Assigns an Eigen matrix to a VariableMatrix.
   *
   * @param values Eigen matrix of values.
   */
  template <typename Derived>
  VariableMatrix& operator=(const Eigen::MatrixBase<Derived>& values) {
    Assert(Rows() == values.rows());
    Assert(Cols() == values.cols());

    for (int row = 0; row < values.rows(); ++row) {
      for (int col = 0; col < values.cols(); ++col) {
        (*this)(row, col) = values(row, col);
      }
    }

    return *this;
  }

  /**
   * Sets the VariableMatrix's internal values.
   *
   * @param values Eigen matrix of values.
   */
  template <typename Derived>
    requires std::same_as<typename Derived::Scalar, double>
  VariableMatrix& SetValue(const Eigen::MatrixBase<Derived>& values) {
    Assert(Rows() == values.rows());
    Assert(Cols() == values.cols());

    for (int row = 0; row < values.rows(); ++row) {
      for (int col = 0; col < values.cols(); ++col) {
        (*this)(row, col).SetValue(values(row, col));
      }
    }

    return *this;
  }

  /**
   * Constructs a scalar VariableMatrix from a Variable.
   *
   * @param variable Variable.
   */
  VariableMatrix(const Variable& variable)  // NOLINT
      : m_rows{1}, m_cols{1} {
    m_storage.emplace_back(variable);
  }

  /**
   * Constructs a scalar VariableMatrix from a Variable.
   *
   * @param variable Variable.
   */
  VariableMatrix(Variable&& variable) : m_rows{1}, m_cols{1} {  // NOLINT
    m_storage.emplace_back(std::move(variable));
  }

  /**
   * Constructs a VariableMatrix from a VariableBlock.
   *
   * @param values VariableBlock of values.
   */
  VariableMatrix(const VariableBlock<VariableMatrix>& values)  // NOLINT
      : m_rows{values.Rows()}, m_cols{values.Cols()} {
    for (int row = 0; row < Rows(); ++row) {
      for (int col = 0; col < Cols(); ++col) {
        m_storage.emplace_back(values(row, col));
      }
    }
  }

  /**
   * Constructs a VariableMatrix from a VariableBlock.
   *
   * @param values VariableBlock of values.
   */
  VariableMatrix(const VariableBlock<const VariableMatrix>& values)  // NOLINT
      : m_rows{values.Rows()}, m_cols{values.Cols()} {
    for (int row = 0; row < Rows(); ++row) {
      for (int col = 0; col < Cols(); ++col) {
        m_storage.emplace_back(values(row, col));
      }
    }
  }

  /**
   * Constructs a column vector wrapper around a Variable array.
   *
   * @param values Variable array to wrap.
   */
  explicit VariableMatrix(std::span<const Variable> values)
      : m_rows{static_cast<int>(values.size())}, m_cols{1} {
    for (int row = 0; row < Rows(); ++row) {
      for (int col = 0; col < Cols(); ++col) {
        m_storage.emplace_back(values[row * Cols() + col]);
      }
    }
  }

  /**
   * Constructs a matrix wrapper around a Variable array.
   *
   * @param values Variable array to wrap.
   * @param rows The number of matrix rows.
   * @param cols The number of matrix columns.
   */
  VariableMatrix(std::span<const Variable> values, int rows, int cols)
      : m_rows{rows}, m_cols{cols} {
    Assert(static_cast<int>(values.size()) == Rows() * Cols());
    for (int row = 0; row < Rows(); ++row) {
      for (int col = 0; col < Cols(); ++col) {
        m_storage.emplace_back(values[row * Cols() + col]);
      }
    }
  }

  /**
   * Returns a block pointing to the given row and column.
   *
   * @param row The block row.
   * @param col The block column.
   */
  Variable& operator()(int row, int col) {
    Assert(row >= 0 && row < Rows());
    Assert(col >= 0 && col < Cols());
    return m_storage[row * Cols() + col];
  }

  /**
   * Returns a block pointing to the given row and column.
   *
   * @param row The block row.
   * @param col The block column.
   */
  const Variable& operator()(int row, int col) const {
    Assert(row >= 0 && row < Rows());
    Assert(col >= 0 && col < Cols());
    return m_storage[row * Cols() + col];
  }

  /**
   * Returns a block pointing to the given row.
   *
   * @param row The block row.
   */
  Variable& operator()(int row) {
    Assert(row >= 0 && row < Rows() * Cols());
    return m_storage[row];
  }

  /**
   * Returns a block pointing to the given row.
   *
   * @param row The block row.
   */
  const Variable& operator()(int row) const {
    Assert(row >= 0 && row < Rows() * Cols());
    return m_storage[row];
  }

  /**
   * Returns a block slice of the variable matrix.
   *
   * @param rowOffset The row offset of the block selection.
   * @param colOffset The column offset of the block selection.
   * @param blockRows The number of rows in the block selection.
   * @param blockCols The number of columns in the block selection.
   */
  VariableBlock<VariableMatrix> Block(int rowOffset, int colOffset,
                                      int blockRows, int blockCols) {
    Assert(rowOffset >= 0 && rowOffset <= Rows());
    Assert(colOffset >= 0 && colOffset <= Cols());
    Assert(blockRows >= 0 && blockRows <= Rows() - rowOffset);
    Assert(blockCols >= 0 && blockCols <= Cols() - colOffset);
    return VariableBlock{*this, rowOffset, colOffset, blockRows, blockCols};
  }

  /**
   * Returns a block slice of the variable matrix.
   *
   * @param rowOffset The row offset of the block selection.
   * @param colOffset The column offset of the block selection.
   * @param blockRows The number of rows in the block selection.
   * @param blockCols The number of columns in the block selection.
   */
  const VariableBlock<const VariableMatrix> Block(int rowOffset, int colOffset,
                                                  int blockRows,
                                                  int blockCols) const {
    Assert(rowOffset >= 0 && rowOffset <= Rows());
    Assert(colOffset >= 0 && colOffset <= Cols());
    Assert(blockRows >= 0 && blockRows <= Rows() - rowOffset);
    Assert(blockCols >= 0 && blockCols <= Cols() - colOffset);
    return VariableBlock{*this, rowOffset, colOffset, blockRows, blockCols};
  }

  /**
   * Returns a segment of the variable vector.
   *
   * @param offset The offset of the segment.
   * @param length The length of the segment.
   */
  VariableBlock<VariableMatrix> Segment(int offset, int length) {
    Assert(offset >= 0 && offset < Rows() * Cols());
    Assert(length >= 0 && length <= Rows() * Cols() - offset);
    return Block(offset, 0, length, 1);
  }

  /**
   * Returns a segment of the variable vector.
   *
   * @param offset The offset of the segment.
   * @param length The length of the segment.
   */
  const VariableBlock<const VariableMatrix> Segment(int offset,
                                                    int length) const {
    Assert(offset >= 0 && offset < Rows() * Cols());
    Assert(length >= 0 && length <= Rows() * Cols() - offset);
    return Block(offset, 0, length, 1);
  }

  /**
   * Returns a row slice of the variable matrix.
   *
   * @param row The row to slice.
   */
  VariableBlock<VariableMatrix> Row(int row) {
    Assert(row >= 0 && row < Rows());
    return Block(row, 0, 1, Cols());
  }

  /**
   * Returns a row slice of the variable matrix.
   *
   * @param row The row to slice.
   */
  const VariableBlock<const VariableMatrix> Row(int row) const {
    Assert(row >= 0 && row < Rows());
    return Block(row, 0, 1, Cols());
  }

  /**
   * Returns a column slice of the variable matrix.
   *
   * @param col The column to slice.
   */
  VariableBlock<VariableMatrix> Col(int col) {
    Assert(col >= 0 && col < Cols());
    return Block(0, col, Rows(), 1);
  }

  /**
   * Returns a column slice of the variable matrix.
   *
   * @param col The column to slice.
   */
  const VariableBlock<const VariableMatrix> Col(int col) const {
    Assert(col >= 0 && col < Cols());
    return Block(0, col, Rows(), 1);
  }

  /**
   * Matrix multiplication operator.
   *
   * @param lhs Operator left-hand side.
   * @param rhs Operator right-hand side.
   */
  friend SLEIPNIR_DLLEXPORT VariableMatrix
  operator*(const VariableMatrix& lhs, const VariableMatrix& rhs) {
    Assert(lhs.Cols() == rhs.Rows());

    VariableMatrix result{lhs.Rows(), rhs.Cols()};

    for (int i = 0; i < lhs.Rows(); ++i) {
      for (int j = 0; j < rhs.Cols(); ++j) {
        Variable sum;
        for (int k = 0; k < lhs.Cols(); ++k) {
          sum += lhs(i, k) * rhs(k, j);
        }
        result(i, j) = sum;
      }
    }

    return result;
  }

  /**
   * Matrix-scalar multiplication operator.
   *
   * @param lhs Operator left-hand side.
   * @param rhs Operator right-hand side.
   */
  friend SLEIPNIR_DLLEXPORT VariableMatrix operator*(const VariableMatrix& lhs,
                                                     const Variable& rhs) {
    VariableMatrix result{lhs.Rows(), lhs.Cols()};

    for (int row = 0; row < result.Rows(); ++row) {
      for (int col = 0; col < result.Cols(); ++col) {
        result(row, col) = lhs(row, col) * rhs;
      }
    }

    return result;
  }

  /**
   * Matrix-scalar multiplication operator.
   *
   * @param lhs Operator left-hand side.
   * @param rhs Operator right-hand side.
   */
  friend SLEIPNIR_DLLEXPORT VariableMatrix operator*(const VariableMatrix& lhs,
                                                     double rhs) {
    return lhs * Variable{rhs};
  }

  /**
   * Scalar-matrix multiplication operator.
   *
   * @param lhs Operator left-hand side.
   * @param rhs Operator right-hand side.
   */
  friend SLEIPNIR_DLLEXPORT VariableMatrix
  operator*(const Variable& lhs, const VariableMatrix& rhs) {
    VariableMatrix result{rhs.Rows(), rhs.Cols()};

    for (int row = 0; row < result.Rows(); ++row) {
      for (int col = 0; col < result.Cols(); ++col) {
        result(row, col) = rhs(row, col) * lhs;
      }
    }

    return result;
  }

  /**
   * Scalar-matrix multiplication operator.
   *
   * @param lhs Operator left-hand side.
   * @param rhs Operator right-hand side.
   */
  friend SLEIPNIR_DLLEXPORT VariableMatrix
  operator*(double lhs, const VariableMatrix& rhs) {
    return Variable{lhs} * rhs;
  }

  /**
   * Compound matrix multiplication-assignment operator.
   *
   * @param rhs Variable to multiply.
   */
  VariableMatrix& operator*=(const VariableMatrix& rhs) {
    Assert(Cols() == rhs.Rows() && Cols() == rhs.Cols());

    for (int i = 0; i < Rows(); ++i) {
      for (int j = 0; j < rhs.Cols(); ++j) {
        Variable sum;
        for (int k = 0; k < Cols(); ++k) {
          sum += (*this)(i, k) * rhs(k, j);
        }
        (*this)(i, j) = sum;
      }
    }

    return *this;
  }

  /**
   * Binary division operator (only enabled when rhs is a scalar).
   *
   * @param lhs Operator left-hand side.
   * @param rhs Operator right-hand side.
   */
  friend SLEIPNIR_DLLEXPORT VariableMatrix operator/(const VariableMatrix& lhs,
                                                     const Variable& rhs) {
    VariableMatrix result{lhs.Rows(), lhs.Cols()};

    for (int row = 0; row < result.Rows(); ++row) {
      for (int col = 0; col < result.Cols(); ++col) {
        result(row, col) = lhs(row, col) / rhs;
      }
    }

    return result;
  }

  /**
   * Compound matrix division-assignment operator (only enabled when rhs
   * is a scalar).
   *
   * @param rhs Variable to divide.
   */
  VariableMatrix& operator/=(const Variable& rhs) {
    for (int row = 0; row < Rows(); ++row) {
      for (int col = 0; col < Cols(); ++col) {
        (*this)(row, col) /= rhs;
      }
    }

    return *this;
  }

  /**
   * Binary addition operator.
   *
   * @param lhs Operator left-hand side.
   * @param rhs Operator right-hand side.
   */
  friend SLEIPNIR_DLLEXPORT VariableMatrix
  operator+(const VariableMatrix& lhs, const VariableMatrix& rhs) {
    VariableMatrix result{lhs.Rows(), lhs.Cols()};

    for (int row = 0; row < result.Rows(); ++row) {
      for (int col = 0; col < result.Cols(); ++col) {
        result(row, col) = lhs(row, col) + rhs(row, col);
      }
    }

    return result;
  }

  /**
   * Compound addition-assignment operator.
   *
   * @param rhs Variable to add.
   */
  VariableMatrix& operator+=(const VariableMatrix& rhs) {
    for (int row = 0; row < Rows(); ++row) {
      for (int col = 0; col < Cols(); ++col) {
        (*this)(row, col) += rhs(row, col);
      }
    }

    return *this;
  }

  /**
   * Binary subtraction operator.
   *
   * @param lhs Operator left-hand side.
   * @param rhs Operator right-hand side.
   */
  friend SLEIPNIR_DLLEXPORT VariableMatrix
  operator-(const VariableMatrix& lhs, const VariableMatrix& rhs) {
    VariableMatrix result{lhs.Rows(), lhs.Cols()};

    for (int row = 0; row < result.Rows(); ++row) {
      for (int col = 0; col < result.Cols(); ++col) {
        result(row, col) = lhs(row, col) - rhs(row, col);
      }
    }

    return result;
  }

  /**
   * Compound subtraction-assignment operator.
   *
   * @param rhs Variable to subtract.
   */
  VariableMatrix& operator-=(const VariableMatrix& rhs) {
    for (int row = 0; row < Rows(); ++row) {
      for (int col = 0; col < Cols(); ++col) {
        (*this)(row, col) -= rhs(row, col);
      }
    }

    return *this;
  }

  /**
   * Unary minus operator.
   *
   * @param lhs Operand for unary minus.
   */
  friend SLEIPNIR_DLLEXPORT VariableMatrix
  operator-(const VariableMatrix& lhs) {
    VariableMatrix result{lhs.Rows(), lhs.Cols()};

    for (int row = 0; row < result.Rows(); ++row) {
      for (int col = 0; col < result.Cols(); ++col) {
        result(row, col) = -lhs(row, col);
      }
    }

    return result;
  }

  /**
   * Implicit conversion operator from 1x1 VariableMatrix to Variable.
   */
  operator Variable() const {  // NOLINT
    Assert(Rows() == 1 && Cols() == 1);
    return (*this)(0, 0);
  }

  /**
   * Returns the transpose of the variable matrix.
   */
  VariableMatrix T() const {
    VariableMatrix result{Cols(), Rows()};

    for (int row = 0; row < Rows(); ++row) {
      for (int col = 0; col < Cols(); ++col) {
        result(col, row) = (*this)(row, col);
      }
    }

    return result;
  }

  /**
   * Returns number of rows in the matrix.
   */
  int Rows() const { return m_rows; }

  /**
   * Returns number of columns in the matrix.
   */
  int Cols() const { return m_cols; }

  /**
   * Returns an element of the variable matrix.
   *
   * @param row The row of the element to return.
   * @param col The column of the element to return.
   */
  double Value(int row, int col) const {
    Assert(row >= 0 && row < Rows());
    Assert(col >= 0 && col < Cols());
    return m_storage[row * Cols() + col].Value();
  }

  /**
   * Returns a row of the variable column vector.
   *
   * @param index The index of the element to return.
   */
  double Value(int index) const {
    Assert(index >= 0 && index < Rows() * Cols());
    return m_storage[index].Value();
  }

  /**
   * Returns the contents of the variable matrix.
   */
  Eigen::MatrixXd Value() const {
    Eigen::MatrixXd result{Rows(), Cols()};

    for (int row = 0; row < Rows(); ++row) {
      for (int col = 0; col < Cols(); ++col) {
        result(row, col) = Value(row, col);
      }
    }

    return result;
  }

  /**
   * Transforms the matrix coefficient-wise with an unary operator.
   *
   * @param unaryOp The unary operator to use for the transform operation.
   */
  VariableMatrix CwiseTransform(
      function_ref<Variable(const Variable&)> unaryOp) const {
    VariableMatrix result{Rows(), Cols()};

    for (int row = 0; row < Rows(); ++row) {
      for (int col = 0; col < Cols(); ++col) {
        result(row, col) = unaryOp((*this)(row, col));
      }
    }

    return result;
  }

  class iterator {
   public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = Variable;
    using difference_type = std::ptrdiff_t;
    using pointer = Variable*;
    using reference = Variable&;

    iterator(VariableMatrix* mat, int row, int col)
        : m_mat{mat}, m_row{row}, m_col{col} {}

    iterator& operator++() {
      ++m_col;
      if (m_col == m_mat->Cols()) {
        m_col = 0;
        ++m_row;
      }
      return *this;
    }
    iterator operator++(int) {
      iterator retval = *this;
      ++(*this);
      return retval;
    }
    bool operator==(const iterator&) const = default;
    reference operator*() { return (*m_mat)(m_row, m_col); }

   private:
    VariableMatrix* m_mat;
    int m_row;
    int m_col;
  };

  class const_iterator {
   public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = Variable;
    using difference_type = std::ptrdiff_t;
    using pointer = Variable*;
    using const_reference = const Variable&;

    const_iterator(const VariableMatrix* mat, int row, int col)
        : m_mat{mat}, m_row{row}, m_col{col} {}

    const_iterator& operator++() {
      ++m_col;
      if (m_col == m_mat->Cols()) {
        m_col = 0;
        ++m_row;
      }
      return *this;
    }
    const_iterator operator++(int) {
      const_iterator retval = *this;
      ++(*this);
      return retval;
    }
    bool operator==(const const_iterator&) const = default;
    const_reference operator*() const { return (*m_mat)(m_row, m_col); }

   private:
    const VariableMatrix* m_mat;
    int m_row;
    int m_col;
  };

  /**
   * Returns begin iterator.
   */
  iterator begin() { return iterator(this, 0, 0); }

  /**
   * Returns end iterator.
   */
  iterator end() { return iterator(this, Rows(), 0); }

  /**
   * Returns begin iterator.
   */
  const_iterator begin() const { return const_iterator(this, 0, 0); }

  /**
   * Returns end iterator.
   */
  const_iterator end() const { return const_iterator(this, Rows(), 0); }

  /**
   * Returns begin iterator.
   */
  const_iterator cbegin() const { return const_iterator(this, 0, 0); }

  /**
   * Returns end iterator.
   */
  const_iterator cend() const { return const_iterator(this, Rows(), 0); }

  /**
   * Returns number of elements in matrix.
   */
  size_t size() const { return m_rows * m_cols; }

  /**
   * Returns a variable matrix filled with zeroes.
   *
   * @param rows The number of matrix rows.
   * @param cols The number of matrix columns.
   */
  static VariableMatrix Zero(int rows, int cols) {
    VariableMatrix result{rows, cols};

    for (auto& elem : result) {
      elem = 0.0;
    }

    return result;
  }

  /**
   * Returns a variable matrix filled with ones.
   *
   * @param rows The number of matrix rows.
   * @param cols The number of matrix columns.
   */
  static VariableMatrix Ones(int rows, int cols) {
    VariableMatrix result{rows, cols};

    for (auto& elem : result) {
      elem = 1.0;
    }

    return result;
  }

 private:
  std::vector<Variable> m_storage;
  int m_rows = 0;
  int m_cols = 0;
};

/**
 * Applies a coefficient-wise reduce operation to two matrices.
 *
 * @param lhs The left-hand side of the binary operator.
 * @param rhs The right-hand side of the binary operator.
 * @param binaryOp The binary operator to use for the reduce operation.
 */
SLEIPNIR_DLLEXPORT inline VariableMatrix CwiseReduce(
    const VariableMatrix& lhs, const VariableMatrix& rhs,
    function_ref<Variable(const Variable&, const Variable&)> binaryOp) {
  Assert(lhs.Rows() == rhs.Rows());
  Assert(lhs.Rows() == rhs.Rows());

  VariableMatrix result{lhs.Rows(), lhs.Cols()};

  for (int row = 0; row < lhs.Rows(); ++row) {
    for (int col = 0; col < lhs.Cols(); ++col) {
      result(row, col) = binaryOp(lhs(row, col), rhs(row, col));
    }
  }

  return result;
}

/**
 * Assemble a VariableMatrix from a nested list of blocks.
 *
 * Each row's blocks must have the same height, and the assembled block rows
 * must have the same width. For example, for the block matrix [[A, B], [C]] to
 * be constructible, the number of rows in A and B must match, and the number of
 * columns in [A, B] and [C] must match.
 *
 * @param list The nested list of blocks.
 */
SLEIPNIR_DLLEXPORT inline VariableMatrix Block(
    std::initializer_list<std::initializer_list<VariableMatrix>> list) {
  // Get row and column counts for destination matrix
  int rows = 0;
  int cols = -1;
  for (const auto& row : list) {
    if (row.size() > 0) {
      rows += row.begin()->Rows();
    }

    // Get number of columns in this row
    int latestCols = 0;
    for (const auto& elem : row) {
      // Assert the first and latest row have the same height
      Assert(row.begin()->Rows() == elem.Rows());

      latestCols += elem.Cols();
    }

    // If this is the first row, record the column count. Otherwise, assert the
    // first and latest column counts are the same.
    if (cols == -1) {
      cols = latestCols;
    } else {
      Assert(cols == latestCols);
    }
  }

  VariableMatrix result{rows, cols};

  int rowOffset = 0;
  for (const auto& row : list) {
    int colOffset = 0;
    for (const auto& elem : row) {
      result.Block(rowOffset, colOffset, elem.Rows(), elem.Cols()) = elem;
      colOffset += elem.Cols();
    }
    rowOffset += row.begin()->Rows();
  }

  return result;
}

/**
 * Assemble a VariableMatrix from a nested list of blocks.
 *
 * Each row's blocks must have the same height, and the assembled block rows
 * must have the same width. For example, for the block matrix [[A, B], [C]] to
 * be constructible, the number of rows in A and B must match, and the number of
 * columns in [A, B] and [C] must match.
 *
 * This overload is for Python bindings only.
 *
 * @param list The nested list of blocks.
 */
SLEIPNIR_DLLEXPORT inline VariableMatrix Block(
    std::vector<std::vector<VariableMatrix>> list) {
  // Get row and column counts for destination matrix
  int rows = 0;
  int cols = -1;
  for (const auto& row : list) {
    if (row.size() > 0) {
      rows += row.begin()->Rows();
    }

    // Get number of columns in this row
    int latestCols = 0;
    for (const auto& elem : row) {
      // Assert the first and latest row have the same height
      Assert(row.begin()->Rows() == elem.Rows());

      latestCols += elem.Cols();
    }

    // If this is the first row, record the column count. Otherwise, assert the
    // first and latest column counts are the same.
    if (cols == -1) {
      cols = latestCols;
    } else {
      Assert(cols == latestCols);
    }
  }

  VariableMatrix result{rows, cols};

  int rowOffset = 0;
  for (const auto& row : list) {
    int colOffset = 0;
    for (const auto& elem : row) {
      result.Block(rowOffset, colOffset, elem.Rows(), elem.Cols()) = elem;
      colOffset += elem.Cols();
    }
    rowOffset += row.begin()->Rows();
  }

  return result;
}

}  // namespace sleipnir
