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
#include <Eigen/QR>
#include <gch/small_vector.hpp>

#include "sleipnir/autodiff/sleipnir_base.hpp"
#include "sleipnir/autodiff/slice.hpp"
#include "sleipnir/autodiff/variable.hpp"
#include "sleipnir/autodiff/variable_block.hpp"
#include "sleipnir/util/assert.hpp"
#include "sleipnir/util/concepts.hpp"
#include "sleipnir/util/empty.hpp"
#include "sleipnir/util/function_ref.hpp"
#include "sleipnir/util/symbol_exports.hpp"

namespace slp {

/**
 * A matrix of autodiff variables.
 *
 * @tparam Scalar_ Scalar type.
 */
template <typename Scalar_>
class VariableMatrix : public SleipnirBase {
 public:
  /**
   * Scalar type alias.
   */
  using Scalar = Scalar_;

  /**
   * Constructs an empty VariableMatrix.
   */
  VariableMatrix() = default;

  /**
   * Constructs a zero-initialized VariableMatrix column vector with the given
   * rows.
   *
   * @param rows The number of matrix rows.
   */
  explicit VariableMatrix(int rows) : VariableMatrix{rows, 1} {}

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
  VariableMatrix(detail::empty_t, int rows, int cols)
      : m_rows{rows}, m_cols{cols} {
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
  VariableMatrix(
      std::initializer_list<std::initializer_list<Variable<Scalar>>> list) {
    // Get row and column counts for destination matrix
    m_rows = list.size();
    m_cols = 0;
    if (list.size() > 0) {
      m_cols = list.begin()->size();
    }

    // Assert all column counts are the same
    for ([[maybe_unused]]
         const auto& row : list) {
      slp_assert(static_cast<int>(row.size()) == m_cols);
    }

    m_storage.reserve(rows() * cols());
    for (const auto& row : list) {
      std::ranges::copy(row, std::back_inserter(m_storage));
    }
  }

  /**
   * Constructs a scalar VariableMatrix from a nested list of scalars.
   *
   * This overload is for Python bindings only.
   *
   * @param list The nested list of Variables.
   */
  // NOLINTNEXTLINE (google-explicit-constructor)
  VariableMatrix(const std::vector<std::vector<Scalar>>& list) {
    // Get row and column counts for destination matrix
    m_rows = list.size();
    m_cols = 0;
    if (list.size() > 0) {
      m_cols = list.begin()->size();
    }

    // Assert all column counts are the same
    for ([[maybe_unused]]
         const auto& row : list) {
      slp_assert(static_cast<int>(row.size()) == m_cols);
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
  // NOLINTNEXTLINE (google-explicit-constructor)
  VariableMatrix(const std::vector<std::vector<Variable<Scalar>>>& list) {
    // Get row and column counts for destination matrix
    m_rows = list.size();
    m_cols = 0;
    if (list.size() > 0) {
      m_cols = list.begin()->size();
    }

    // Assert all column counts are the same
    for ([[maybe_unused]]
         const auto& row : list) {
      slp_assert(static_cast<int>(row.size()) == m_cols);
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
  // NOLINTNEXTLINE (google-explicit-constructor)
  VariableMatrix(const Eigen::MatrixBase<Derived>& values)
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
  // NOLINTNEXTLINE (google-explicit-constructor)
  VariableMatrix(const Eigen::DiagonalBase<Derived>& values)
      : m_rows{static_cast<int>(values.rows())},
        m_cols{static_cast<int>(values.cols())} {
    m_storage.reserve(values.rows() * values.cols());
    for (int row = 0; row < values.rows(); ++row) {
      for (int col = 0; col < values.cols(); ++col) {
        if (row == col) {
          m_storage.emplace_back(values.diagonal()[row]);
        } else {
          m_storage.emplace_back(Scalar(0));
        }
      }
    }
  }

  /**
   * Constructs a scalar VariableMatrix from a Variable.
   *
   * @param variable Variable.
   */
  // NOLINTNEXTLINE (google-explicit-constructor)
  VariableMatrix(const Variable<Scalar>& variable) : m_rows{1}, m_cols{1} {
    m_storage.emplace_back(variable);
  }

  /**
   * Constructs a scalar VariableMatrix from a Variable.
   *
   * @param variable Variable.
   */
  // NOLINTNEXTLINE (google-explicit-constructor)
  VariableMatrix(Variable<Scalar>&& variable) : m_rows{1}, m_cols{1} {
    m_storage.emplace_back(std::move(variable));
  }

  /**
   * Constructs a VariableMatrix from a VariableBlock.
   *
   * @param values VariableBlock of values.
   */
  // NOLINTNEXTLINE (google-explicit-constructor)
  VariableMatrix(const VariableBlock<VariableMatrix>& values)
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
  // NOLINTNEXTLINE (google-explicit-constructor)
  VariableMatrix(const VariableBlock<const VariableMatrix>& values)
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
  explicit VariableMatrix(std::span<const Variable<Scalar>> values)
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
  VariableMatrix(std::span<const Variable<Scalar>> values, int rows, int cols)
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
   * Assigns a scalar to the matrix.
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
    requires std::same_as<typename Derived::Scalar, Scalar>
  void set_value(const Eigen::MatrixBase<Derived>& values) {
    slp_assert(rows() == values.rows() && cols() == values.cols());

    for (int row = 0; row < values.rows(); ++row) {
      for (int col = 0; col < values.cols(); ++col) {
        (*this)(row, col).set_value(values(row, col));
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
  Variable<Scalar>& operator()(int row, int col) {
    slp_assert(row >= 0 && row < rows());
    slp_assert(col >= 0 && col < cols());
    return m_storage[row * cols() + col];
  }

  /**
   * Returns the element at the given row and column.
   *
   * @param row The row.
   * @param col The column.
   * @return The element at the given row and column.
   */
  const Variable<Scalar>& operator()(int row, int col) const {
    slp_assert(row >= 0 && row < rows());
    slp_assert(col >= 0 && col < cols());
    return m_storage[row * cols() + col];
  }

  /**
   * Returns the element at the given index.
   *
   * @param index The index.
   * @return The element at the given index.
   */
  Variable<Scalar>& operator[](int index) {
    slp_assert(index >= 0 && index < rows() * cols());
    return m_storage[index];
  }

  /**
   * Returns the element at the given index.
   *
   * @param index The index.
   * @return The element at the given index.
   */
  const Variable<Scalar>& operator[](int index) const {
    slp_assert(index >= 0 && index < rows() * cols());
    return m_storage[index];
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
    slp_assert(cols() == 1);
    slp_assert(offset >= 0 && offset < rows());
    slp_assert(length >= 0 && length <= rows() - offset);
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
    slp_assert(cols() == 1);
    slp_assert(offset >= 0 && offset < rows());
    slp_assert(length >= 0 && length <= rows() - offset);
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
  template <EigenMatrixLike LHS, SleipnirMatrixLike<Scalar> RHS>
  friend VariableMatrix<Scalar> operator*(const LHS& lhs, const RHS& rhs) {
    slp_assert(lhs.cols() == rhs.rows());

    VariableMatrix<Scalar> result(detail::empty, lhs.rows(), rhs.cols());

#if __GNUC__ >= 12
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif
    for (int i = 0; i < lhs.rows(); ++i) {
      for (int j = 0; j < rhs.cols(); ++j) {
        Variable sum{Scalar(0)};
        for (int k = 0; k < lhs.cols(); ++k) {
          sum += lhs(i, k) * rhs(k, j);
        }
        result(i, j) = sum;
      }
    }

    return result;
  }

  /**
   * Matrix multiplication operator.
   *
   * @param lhs Operator left-hand side.
   * @param rhs Operator right-hand side.
   */
  template <SleipnirMatrixLike<Scalar> LHS, EigenMatrixLike RHS>
  friend VariableMatrix<Scalar> operator*(const LHS& lhs, const RHS& rhs) {
    slp_assert(lhs.cols() == rhs.rows());

    VariableMatrix<Scalar> result(detail::empty, lhs.rows(), rhs.cols());

    for (int i = 0; i < lhs.rows(); ++i) {
      for (int j = 0; j < rhs.cols(); ++j) {
        Variable sum{Scalar(0)};
        for (int k = 0; k < lhs.cols(); ++k) {
          sum += lhs(i, k) * rhs(k, j);
        }
        result(i, j) = sum;
      }
    }

    return result;
  }

  /**
   * Matrix multiplication operator.
   *
   * @param lhs Operator left-hand side.
   * @param rhs Operator right-hand side.
   */
  template <SleipnirMatrixLike<Scalar> LHS, SleipnirMatrixLike<Scalar> RHS>
  friend VariableMatrix<Scalar> operator*(const LHS& lhs, const RHS& rhs) {
    slp_assert(lhs.cols() == rhs.rows());

    VariableMatrix<Scalar> result(detail::empty, lhs.rows(), rhs.cols());

    for (int i = 0; i < lhs.rows(); ++i) {
      for (int j = 0; j < rhs.cols(); ++j) {
        Variable sum{Scalar(0)};
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
  template <EigenMatrixLike LHS>
  friend VariableMatrix<Scalar> operator*(const LHS& lhs,
                                          const Variable<Scalar>& rhs) {
    VariableMatrix<Scalar> result(detail::empty, lhs.rows(), lhs.cols());

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
  template <SleipnirMatrixLike<Scalar> LHS, ScalarLike RHS>
  friend VariableMatrix<Scalar> operator*(const LHS& lhs, const RHS& rhs) {
    VariableMatrix<Scalar> result(detail::empty, lhs.rows(), lhs.cols());

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
  template <EigenMatrixLike RHS>
  friend VariableMatrix<Scalar> operator*(const Variable<Scalar>& lhs,
                                          const RHS& rhs) {
    VariableMatrix<Scalar> result(detail::empty, rhs.rows(), rhs.cols());

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
  template <ScalarLike LHS, SleipnirMatrixLike<Scalar> RHS>
  friend VariableMatrix<Scalar> operator*(const LHS& lhs, const RHS& rhs) {
    VariableMatrix<Scalar> result(detail::empty, rhs.rows(), rhs.cols());

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
        Variable sum{Scalar(0)};
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
  template <EigenMatrixLike LHS>
  friend VariableMatrix<Scalar> operator/(const LHS& lhs,
                                          const Variable<Scalar>& rhs) {
    VariableMatrix<Scalar> result(detail::empty, lhs.rows(), lhs.cols());

    for (int row = 0; row < result.rows(); ++row) {
      for (int col = 0; col < result.cols(); ++col) {
        result(row, col) = lhs(row, col) / rhs;
      }
    }

    return result;
  }

  /**
   * Binary division operator.
   *
   * @param lhs Operator left-hand side.
   * @param rhs Operator right-hand side.
   * @return Result of division.
   */
  template <SleipnirMatrixLike<Scalar> LHS, ScalarLike RHS>
    requires(!SleipnirScalarLike<RHS, Scalar>)
  friend VariableMatrix<Scalar> operator/(const LHS& lhs, const RHS& rhs) {
    VariableMatrix<Scalar> result(detail::empty, lhs.rows(), lhs.cols());

    for (int row = 0; row < result.rows(); ++row) {
      for (int col = 0; col < result.cols(); ++col) {
        result(row, col) = lhs(row, col) / rhs;
      }
    }

    return result;
  }

  /**
   * Binary division operator.
   *
   * @param lhs Operator left-hand side.
   * @param rhs Operator right-hand side.
   * @return Result of division.
   */
  template <SleipnirMatrixLike<Scalar> LHS>
  friend VariableMatrix<Scalar> operator/(const LHS& lhs,
                                          const Variable<Scalar>& rhs) {
    VariableMatrix<Scalar> result(detail::empty, lhs.rows(), lhs.cols());

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
  template <EigenMatrixLike LHS, SleipnirMatrixLike<Scalar> RHS>
  friend VariableMatrix<Scalar> operator+(const LHS& lhs, const RHS& rhs) {
    slp_assert(lhs.rows() == rhs.rows() && lhs.cols() == rhs.cols());

    VariableMatrix<Scalar> result(detail::empty, lhs.rows(), lhs.cols());

    for (int row = 0; row < result.rows(); ++row) {
      for (int col = 0; col < result.cols(); ++col) {
        result(row, col) = lhs(row, col) + rhs(row, col);
      }
    }

    return result;
  }

  /**
   * Binary addition operator.
   *
   * @param lhs Operator left-hand side.
   * @param rhs Operator right-hand side.
   * @return Result of addition.
   */
  template <SleipnirMatrixLike<Scalar> LHS, EigenMatrixLike RHS>
  friend VariableMatrix<Scalar> operator+(const LHS& lhs, const RHS& rhs) {
    slp_assert(lhs.rows() == rhs.rows() && lhs.cols() == rhs.cols());

    VariableMatrix<Scalar> result(detail::empty, lhs.rows(), lhs.cols());

    for (int row = 0; row < result.rows(); ++row) {
      for (int col = 0; col < result.cols(); ++col) {
        result(row, col) = lhs(row, col) + rhs(row, col);
      }
    }

    return result;
  }

  /**
   * Binary addition operator.
   *
   * @param lhs Operator left-hand side.
   * @param rhs Operator right-hand side.
   * @return Result of addition.
   */
  template <SleipnirMatrixLike<Scalar> LHS, SleipnirMatrixLike<Scalar> RHS>
  friend VariableMatrix<Scalar> operator+(const LHS& lhs, const RHS& rhs) {
    slp_assert(lhs.rows() == rhs.rows() && lhs.cols() == rhs.cols());

    VariableMatrix<Scalar> result(detail::empty, lhs.rows(), lhs.cols());

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
  template <EigenMatrixLike LHS, SleipnirMatrixLike<Scalar> RHS>
  friend VariableMatrix<Scalar> operator-(const LHS& lhs, const RHS& rhs) {
    slp_assert(lhs.rows() == rhs.rows() && lhs.cols() == rhs.cols());

    VariableMatrix<Scalar> result(detail::empty, lhs.rows(), lhs.cols());

    for (int row = 0; row < result.rows(); ++row) {
      for (int col = 0; col < result.cols(); ++col) {
        result(row, col) = lhs(row, col) - rhs(row, col);
      }
    }

    return result;
  }

  /**
   * Binary subtraction operator.
   *
   * @param lhs Operator left-hand side.
   * @param rhs Operator right-hand side.
   * @return Result of subtraction.
   */
  template <SleipnirMatrixLike<Scalar> LHS, EigenMatrixLike RHS>
  friend VariableMatrix<Scalar> operator-(const LHS& lhs, const RHS& rhs) {
    slp_assert(lhs.rows() == rhs.rows() && lhs.cols() == rhs.cols());

    VariableMatrix<Scalar> result(detail::empty, lhs.rows(), lhs.cols());

    for (int row = 0; row < result.rows(); ++row) {
      for (int col = 0; col < result.cols(); ++col) {
        result(row, col) = lhs(row, col) - rhs(row, col);
      }
    }

    return result;
  }

  /**
   * Binary subtraction operator.
   *
   * @param lhs Operator left-hand side.
   * @param rhs Operator right-hand side.
   * @return Result of subtraction.
   */
  template <SleipnirMatrixLike<Scalar> LHS, SleipnirMatrixLike<Scalar> RHS>
  friend VariableMatrix<Scalar> operator-(const LHS& lhs, const RHS& rhs) {
    slp_assert(lhs.rows() == rhs.rows() && lhs.cols() == rhs.cols());

    VariableMatrix<Scalar> result(detail::empty, lhs.rows(), lhs.cols());

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
  friend VariableMatrix<Scalar> operator-(
      const SleipnirMatrixLike<Scalar> auto& lhs) {
    VariableMatrix<Scalar> result{detail::empty, lhs.rows(), lhs.cols()};

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
  // NOLINTNEXTLINE (google-explicit-constructor)
  operator Variable<Scalar>() const {
    slp_assert(rows() == 1 && cols() == 1);
    return (*this)(0, 0);
  }

  /**
   * Returns the transpose of the variable matrix.
   *
   * @return The transpose of the variable matrix.
   */
  VariableMatrix<Scalar> T() const {
    VariableMatrix<Scalar> result{detail::empty, cols(), rows()};

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
  Scalar value(int row, int col) { return (*this)(row, col).value(); }

  /**
   * Returns an element of the variable matrix.
   *
   * @param index The index of the element to return.
   * @return An element of the variable matrix.
   */
  Scalar value(int index) { return (*this)[index].value(); }

  /**
   * Returns the contents of the variable matrix.
   *
   * @return The contents of the variable matrix.
   */
  Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic> value() {
    Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic> result{rows(),
                                                                 cols()};

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
  VariableMatrix<Scalar> cwise_transform(
      function_ref<Variable<Scalar>(const Variable<Scalar>& x)> unary_op)
      const {
    VariableMatrix<Scalar> result{detail::empty, rows(), cols()};

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
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = Variable<Scalar>;
    using difference_type = std::ptrdiff_t;
    using pointer = Variable<Scalar>*;
    using reference = Variable<Scalar>&;

    constexpr iterator() noexcept = default;

    explicit constexpr iterator(
        gch::small_vector<Variable<Scalar>>::iterator it) noexcept
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

    constexpr iterator& operator--() noexcept {
      --m_it;
      return *this;
    }

    constexpr iterator operator--(int) noexcept {
      iterator retval = *this;
      --(*this);
      return retval;
    }

    constexpr bool operator==(const iterator&) const noexcept = default;

    constexpr reference operator*() const noexcept { return *m_it; }

   private:
    gch::small_vector<Variable<Scalar>>::iterator m_it;
  };

  class const_iterator {
   public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = Variable<Scalar>;
    using difference_type = std::ptrdiff_t;
    using pointer = Variable<Scalar>*;
    using const_reference = const Variable<Scalar>&;

    constexpr const_iterator() noexcept = default;

    explicit constexpr const_iterator(
        gch::small_vector<Variable<Scalar>>::const_iterator it) noexcept
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

    constexpr const_iterator& operator--() noexcept {
      --m_it;
      return *this;
    }

    constexpr const_iterator operator--(int) noexcept {
      const_iterator retval = *this;
      --(*this);
      return retval;
    }

    constexpr bool operator==(const const_iterator&) const noexcept = default;

    constexpr const_reference operator*() const noexcept { return *m_it; }

   private:
    gch::small_vector<Variable<Scalar>>::const_iterator m_it;
  };

  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

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
   * Returns const begin iterator.
   *
   * @return Const begin iterator.
   */
  const_iterator begin() const { return const_iterator{m_storage.begin()}; }

  /**
   * Returns const end iterator.
   *
   * @return Const end iterator.
   */
  const_iterator end() const { return const_iterator{m_storage.end()}; }

  /**
   * Returns const begin iterator.
   *
   * @return Const begin iterator.
   */
  const_iterator cbegin() const { return const_iterator{m_storage.begin()}; }

  /**
   * Returns const end iterator.
   *
   * @return Const end iterator.
   */
  const_iterator cend() const { return const_iterator{m_storage.end()}; }

  /**
   * Returns reverse begin iterator.
   *
   * @return Reverse begin iterator.
   */
  reverse_iterator rbegin() { return reverse_iterator{end()}; }

  /**
   * Returns reverse end iterator.
   *
   * @return Reverse end iterator.
   */
  reverse_iterator rend() { return reverse_iterator{begin()}; }

  /**
   * Returns const reverse begin iterator.
   *
   * @return Const reverse begin iterator.
   */
  const_reverse_iterator rbegin() const {
    return const_reverse_iterator{end()};
  }

  /**
   * Returns const reverse end iterator.
   *
   * @return Const reverse end iterator.
   */
  const_reverse_iterator rend() const {
    return const_reverse_iterator{begin()};
  }

  /**
   * Returns const reverse begin iterator.
   *
   * @return Const reverse begin iterator.
   */
  const_reverse_iterator crbegin() const {
    return const_reverse_iterator{cend()};
  }

  /**
   * Returns const reverse end iterator.
   *
   * @return Const reverse end iterator.
   */
  const_reverse_iterator crend() const {
    return const_reverse_iterator{cbegin()};
  }

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
  static VariableMatrix<Scalar> zero(int rows, int cols) {
    VariableMatrix<Scalar> result{detail::empty, rows, cols};

    for (auto& elem : result) {
      elem = Scalar(0);
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
  static VariableMatrix<Scalar> ones(int rows, int cols) {
    VariableMatrix<Scalar> result{detail::empty, rows, cols};

    for (auto& elem : result) {
      elem = Scalar(1);
    }

    return result;
  }

 private:
  gch::small_vector<Variable<Scalar>> m_storage;
  int m_rows = 0;
  int m_cols = 0;
};

template <typename Derived>
VariableMatrix(const Eigen::MatrixBase<Derived>&)
    -> VariableMatrix<typename Derived::Scalar>;

template <typename Derived>
VariableMatrix(const Eigen::DiagonalBase<Derived>&)
    -> VariableMatrix<typename Derived::Scalar>;

/**
 * Applies a coefficient-wise reduce operation to two matrices.
 *
 * @tparam Scalar Scalar type.
 * @param lhs The left-hand side of the binary operator.
 * @param rhs The right-hand side of the binary operator.
 * @param binary_op The binary operator to use for the reduce operation.
 */
template <typename Scalar>
VariableMatrix<Scalar> cwise_reduce(
    const VariableMatrix<Scalar>& lhs, const VariableMatrix<Scalar>& rhs,
    function_ref<Variable<Scalar>(const Variable<Scalar>& x,
                                  const Variable<Scalar>& y)>
        binary_op) {
  slp_assert(lhs.rows() == rhs.rows() && lhs.cols() == rhs.cols());

  VariableMatrix<Scalar> result{detail::empty, lhs.rows(), lhs.cols()};

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
 * @tparam Scalar Scalar type.
 * @param list The nested list of blocks.
 */
template <typename Scalar>
VariableMatrix<Scalar> block(
    std::initializer_list<std::initializer_list<VariableMatrix<Scalar>>> list) {
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

  VariableMatrix<Scalar> result{detail::empty, rows, cols};

  int row_offset = 0;
  for (const auto& row : list) {
    int col_offset = 0;
    for (const auto& elem : row) {
      result.block(row_offset, col_offset, elem.rows(), elem.cols()) = elem;
      col_offset += elem.cols();
    }
    if (row.size() > 0) {
      row_offset += row.begin()->rows();
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
 * This overload is for Python bindings only.
 *
 * @tparam Scalar Scalar type.
 * @param list The nested list of blocks.
 */
template <typename Scalar>
VariableMatrix<Scalar> block(
    const std::vector<std::vector<VariableMatrix<Scalar>>>& list) {
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

  VariableMatrix<Scalar> result{detail::empty, rows, cols};

  int row_offset = 0;
  for (const auto& row : list) {
    int col_offset = 0;
    for (const auto& elem : row) {
      result.block(row_offset, col_offset, elem.rows(), elem.cols()) = elem;
      col_offset += elem.cols();
    }
    if (row.size() > 0) {
      row_offset += row.begin()->rows();
    }
  }

  return result;
}

/**
 * Solves the VariableMatrix equation AX = B for X.
 *
 * @tparam Scalar Scalar type.
 * @param A The left-hand side.
 * @param B The right-hand side.
 * @return The solution X.
 */
template <typename Scalar>
VariableMatrix<Scalar> solve(const VariableMatrix<Scalar>& A,
                             const VariableMatrix<Scalar>& B) {
  // m x n * n x p = m x p
  slp_assert(A.rows() == B.rows());

  if (A.rows() == 1 && A.cols() == 1) {
    // Compute optimal inverse instead of using Eigen's general solver
    return B(0, 0) / A(0, 0);
  } else if (A.rows() == 2 && A.cols() == 2) {
    // Compute optimal inverse instead of using Eigen's general solver
    //
    // [a  b]⁻¹  ___1___ [ d  −b]
    // [c  d]  = ad − bc [−c   a]

    const auto& a = A(0, 0);
    const auto& b = A(0, 1);
    const auto& c = A(1, 0);
    const auto& d = A(1, 1);

    VariableMatrix adj_A{{d, -b}, {-c, a}};
    auto det_A = a * d - b * c;
    return adj_A / det_A * B;
  } else if (A.rows() == 3 && A.cols() == 3) {
    // Compute optimal inverse instead of using Eigen's general solver
    //
    // [a  b  c]⁻¹
    // [d  e  f]
    // [g  h  i]
    //                        1                   [ei − fh  ch − bi  bf − ce]
    //     = ------------------------------------ [fg − di  ai − cg  cd − af]
    //       a(ei − fh) + b(fg − di) + c(dh − eg) [dh − eg  bg − ah  ae − bd]
    //
    // https://www.wolframalpha.com/input?i=inverse+%7B%7Ba%2C+b%2C+c%7D%2C+%7Bd%2C+e%2C+f%7D%2C+%7Bg%2C+h%2C+i%7D%7D

    const auto& a = A(0, 0);
    const auto& b = A(0, 1);
    const auto& c = A(0, 2);
    const auto& d = A(1, 0);
    const auto& e = A(1, 1);
    const auto& f = A(1, 2);
    const auto& g = A(2, 0);
    const auto& h = A(2, 1);
    const auto& i = A(2, 2);

    auto ae = a * e;
    auto af = a * f;
    auto ah = a * h;
    auto ai = a * i;
    auto bd = b * d;
    auto bf = b * f;
    auto bg = b * g;
    auto bi = b * i;
    auto cd = c * d;
    auto ce = c * e;
    auto cg = c * g;
    auto ch = c * h;
    auto dh = d * h;
    auto di = d * i;
    auto eg = e * g;
    auto ei = e * i;
    auto fg = f * g;
    auto fh = f * h;

    auto adj_A00 = ei - fh;
    auto adj_A10 = fg - di;
    auto adj_A20 = dh - eg;

    VariableMatrix adj_A{{adj_A00, ch - bi, bf - ce},
                         {adj_A10, ai - cg, cd - af},
                         {adj_A20, bg - ah, ae - bd}};
    auto det_A = a * adj_A00 + b * adj_A10 + c * adj_A20;
    return adj_A / det_A * B;
  } else if (A.rows() == 4 && A.cols() == 4) {
    // Compute optimal inverse instead of using Eigen's general solver
    //
    // [a  b  c  d]⁻¹
    // [e  f  g  h]
    // [i  j  k  l]
    // [m  n  o  p]
    //
    // https://www.wolframalpha.com/input?i=inverse+%7B%7Ba%2C+b%2C+c%2C+d%7D%2C+%7Be%2C+f%2C+g%2C+h%7D%2C+%7Bi%2C+j%2C+k%2C+l%7D%2C+%7Bm%2C+n%2C+o%2C+p%7D%7D

    const auto& a = A(0, 0);
    const auto& b = A(0, 1);
    const auto& c = A(0, 2);
    const auto& d = A(0, 3);
    const auto& e = A(1, 0);
    const auto& f = A(1, 1);
    const auto& g = A(1, 2);
    const auto& h = A(1, 3);
    const auto& i = A(2, 0);
    const auto& j = A(2, 1);
    const auto& k = A(2, 2);
    const auto& l = A(2, 3);
    const auto& m = A(3, 0);
    const auto& n = A(3, 1);
    const auto& o = A(3, 2);
    const auto& p = A(3, 3);

    auto afk = a * f * k;
    auto afl = a * f * l;
    auto afo = a * f * o;
    auto afp = a * f * p;
    auto agj = a * g * j;
    auto agl = a * g * l;
    auto agn = a * g * n;
    auto agp = a * g * p;
    auto ahj = a * h * j;
    auto ahk = a * h * k;
    auto ahn = a * h * n;
    auto aho = a * h * o;
    auto ajo = a * j * o;
    auto ajp = a * j * p;
    auto akn = a * k * n;
    auto akp = a * k * p;
    auto aln = a * l * n;
    auto alo = a * l * o;
    auto bek = b * e * k;
    auto bel = b * e * l;
    auto beo = b * e * o;
    auto bep = b * e * p;
    auto bgi = b * g * i;
    auto bgl = b * g * l;
    auto bgm = b * g * m;
    auto bgp = b * g * p;
    auto bhi = b * h * i;
    auto bhk = b * h * k;
    auto bhm = b * h * m;
    auto bho = b * h * o;
    auto bio = b * i * o;
    auto bip = b * i * p;
    auto bjp = b * j * p;
    auto bkm = b * k * m;
    auto bkp = b * k * p;
    auto blm = b * l * m;
    auto blo = b * l * o;
    auto cej = c * e * j;
    auto cel = c * e * l;
    auto cen = c * e * n;
    auto cep = c * e * p;
    auto cfi = c * f * i;
    auto cfl = c * f * l;
    auto cfm = c * f * m;
    auto cfp = c * f * p;
    auto chi = c * h * i;
    auto chj = c * h * j;
    auto chm = c * h * m;
    auto chn = c * h * n;
    auto cin = c * i * n;
    auto cip = c * i * p;
    auto cjm = c * j * m;
    auto cjp = c * j * p;
    auto clm = c * l * m;
    auto cln = c * l * n;
    auto dej = d * e * j;
    auto dek = d * e * k;
    auto den = d * e * n;
    auto deo = d * e * o;
    auto dfi = d * f * i;
    auto dfk = d * f * k;
    auto dfm = d * f * m;
    auto dfo = d * f * o;
    auto dgi = d * g * i;
    auto dgj = d * g * j;
    auto dgm = d * g * m;
    auto dgn = d * g * n;
    auto din = d * i * n;
    auto dio = d * i * o;
    auto djm = d * j * m;
    auto djo = d * j * o;
    auto dkm = d * k * m;
    auto dkn = d * k * n;
    auto ejo = e * j * o;
    auto ejp = e * j * p;
    auto ekn = e * k * n;
    auto ekp = e * k * p;
    auto eln = e * l * n;
    auto elo = e * l * o;
    auto fio = f * i * o;
    auto fip = f * i * p;
    auto fkm = f * k * m;
    auto fkp = f * k * p;
    auto flm = f * l * m;
    auto flo = f * l * o;
    auto gin = g * i * n;
    auto gip = g * i * p;
    auto gjm = g * j * m;
    auto gjp = g * j * p;
    auto glm = g * l * m;
    auto gln = g * l * n;
    auto hin = h * i * n;
    auto hio = h * i * o;
    auto hjm = h * j * m;
    auto hjo = h * j * o;
    auto hkm = h * k * m;
    auto hkn = h * k * n;

    auto adj_A00 = fkp - flo - gjp + gln + hjo - hkn;
    auto adj_A01 = -bkp + blo + cjp - cln - djo + dkn;
    auto adj_A02 = bgp - bho - cfp + chn + dfo - dgn;
    auto adj_A03 = -bgl + bhk + cfl - chj - dfk + dgj;
    auto adj_A10 = -ekp + elo + gip - glm - hio + hkm;
    auto adj_A11 = akp - alo - cip + clm + dio - dkm;
    auto adj_A12 = -agp + aho + cep - chm - deo + dgm;
    auto adj_A13 = agl - ahk - cel + chi + dek - dgi;
    auto adj_A20 = ejp - eln - fip + flm + hin - hjm;
    auto adj_A21 = -ajp + aln + bip - blm - din + djm;
    auto adj_A22 = afp - ahn - bep + bhm + den - dfm;
    auto adj_A23 = -afl + ahj + bel - bhi - dej + dfi;
    auto adj_A30 = -ejo + ekn + fio - fkm - gin + gjm;
    // NOLINTNEXTLINE(build/include_what_you_use)
    auto adj_A31 = ajo - akn - bio + bkm + cin - cjm;
    auto adj_A32 = -afo + agn + beo - bgm - cen + cfm;
    auto adj_A33 = afk - agj - bek + bgi + cej - cfi;

    VariableMatrix adj_A{{adj_A00, adj_A01, adj_A02, adj_A03},
                         {adj_A10, adj_A11, adj_A12, adj_A13},
                         {adj_A20, adj_A21, adj_A22, adj_A23},
                         {adj_A30, adj_A31, adj_A32, adj_A33}};
    auto det_A = a * adj_A00 + b * adj_A10 + c * adj_A20 + d * adj_A30;
    return adj_A / det_A * B;
  } else {
    using MatrixXv =
        Eigen::Matrix<Variable<Scalar>, Eigen::Dynamic, Eigen::Dynamic>;

    MatrixXv eigen_A{A.rows(), A.cols()};
    for (int row = 0; row < A.rows(); ++row) {
      for (int col = 0; col < A.cols(); ++col) {
        eigen_A(row, col) = A(row, col);
      }
    }

    MatrixXv eigen_B{B.rows(), B.cols()};
    for (int row = 0; row < B.rows(); ++row) {
      for (int col = 0; col < B.cols(); ++col) {
        eigen_B(row, col) = B(row, col);
      }
    }

    MatrixXv eigen_X = eigen_A.householderQr().solve(eigen_B);

    VariableMatrix<Scalar> X{detail::empty, A.cols(), B.cols()};
    for (int row = 0; row < X.rows(); ++row) {
      for (int col = 0; col < X.cols(); ++col) {
        X(row, col) = eigen_X(row, col);
      }
    }

    return X;
  }
}

extern template SLEIPNIR_DLLEXPORT VariableMatrix<double> solve(
    const VariableMatrix<double>& A, const VariableMatrix<double>& B);

}  // namespace slp
