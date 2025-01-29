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
#include <gch/small_vector.hpp>

#include "sleipnir/autodiff/slice.hpp"
#include "sleipnir/autodiff/variable.hpp"
#include "sleipnir/autodiff/variable_block.hpp"
#include "sleipnir/util/assert.hpp"
#include "sleipnir/util/concepts.hpp"
#include "sleipnir/util/function_ref.hpp"
#include "sleipnir/util/symbol_exports.hpp"

namespace slp {

/**
 * A matrix of autodiff variables.
 */
class SLEIPNIR_DLLEXPORT VariableMatrix {
 public:
  /**
   * Type tag used to designate an uninitialized VariableMatrix.
   */
  struct empty_t {};

  /**
   * Designates an uninitialized VariableMatrix.
   */
  static constexpr empty_t empty{};

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
    m_storage.reserve(rows);
    for (int row = 0; row < rows; ++row) {
      m_storage.emplace_back();
    }
  }

  /**
   * Constructs a zero-initialized VariableMatrix with the given dimensions.
   *
   * @param rows The number of matrix rows.
   * @param cols The number of matrix columns.
   */
  VariableMatrix(int rows, int cols) : m_rows{rows}, m_cols{cols} {
    m_storage.reserve(rows * cols);
    for (int index = 0; index < rows * cols; ++index) {
      m_storage.emplace_back();
    }
  }

  /**
   * Constructs an empty VariableMatrix with the given dimensions.
   *
   * @param rows The number of matrix rows.
   * @param cols The number of matrix columns.
   */
  VariableMatrix(empty_t, int rows, int cols) : m_rows{rows}, m_cols{cols} {
    m_storage.reserve(rows * cols);
    for (int index = 0; index < rows * cols; ++index) {
      m_storage.emplace_back(nullptr);
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
      slp_assert(list.begin()->size() == row.size());
    }

    m_storage.reserve(rows() * cols());
    for (const auto& row : list) {
      std::ranges::copy(row, std::back_inserter(m_storage));
    }
  }

  /**
   * Constructs a scalar VariableMatrix from a nested list of doubles.
   *
   * This overload is for Python bindings only.
   *
   * @param list The nested list of Variables.
   */
  VariableMatrix(const std::vector<std::vector<double>>& list) {  // NOLINT
    // Get row and column counts for destination matrix
    m_rows = list.size();
    m_cols = 0;
    if (list.size() > 0) {
      m_cols = list.begin()->size();
    }

    // Assert the first and latest column counts are the same
    for ([[maybe_unused]]
         const auto& row : list) {
      slp_assert(list.begin()->size() == row.size());
    }

    m_storage.reserve(rows() * cols());
    for (const auto& row : list) {
      std::ranges::copy(row, std::back_inserter(m_storage));
    }
  }

  /**
   * Constructs a scalar VariableMatrix from a nested list of Variables.
   *
   * This overload is for Python bindings only.
   *
   * @param list The nested list of Variables.
   */
  VariableMatrix(const std::vector<std::vector<Variable>>& list) {  // NOLINT
    // Get row and column counts for destination matrix
    m_rows = list.size();
    m_cols = 0;
    if (list.size() > 0) {
      m_cols = list.begin()->size();
    }

    // Assert the first and latest column counts are the same
    for ([[maybe_unused]]
         const auto& row : list) {
      slp_assert(list.begin()->size() == row.size());
    }

    m_storage.reserve(rows() * cols());
    for (const auto& row : list) {
      std::ranges::copy(row, std::back_inserter(m_storage));
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
          m_storage.emplace_back(values.diagonal()[row]);
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
   * @return This VariableMatrix.
   */
  template <typename Derived>
  VariableMatrix& operator=(const Eigen::MatrixBase<Derived>& values) {
    slp_assert(rows() == values.rows() && cols() == values.cols());

    for (int row = 0; row < values.rows(); ++row) {
      for (int col = 0; col < values.cols(); ++col) {
        (*this)(row, col) = values(row, col);
      }
    }

    return *this;
  }

  /**
   * Assigns a double to the matrix.
   *
   * This only works for matrices with one row and one column.
   *
   * @param value Value to assign.
   * @return This VariableMatrix.
   */
  VariableMatrix& operator=(ScalarLike auto value) {
    slp_assert(rows() == 1 && cols() == 1);

    (*this)(0, 0) = value;

    return *this;
  }

  /**
   * Sets the VariableMatrix's internal values.
   *
   * @param values Eigen matrix of values.
   */
  template <typename Derived>
    requires std::same_as<typename Derived::Scalar, double>
  void set_value(const Eigen::MatrixBase<Derived>& values) {
    slp_assert(rows() == values.rows() && cols() == values.cols());

    for (int row = 0; row < values.rows(); ++row) {
      for (int col = 0; col < values.cols(); ++col) {
        (*this)(row, col).set_value(values(row, col));
      }
    }
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
      : m_rows{values.rows()}, m_cols{values.cols()} {
    m_storage.reserve(rows() * cols());
    for (int row = 0; row < rows(); ++row) {
      for (int col = 0; col < cols(); ++col) {
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
      : m_rows{values.rows()}, m_cols{values.cols()} {
    m_storage.reserve(rows() * cols());
    for (int row = 0; row < rows(); ++row) {
      for (int col = 0; col < cols(); ++col) {
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
    m_storage.reserve(rows() * cols());
    for (int row = 0; row < rows(); ++row) {
      for (int col = 0; col < cols(); ++col) {
        m_storage.emplace_back(values[row * cols() + col]);
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
    slp_assert(static_cast<int>(values.size()) == rows * cols);
    m_storage.reserve(rows * cols);
    for (int row = 0; row < rows; ++row) {
      for (int col = 0; col < cols; ++col) {
        m_storage.emplace_back(values[row * cols + col]);
      }
    }
  }

  /**
   * Returns a block pointing to the given row and column.
   *
   * @param row The block row.
   * @param col The block column.
   * @return A block pointing to the given row and column.
   */
  Variable& operator()(int row, int col) {
    slp_assert(row >= 0 && row < rows());
    slp_assert(col >= 0 && col < cols());
    return m_storage[row * cols() + col];
  }

  /**
   * Returns a block pointing to the given row and column.
   *
   * @param row The block row.
   * @param col The block column.
   * @return A block pointing to the given row and column.
   */
  const Variable& operator()(int row, int col) const {
    slp_assert(row >= 0 && row < rows());
    slp_assert(col >= 0 && col < cols());
    return m_storage[row * cols() + col];
  }

  /**
   * Returns a block pointing to the given row.
   *
   * @param row The block row.
   * @return A block pointing to the given row.
   */
  Variable& operator[](int row) {
    slp_assert(row >= 0 && row < rows() * cols());
    return m_storage[row];
  }

  /**
   * Returns a block pointing to the given row.
   *
   * @param row The block row.
   * @return A block pointing to the given row.
   */
  const Variable& operator[](int row) const {
    slp_assert(row >= 0 && row < rows() * cols());
    return m_storage[row];
  }

  /**
   * Returns a block of the variable matrix.
   *
   * @param row_offset The row offset of the block selection.
   * @param col_offset The column offset of the block selection.
   * @param block_rows The number of rows in the block selection.
   * @param block_cols The number of columns in the block selection.
   * @return A block of the variable matrix.
   */
  VariableBlock<VariableMatrix> block(int row_offset, int col_offset,
                                      int block_rows, int block_cols) {
    slp_assert(row_offset >= 0 && row_offset <= rows());
    slp_assert(col_offset >= 0 && col_offset <= cols());
    slp_assert(block_rows >= 0 && block_rows <= rows() - row_offset);
    slp_assert(block_cols >= 0 && block_cols <= cols() - col_offset);
    return VariableBlock{*this, row_offset, col_offset, block_rows, block_cols};
  }

  /**
   * Returns a block of the variable matrix.
   *
   * @param row_offset The row offset of the block selection.
   * @param col_offset The column offset of the block selection.
   * @param block_rows The number of rows in the block selection.
   * @param block_cols The number of columns in the block selection.
   * @return A block of the variable matrix.
   */
  const VariableBlock<const VariableMatrix> block(int row_offset,
                                                  int col_offset,
                                                  int block_rows,
                                                  int block_cols) const {
    slp_assert(row_offset >= 0 && row_offset <= rows());
    slp_assert(col_offset >= 0 && col_offset <= cols());
    slp_assert(block_rows >= 0 && block_rows <= rows() - row_offset);
    slp_assert(block_cols >= 0 && block_cols <= cols() - col_offset);
    return VariableBlock{*this, row_offset, col_offset, block_rows, block_cols};
  }

  /**
   * Returns a slice of the variable matrix.
   *
   * @param row_slice The row slice.
   * @param col_slice The column slice.
   * @return A slice of the variable matrix.
   */
  VariableBlock<VariableMatrix> operator()(Slice row_slice, Slice col_slice) {
    int row_slice_length = row_slice.adjust(rows());
    int col_slice_length = col_slice.adjust(cols());
    return VariableBlock{*this, std::move(row_slice), row_slice_length,
                         std::move(col_slice), col_slice_length};
  }

  /**
   * Returns a slice of the variable matrix.
   *
   * @param row_slice The row slice.
   * @param col_slice The column slice.
   * @return A slice of the variable matrix.
   */
  const VariableBlock<const VariableMatrix> operator()(Slice row_slice,
                                                       Slice col_slice) const {
    int row_slice_length = row_slice.adjust(rows());
    int col_slice_length = col_slice.adjust(cols());
    return VariableBlock{*this, std::move(row_slice), row_slice_length,
                         std::move(col_slice), col_slice_length};
  }

  /**
   * Returns a slice of the variable matrix.
   *
   * The given slices aren't adjusted. This overload is for Python bindings
   * only.
   *
   * @param row_slice The row slice.
   * @param row_slice_length The row slice length.
   * @param col_slice The column slice.
   * @param col_slice_length The column slice length.
   * @return A slice of the variable matrix.
   *
   */
  VariableBlock<VariableMatrix> operator()(Slice row_slice,
                                           int row_slice_length,
                                           Slice col_slice,
                                           int col_slice_length) {
    return VariableBlock{*this, std::move(row_slice), row_slice_length,
                         std::move(col_slice), col_slice_length};
  }

  /**
   * Returns a slice of the variable matrix.
   *
   * The given slices aren't adjusted. This overload is for Python bindings
   * only.
   *
   * @param row_slice The row slice.
   * @param row_slice_length The row slice length.
   * @param col_slice The column slice.
   * @param col_slice_length The column slice length.
   * @return A slice of the variable matrix.
   */
  const VariableBlock<const VariableMatrix> operator()(
      Slice row_slice, int row_slice_length, Slice col_slice,
      int col_slice_length) const {
    return VariableBlock{*this, std::move(row_slice), row_slice_length,
                         std::move(col_slice), col_slice_length};
  }

  /**
   * Returns a segment of the variable vector.
   *
   * @param offset The offset of the segment.
   * @param length The length of the segment.
   * @return A segment of the variable vector.
   */
  VariableBlock<VariableMatrix> segment(int offset, int length) {
    slp_assert(offset >= 0 && offset < rows() * cols());
    slp_assert(length >= 0 && length <= rows() * cols() - offset);
    return block(offset, 0, length, 1);
  }

  /**
   * Returns a segment of the variable vector.
   *
   * @param offset The offset of the segment.
   * @param length The length of the segment.
   * @return A segment of the variable vector.
   */
  const VariableBlock<const VariableMatrix> segment(int offset,
                                                    int length) const {
    slp_assert(offset >= 0 && offset < rows() * cols());
    slp_assert(length >= 0 && length <= rows() * cols() - offset);
    return block(offset, 0, length, 1);
  }

  /**
   * Returns a row slice of the variable matrix.
   *
   * @param row The row to slice.
   * @return A row slice of the variable matrix.
   */
  VariableBlock<VariableMatrix> row(int row) {
    slp_assert(row >= 0 && row < rows());
    return block(row, 0, 1, cols());
  }

  /**
   * Returns a row slice of the variable matrix.
   *
   * @param row The row to slice.
   * @return A row slice of the variable matrix.
   */
  const VariableBlock<const VariableMatrix> row(int row) const {
    slp_assert(row >= 0 && row < rows());
    return block(row, 0, 1, cols());
  }

  /**
   * Returns a column slice of the variable matrix.
   *
   * @param col The column to slice.
   * @return A column slice of the variable matrix.
   */
  VariableBlock<VariableMatrix> col(int col) {
    slp_assert(col >= 0 && col < cols());
    return block(0, col, rows(), 1);
  }

  /**
   * Returns a column slice of the variable matrix.
   *
   * @param col The column to slice.
   * @return A column slice of the variable matrix.
   */
  const VariableBlock<const VariableMatrix> col(int col) const {
    slp_assert(col >= 0 && col < cols());
    return block(0, col, rows(), 1);
  }

  /**
   * Matrix multiplication operator.
   *
   * @param lhs Operator left-hand side.
   * @param rhs Operator right-hand side.
   */
  template <MatrixLike LHS, MatrixLike RHS>
    requires SleipnirMatrixLike<LHS> || SleipnirMatrixLike<RHS>
  friend SLEIPNIR_DLLEXPORT VariableMatrix operator*(const LHS& lhs,
                                                     const RHS& rhs) {
    slp_assert(lhs.cols() == rhs.rows());

    VariableMatrix result(VariableMatrix::empty, lhs.rows(), rhs.cols());

#if __GNUC__ >= 12
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif
    for (int i = 0; i < lhs.rows(); ++i) {
      for (int j = 0; j < rhs.cols(); ++j) {
        Variable sum;
        for (int k = 0; k < lhs.cols(); ++k) {
          sum += lhs(i, k) * rhs(k, j);
        }
        result(i, j) = sum;
      }
    }
#if __GNUC__ >= 12
#pragma GCC diagnostic pop
#endif

    return result;
  }

  /**
   * Matrix-scalar multiplication operator.
   *
   * @param lhs Operator left-hand side.
   * @param rhs Operator right-hand side.
   */
  friend SLEIPNIR_DLLEXPORT VariableMatrix
  operator*(const SleipnirMatrixLike auto& lhs, const ScalarLike auto& rhs) {
    VariableMatrix result{VariableMatrix::empty, lhs.rows(), lhs.cols()};

    for (int row = 0; row < result.rows(); ++row) {
      for (int col = 0; col < result.cols(); ++col) {
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
  friend SLEIPNIR_DLLEXPORT VariableMatrix operator*(const MatrixLike auto& lhs,
                                                     const Variable& rhs) {
    VariableMatrix result(VariableMatrix::empty, lhs.rows(), lhs.cols());

    for (int row = 0; row < result.rows(); ++row) {
      for (int col = 0; col < result.cols(); ++col) {
        result(row, col) = lhs(row, col) * rhs;
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
  operator*(const ScalarLike auto& lhs, const SleipnirMatrixLike auto& rhs) {
    VariableMatrix result{VariableMatrix::empty, rhs.rows(), rhs.cols()};

    for (int row = 0; row < result.rows(); ++row) {
      for (int col = 0; col < result.cols(); ++col) {
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
  operator*(const Variable& lhs, const MatrixLike auto& rhs) {
    VariableMatrix result(VariableMatrix::empty, rhs.rows(), rhs.cols());

    for (int row = 0; row < result.rows(); ++row) {
      for (int col = 0; col < result.cols(); ++col) {
        result(row, col) = rhs(row, col) * lhs;
      }
    }

    return result;
  }

  /**
   * Compound matrix multiplication-assignment operator.
   *
   * @param rhs Variable to multiply.
   * @return Result of multiplication.
   */
  VariableMatrix& operator*=(const MatrixLike auto& rhs) {
    slp_assert(cols() == rhs.rows() && cols() == rhs.cols());

    for (int i = 0; i < rows(); ++i) {
      for (int j = 0; j < rhs.cols(); ++j) {
        Variable sum;
        for (int k = 0; k < cols(); ++k) {
          sum += (*this)(i, k) * rhs(k, j);
        }
        (*this)(i, j) = sum;
      }
    }

    return *this;
  }

  /**
   * Compound matrix-scalar multiplication-assignment operator.
   *
   * @param rhs Variable to multiply.
   * @return Result of multiplication.
   */
  VariableMatrix& operator*=(const ScalarLike auto& rhs) {
    for (int row = 0; row < rows(); ++row) {
      for (int col = 0; col < rhs.cols(); ++col) {
        (*this)(row, col) *= rhs;
      }
    }

    return *this;
  }

  /**
   * Binary division operator.
   *
   * @param lhs Operator left-hand side.
   * @param rhs Operator right-hand side.
   * @return Result of division.
   */
  friend SLEIPNIR_DLLEXPORT VariableMatrix
  operator/(const MatrixLike auto& lhs, const ScalarLike auto& rhs) {
    VariableMatrix result(VariableMatrix::empty, lhs.rows(), lhs.cols());

    for (int row = 0; row < result.rows(); ++row) {
      for (int col = 0; col < result.cols(); ++col) {
        result(row, col) = lhs(row, col) / rhs;
      }
    }

    return result;
  }

  /**
   * Compound matrix division-assignment operator.
   *
   * @param rhs Variable to divide.
   * @return Result of division.
   */
  VariableMatrix& operator/=(const ScalarLike auto& rhs) {
    for (int row = 0; row < rows(); ++row) {
      for (int col = 0; col < cols(); ++col) {
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
   * @return Result of addition.
   */
  template <MatrixLike LHS, MatrixLike RHS>
    requires SleipnirMatrixLike<LHS> || SleipnirMatrixLike<RHS>
  friend SLEIPNIR_DLLEXPORT VariableMatrix operator+(const LHS& lhs,
                                                     const RHS& rhs) {
    slp_assert(lhs.rows() == rhs.rows() && lhs.cols() == rhs.cols());

    VariableMatrix result(VariableMatrix::empty, lhs.rows(), lhs.cols());

    for (int row = 0; row < result.rows(); ++row) {
      for (int col = 0; col < result.cols(); ++col) {
        result(row, col) = lhs(row, col) + rhs(row, col);
      }
    }

    return result;
  }

  /**
   * Compound addition-assignment operator.
   *
   * @param rhs Variable to add.
   * @return Result of addition.
   */
  VariableMatrix& operator+=(const MatrixLike auto& rhs) {
    slp_assert(rows() == rhs.rows() && cols() == rhs.cols());

    for (int row = 0; row < rows(); ++row) {
      for (int col = 0; col < cols(); ++col) {
        (*this)(row, col) += rhs(row, col);
      }
    }

    return *this;
  }

  /**
   * Compound addition-assignment operator.
   *
   * @param rhs Variable to add.
   * @return Result of addition.
   */
  VariableMatrix& operator+=(const ScalarLike auto& rhs) {
    slp_assert(rows() == 1 && cols() == 1);

    for (int row = 0; row < rows(); ++row) {
      for (int col = 0; col < cols(); ++col) {
        (*this)(row, col) += rhs;
      }
    }

    return *this;
  }

  /**
   * Binary subtraction operator.
   *
   * @param lhs Operator left-hand side.
   * @param rhs Operator right-hand side.
   * @return Result of subtraction.
   */
  template <MatrixLike LHS, MatrixLike RHS>
    requires SleipnirMatrixLike<LHS> || SleipnirMatrixLike<RHS>
  friend SLEIPNIR_DLLEXPORT VariableMatrix operator-(const LHS& lhs,
                                                     const RHS& rhs) {
    slp_assert(lhs.rows() == rhs.rows() && lhs.cols() == rhs.cols());

    VariableMatrix result(VariableMatrix::empty, lhs.rows(), lhs.cols());

    for (int row = 0; row < result.rows(); ++row) {
      for (int col = 0; col < result.cols(); ++col) {
        result(row, col) = lhs(row, col) - rhs(row, col);
      }
    }

    return result;
  }

  /**
   * Compound subtraction-assignment operator.
   *
   * @param rhs Variable to subtract.
   * @return Result of subtraction.
   */
  VariableMatrix& operator-=(const MatrixLike auto& rhs) {
    slp_assert(rows() == rhs.rows() && cols() == rhs.cols());

    for (int row = 0; row < rows(); ++row) {
      for (int col = 0; col < cols(); ++col) {
        (*this)(row, col) -= rhs(row, col);
      }
    }

    return *this;
  }

  /**
   * Compound subtraction-assignment operator.
   *
   * @param rhs Variable to subtract.
   * @return Result of subtraction.
   */
  VariableMatrix& operator-=(const ScalarLike auto& rhs) {
    slp_assert(rows() == 1 && cols() == 1);

    for (int row = 0; row < rows(); ++row) {
      for (int col = 0; col < cols(); ++col) {
        (*this)(row, col) -= rhs;
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
  operator-(const SleipnirMatrixLike auto& lhs) {
    VariableMatrix result{VariableMatrix::empty, lhs.rows(), lhs.cols()};

    for (int row = 0; row < result.rows(); ++row) {
      for (int col = 0; col < result.cols(); ++col) {
        result(row, col) = -lhs(row, col);
      }
    }

    return result;
  }

  /**
   * Implicit conversion operator from 1x1 VariableMatrix to Variable.
   */
  operator Variable() const {  // NOLINT
    slp_assert(rows() == 1 && cols() == 1);
    return (*this)(0, 0);
  }

  /**
   * Returns the transpose of the variable matrix.
   *
   * @return The transpose of the variable matrix.
   */
  VariableMatrix T() const {
    VariableMatrix result{VariableMatrix::empty, cols(), rows()};

    for (int row = 0; row < rows(); ++row) {
      for (int col = 0; col < cols(); ++col) {
        result(col, row) = (*this)(row, col);
      }
    }

    return result;
  }

  /**
   * Returns the number of rows in the matrix.
   *
   * @return The number of rows in the matrix.
   */
  int rows() const { return m_rows; }

  /**
   * Returns the number of columns in the matrix.
   *
   * @return The number of columns in the matrix.
   */
  int cols() const { return m_cols; }

  /**
   * Returns an element of the variable matrix.
   *
   * @param row The row of the element to return.
   * @param col The column of the element to return.
   * @return An element of the variable matrix.
   */
  double value(int row, int col) {
    slp_assert(row >= 0 && row < rows());
    slp_assert(col >= 0 && col < cols());
    return m_storage[row * cols() + col].value();
  }

  /**
   * Returns a row of the variable column vector.
   *
   * @param index The index of the element to return.
   * @return A row of the variable column vector.
   */
  double value(int index) {
    slp_assert(index >= 0 && index < rows() * cols());
    return m_storage[index].value();
  }

  /**
   * Returns the contents of the variable matrix.
   *
   * @return The contents of the variable matrix.
   */
  Eigen::MatrixXd value() {
    Eigen::MatrixXd result{rows(), cols()};

    for (int row = 0; row < rows(); ++row) {
      for (int col = 0; col < cols(); ++col) {
        result(row, col) = value(row, col);
      }
    }

    return result;
  }

  /**
   * Transforms the matrix coefficient-wise with an unary operator.
   *
   * @param unary_op The unary operator to use for the transform operation.
   * @return Result of the unary operator.
   */
  VariableMatrix cwise_transform(
      function_ref<Variable(const Variable& x)> unary_op) const {
    VariableMatrix result{VariableMatrix::empty, rows(), cols()};

    for (int row = 0; row < rows(); ++row) {
      for (int col = 0; col < cols(); ++col) {
        result(row, col) = unary_op((*this)(row, col));
      }
    }

    return result;
  }

#ifndef DOXYGEN_SHOULD_SKIP_THIS

  class iterator {
   public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = Variable;
    using difference_type = std::ptrdiff_t;
    using pointer = Variable*;
    using reference = Variable&;

    constexpr iterator() noexcept = default;

    explicit constexpr iterator(
        gch::small_vector<Variable>::iterator it) noexcept
        : m_it{it} {}

    constexpr iterator& operator++() noexcept {
      ++m_it;
      return *this;
    }

    constexpr iterator operator++(int) noexcept {
      iterator retval = *this;
      ++(*this);
      return retval;
    }

    constexpr bool operator==(const iterator&) const noexcept = default;

    constexpr reference operator*() const noexcept { return *m_it; }

   private:
    gch::small_vector<Variable>::iterator m_it;
  };

  class const_iterator {
   public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = Variable;
    using difference_type = std::ptrdiff_t;
    using pointer = Variable*;
    using const_reference = const Variable&;

    constexpr const_iterator() noexcept = default;

    explicit constexpr const_iterator(
        gch::small_vector<Variable>::const_iterator it) noexcept
        : m_it{it} {}

    constexpr const_iterator& operator++() noexcept {
      ++m_it;
      return *this;
    }

    constexpr const_iterator operator++(int) noexcept {
      const_iterator retval = *this;
      ++(*this);
      return retval;
    }

    constexpr bool operator==(const const_iterator&) const noexcept = default;

    constexpr const_reference operator*() const noexcept { return *m_it; }

   private:
    gch::small_vector<Variable>::const_iterator m_it;
  };

#endif  // DOXYGEN_SHOULD_SKIP_THIS

  /**
   * Returns begin iterator.
   *
   * @return Begin iterator.
   */
  iterator begin() { return iterator{m_storage.begin()}; }

  /**
   * Returns end iterator.
   *
   * @return End iterator.
   */
  iterator end() { return iterator{m_storage.end()}; }

  /**
   * Returns begin iterator.
   *
   * @return Begin iterator.
   */
  const_iterator begin() const { return const_iterator{m_storage.begin()}; }

  /**
   * Returns end iterator.
   *
   * @return End iterator.
   */
  const_iterator end() const { return const_iterator{m_storage.end()}; }

  /**
   * Returns begin iterator.
   *
   * @return Begin iterator.
   */
  const_iterator cbegin() const { return const_iterator{m_storage.begin()}; }

  /**
   * Returns end iterator.
   *
   * @return End iterator.
   */
  const_iterator cend() const { return const_iterator{m_storage.end()}; }

  /**
   * Returns number of elements in matrix.
   *
   * @return Number of elements in matrix.
   */
  size_t size() const { return m_storage.size(); }

  /**
   * Returns a variable matrix filled with zeroes.
   *
   * @param rows The number of matrix rows.
   * @param cols The number of matrix columns.
   * @return A variable matrix filled with zeroes.
   */
  static VariableMatrix zero(int rows, int cols) {
    VariableMatrix result{VariableMatrix::empty, rows, cols};

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
   * @return A variable matrix filled with ones.
   */
  static VariableMatrix ones(int rows, int cols) {
    VariableMatrix result{VariableMatrix::empty, rows, cols};

    for (auto& elem : result) {
      elem = 1.0;
    }

    return result;
  }

 private:
  gch::small_vector<Variable> m_storage;
  int m_rows = 0;
  int m_cols = 0;
};

/**
 * Applies a coefficient-wise reduce operation to two matrices.
 *
 * @param lhs The left-hand side of the binary operator.
 * @param rhs The right-hand side of the binary operator.
 * @param binary_op The binary operator to use for the reduce operation.
 */
SLEIPNIR_DLLEXPORT inline VariableMatrix cwise_reduce(
    const VariableMatrix& lhs, const VariableMatrix& rhs,
    function_ref<Variable(const Variable& x, const Variable& y)> binary_op) {
  slp_assert(lhs.rows() == rhs.rows() && lhs.rows() == rhs.rows());

  VariableMatrix result{VariableMatrix::empty, lhs.rows(), lhs.cols()};

  for (int row = 0; row < lhs.rows(); ++row) {
    for (int col = 0; col < lhs.cols(); ++col) {
      result(row, col) = binary_op(lhs(row, col), rhs(row, col));
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
SLEIPNIR_DLLEXPORT inline VariableMatrix block(
    std::initializer_list<std::initializer_list<VariableMatrix>> list) {
  // Get row and column counts for destination matrix
  int rows = 0;
  int cols = -1;
  for (const auto& row : list) {
    if (row.size() > 0) {
      rows += row.begin()->rows();
    }

    // Get number of columns in this row
    int latest_cols = 0;
    for (const auto& elem : row) {
      // Assert the first and latest row have the same height
      slp_assert(row.begin()->rows() == elem.rows());

      latest_cols += elem.cols();
    }

    // If this is the first row, record the column count. Otherwise, assert the
    // first and latest column counts are the same.
    if (cols == -1) {
      cols = latest_cols;
    } else {
      slp_assert(cols == latest_cols);
    }
  }

  VariableMatrix result{VariableMatrix::empty, rows, cols};

  int row_offset = 0;
  for (const auto& row : list) {
    int col_offset = 0;
    for (const auto& elem : row) {
      result.block(row_offset, col_offset, elem.rows(), elem.cols()) = elem;
      col_offset += elem.cols();
    }
    row_offset += row.begin()->rows();
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
SLEIPNIR_DLLEXPORT inline VariableMatrix block(
    const std::vector<std::vector<VariableMatrix>>& list) {
  // Get row and column counts for destination matrix
  int rows = 0;
  int cols = -1;
  for (const auto& row : list) {
    if (row.size() > 0) {
      rows += row.begin()->rows();
    }

    // Get number of columns in this row
    int latest_cols = 0;
    for (const auto& elem : row) {
      // Assert the first and latest row have the same height
      slp_assert(row.begin()->rows() == elem.rows());

      latest_cols += elem.cols();
    }

    // If this is the first row, record the column count. Otherwise, assert the
    // first and latest column counts are the same.
    if (cols == -1) {
      cols = latest_cols;
    } else {
      slp_assert(cols == latest_cols);
    }
  }

  VariableMatrix result{VariableMatrix::empty, rows, cols};

  int row_offset = 0;
  for (const auto& row : list) {
    int col_offset = 0;
    for (const auto& elem : row) {
      result.block(row_offset, col_offset, elem.rows(), elem.cols()) = elem;
      col_offset += elem.cols();
    }
    row_offset += row.begin()->rows();
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
SLEIPNIR_DLLEXPORT VariableMatrix solve(const VariableMatrix& A,
                                        const VariableMatrix& B);

}  // namespace slp
