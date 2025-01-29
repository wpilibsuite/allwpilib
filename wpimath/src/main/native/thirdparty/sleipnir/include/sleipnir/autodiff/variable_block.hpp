// Copyright (c) Sleipnir contributors

#pragma once

#include <concepts>
#include <type_traits>
#include <utility>

#include <Eigen/Core>

#include "sleipnir/autodiff/slice.hpp"
#include "sleipnir/autodiff/variable.hpp"
#include "sleipnir/util/assert.hpp"
#include "sleipnir/util/function_ref.hpp"

namespace slp {

/**
 * A submatrix of autodiff variables with reference semantics.
 *
 * @tparam Mat The type of the matrix whose storage this class points to.
 */
template <typename Mat>
class VariableBlock {
 public:
  /**
   * Copy constructor.
   */
  VariableBlock(const VariableBlock<Mat>&) = default;

  /**
   * Assigns a VariableBlock to the block.
   *
   * @param values VariableBlock of values.
   * @return This VariableBlock.
   */
  VariableBlock<Mat>& operator=(const VariableBlock<Mat>& values) {
    if (this == &values) {
      return *this;
    }

    if (m_mat == nullptr) {
      m_mat = values.m_mat;
      m_row_slice = values.m_row_slice;
      m_row_slice_length = values.m_row_slice_length;
      m_col_slice = values.m_col_slice;
      m_col_slice_length = values.m_col_slice_length;
    } else {
      slp_assert(rows() == values.rows() && cols() == values.cols());

      for (int row = 0; row < rows(); ++row) {
        for (int col = 0; col < cols(); ++col) {
          (*this)(row, col) = values(row, col);
        }
      }
    }

    return *this;
  }

  /**
   * Move constructor.
   */
  VariableBlock(VariableBlock<Mat>&&) = default;

  /**
   * Assigns a VariableBlock to the block.
   *
   * @param values VariableBlock of values.
   * @return This VariableBlock.
   */
  VariableBlock<Mat>& operator=(VariableBlock<Mat>&& values) {
    if (this == &values) {
      return *this;
    }

    if (m_mat == nullptr) {
      m_mat = values.m_mat;
      m_row_slice = values.m_row_slice;
      m_row_slice_length = values.m_row_slice_length;
      m_col_slice = values.m_col_slice;
      m_col_slice_length = values.m_col_slice_length;
    } else {
      slp_assert(rows() == values.rows() && cols() == values.cols());

      for (int row = 0; row < rows(); ++row) {
        for (int col = 0; col < cols(); ++col) {
          (*this)(row, col) = values(row, col);
        }
      }
    }

    return *this;
  }

  /**
   * Constructs a Variable block pointing to all of the given matrix.
   *
   * @param mat The matrix to which to point.
   */
  VariableBlock(Mat& mat)  // NOLINT
      : m_mat{&mat},
        m_row_slice{0, mat.rows(), 1},
        m_row_slice_length{m_row_slice.adjust(mat.rows())},
        m_col_slice{0, mat.cols(), 1},
        m_col_slice_length{m_col_slice.adjust(mat.cols())} {}

  /**
   * Constructs a Variable block pointing to a subset of the given matrix.
   *
   * @param mat The matrix to which to point.
   * @param row_offset The block's row offset.
   * @param col_offset The block's column offset.
   * @param block_rows The number of rows in the block.
   * @param block_cols The number of columns in the block.
   */
  VariableBlock(Mat& mat, int row_offset, int col_offset, int block_rows,
                int block_cols)
      : m_mat{&mat},
        m_row_slice{row_offset, row_offset + block_rows, 1},
        m_row_slice_length{m_row_slice.adjust(mat.rows())},
        m_col_slice{col_offset, col_offset + block_cols, 1},
        m_col_slice_length{m_col_slice.adjust(mat.cols())} {}

  /**
   * Constructs a Variable block pointing to a subset of the given matrix.
   *
   * Note that the slices are taken as is rather than adjusted.
   *
   * @param mat The matrix to which to point.
   * @param row_slice The block's row slice.
   * @param row_slice_length The block's row length.
   * @param col_slice The block's column slice.
   * @param col_slice_length The block's column length.
   */
  VariableBlock(Mat& mat, Slice row_slice, int row_slice_length,
                Slice col_slice, int col_slice_length)
      : m_mat{&mat},
        m_row_slice{std::move(row_slice)},
        m_row_slice_length{row_slice_length},
        m_col_slice{std::move(col_slice)},
        m_col_slice_length{col_slice_length} {}

  /**
   * Assigns a double to the block.
   *
   * This only works for blocks with one row and one column.
   *
   * @param value Value to assign.
   * @return This VariableBlock.
   */
  VariableBlock<Mat>& operator=(ScalarLike auto value) {
    slp_assert(rows() == 1 && cols() == 1);

    (*this)(0, 0) = value;

    return *this;
  }

  /**
   * Assigns a double to the block.
   *
   * This only works for blocks with one row and one column.
   *
   * @param value Value to assign.
   */
  void set_value(double value) {
    slp_assert(rows() == 1 && cols() == 1);

    (*this)(0, 0).set_value(value);
  }

  /**
   * Assigns an Eigen matrix to the block.
   *
   * @param values Eigen matrix of values to assign.
   * @return This VariableBlock.
   */
  template <typename Derived>
  VariableBlock<Mat>& operator=(const Eigen::MatrixBase<Derived>& values) {
    slp_assert(rows() == values.rows() && cols() == values.cols());

    for (int row = 0; row < rows(); ++row) {
      for (int col = 0; col < cols(); ++col) {
        (*this)(row, col) = values(row, col);
      }
    }

    return *this;
  }

  /**
   * Sets block's internal values.
   *
   * @param values Eigen matrix of values.
   */
  template <typename Derived>
    requires std::same_as<typename Derived::Scalar, double>
  void set_value(const Eigen::MatrixBase<Derived>& values) {
    slp_assert(rows() == values.rows() && cols() == values.cols());

    for (int row = 0; row < rows(); ++row) {
      for (int col = 0; col < cols(); ++col) {
        (*this)(row, col).set_value(values(row, col));
      }
    }
  }

  /**
   * Assigns a VariableMatrix to the block.
   *
   * @param values VariableMatrix of values.
   * @return This VariableBlock.
   */
  VariableBlock<Mat>& operator=(const Mat& values) {
    slp_assert(rows() == values.rows() && cols() == values.cols());

    for (int row = 0; row < rows(); ++row) {
      for (int col = 0; col < cols(); ++col) {
        (*this)(row, col) = values(row, col);
      }
    }
    return *this;
  }

  /**
   * Assigns a VariableMatrix to the block.
   *
   * @param values VariableMatrix of values.
   * @return This VariableBlock.
   */
  VariableBlock<Mat>& operator=(Mat&& values) {
    slp_assert(rows() == values.rows() && cols() == values.cols());

    for (int row = 0; row < rows(); ++row) {
      for (int col = 0; col < cols(); ++col) {
        (*this)(row, col) = std::move(values(row, col));
      }
    }
    return *this;
  }

  /**
   * Returns a scalar subblock at the given row and column.
   *
   * @param row The scalar subblock's row.
   * @param col The scalar subblock's column.
   * @return A scalar subblock at the given row and column.
   */
  Variable& operator()(int row, int col)
    requires(!std::is_const_v<Mat>)
  {
    slp_assert(row >= 0 && row < rows());
    slp_assert(col >= 0 && col < cols());
    return (*m_mat)(m_row_slice.start + row * m_row_slice.step,
                    m_col_slice.start + col * m_col_slice.step);
  }

  /**
   * Returns a scalar subblock at the given row and column.
   *
   * @param row The scalar subblock's row.
   * @param col The scalar subblock's column.
   * @return A scalar subblock at the given row and column.
   */
  const Variable& operator()(int row, int col) const {
    slp_assert(row >= 0 && row < rows());
    slp_assert(col >= 0 && col < cols());
    return (*m_mat)(m_row_slice.start + row * m_row_slice.step,
                    m_col_slice.start + col * m_col_slice.step);
  }

  /**
   * Returns a scalar subblock at the given row.
   *
   * @param row The scalar subblock's row.
   * @return A scalar subblock at the given row.
   */
  Variable& operator[](int row)
    requires(!std::is_const_v<Mat>)
  {
    slp_assert(row >= 0 && row < rows() * cols());
    return (*this)(row / cols(), row % cols());
  }

  /**
   * Returns a scalar subblock at the given row.
   *
   * @param row The scalar subblock's row.
   * @return A scalar subblock at the given row.
   */
  const Variable& operator[](int row) const {
    slp_assert(row >= 0 && row < rows() * cols());
    return (*this)(row / cols(), row % cols());
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
  VariableBlock<Mat> block(int row_offset, int col_offset, int block_rows,
                           int block_cols) {
    slp_assert(row_offset >= 0 && row_offset <= rows());
    slp_assert(col_offset >= 0 && col_offset <= cols());
    slp_assert(block_rows >= 0 && block_rows <= rows() - row_offset);
    slp_assert(block_cols >= 0 && block_cols <= cols() - col_offset);
    return (*this)({row_offset, row_offset + block_rows, 1},
                   {col_offset, col_offset + block_cols, 1});
  }

  /**
   * Returns a block slice of the variable matrix.
   *
   * @param row_offset The row offset of the block selection.
   * @param col_offset The column offset of the block selection.
   * @param block_rows The number of rows in the block selection.
   * @param block_cols The number of columns in the block selection.
   * @return A block slice of the variable matrix.
   */
  const VariableBlock<const Mat> block(int row_offset, int col_offset,
                                       int block_rows, int block_cols) const {
    slp_assert(row_offset >= 0 && row_offset <= rows());
    slp_assert(col_offset >= 0 && col_offset <= cols());
    slp_assert(block_rows >= 0 && block_rows <= rows() - row_offset);
    slp_assert(block_cols >= 0 && block_cols <= cols() - col_offset);
    return (*this)({row_offset, row_offset + block_rows, 1},
                   {col_offset, col_offset + block_cols, 1});
  }

  /**
   * Returns a slice of the variable matrix.
   *
   * @param row_slice The row slice.
   * @param col_slice The column slice.
   * @return A slice of the variable matrix.
   */
  VariableBlock<Mat> operator()(Slice row_slice, Slice col_slice) {
    int row_slice_length = row_slice.adjust(m_row_slice_length);
    int col_slice_length = col_slice.adjust(m_col_slice_length);
    return VariableBlock{
        *m_mat,
        {m_row_slice.start + row_slice.start * m_row_slice.step,
         m_row_slice.start + row_slice.stop, m_row_slice.step * row_slice.step},
        row_slice_length,
        {m_col_slice.start + col_slice.start * m_col_slice.step,
         m_col_slice.start + col_slice.stop, m_col_slice.step * col_slice.step},
        col_slice_length};
  }

  /**
   * Returns a slice of the variable matrix.
   *
   * @param row_slice The row slice.
   * @param col_slice The column slice.
   * @return A slice of the variable matrix.
   */
  const VariableBlock<const Mat> operator()(Slice row_slice,
                                            Slice col_slice) const {
    int row_slice_length = row_slice.adjust(m_row_slice_length);
    int col_slice_length = col_slice.adjust(m_col_slice_length);
    return VariableBlock{
        *m_mat,
        {m_row_slice.start + row_slice.start * m_row_slice.step,
         m_row_slice.start + row_slice.stop, m_row_slice.step * row_slice.step},
        row_slice_length,
        {m_col_slice.start + col_slice.start * m_col_slice.step,
         m_col_slice.start + col_slice.stop, m_col_slice.step * col_slice.step},
        col_slice_length};
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
  VariableBlock<Mat> operator()(Slice row_slice, int row_slice_length,
                                Slice col_slice, int col_slice_length) {
    return VariableBlock{
        *m_mat,
        {m_row_slice.start + row_slice.start * m_row_slice.step,
         m_row_slice.start + row_slice.stop, m_row_slice.step * row_slice.step},
        row_slice_length,
        {m_col_slice.start + col_slice.start * m_col_slice.step,
         m_col_slice.start + col_slice.stop, m_col_slice.step * col_slice.step},
        col_slice_length};
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
  const VariableBlock<const Mat> operator()(Slice row_slice,
                                            int row_slice_length,
                                            Slice col_slice,
                                            int col_slice_length) const {
    return VariableBlock{
        *m_mat,
        {m_row_slice.start + row_slice.start * m_row_slice.step,
         m_row_slice.start + row_slice.stop, m_row_slice.step * row_slice.step},
        row_slice_length,
        {m_col_slice.start + col_slice.start * m_col_slice.step,
         m_col_slice.start + col_slice.stop, m_col_slice.step * col_slice.step},
        col_slice_length};
  }

  /**
   * Returns a segment of the variable vector.
   *
   * @param offset The offset of the segment.
   * @param length The length of the segment.
   * @return A segment of the variable vector.
   */
  VariableBlock<Mat> segment(int offset, int length) {
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
  const VariableBlock<Mat> segment(int offset, int length) const {
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
  VariableBlock<Mat> row(int row) {
    slp_assert(row >= 0 && row < rows());
    return block(row, 0, 1, cols());
  }

  /**
   * Returns a row slice of the variable matrix.
   *
   * @param row The row to slice.
   * @return A row slice of the variable matrix.
   */
  VariableBlock<const Mat> row(int row) const {
    slp_assert(row >= 0 && row < rows());
    return block(row, 0, 1, cols());
  }

  /**
   * Returns a column slice of the variable matrix.
   *
   * @param col The column to slice.
   * @return A column slice of the variable matrix.
   */
  VariableBlock<Mat> col(int col) {
    slp_assert(col >= 0 && col < cols());
    return block(0, col, rows(), 1);
  }

  /**
   * Returns a column slice of the variable matrix.
   *
   * @param col The column to slice.
   * @return A column slice of the variable matrix.
   */
  VariableBlock<const Mat> col(int col) const {
    slp_assert(col >= 0 && col < cols());
    return block(0, col, rows(), 1);
  }

  /**
   * Compound matrix multiplication-assignment operator.
   *
   * @param rhs Variable to multiply.
   * @return Result of multiplication.
   */
  VariableBlock<Mat>& operator*=(const MatrixLike auto& rhs) {
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
   * Compound matrix multiplication-assignment operator.
   *
   * @param rhs Variable to multiply.
   * @return Result of multiplication.
   */
  VariableBlock<Mat>& operator*=(const ScalarLike auto& rhs) {
    for (int row = 0; row < rows(); ++row) {
      for (int col = 0; col < cols(); ++col) {
        (*this)(row, col) *= rhs;
      }
    }

    return *this;
  }

  /**
   * Compound matrix division-assignment operator.
   *
   * @param rhs Variable to divide.
   * @return Result of division.
   */
  VariableBlock<Mat>& operator/=(const MatrixLike auto& rhs) {
    slp_assert(rhs.rows() == 1 && rhs.cols() == 1);

    for (int row = 0; row < rows(); ++row) {
      for (int col = 0; col < cols(); ++col) {
        (*this)(row, col) /= rhs(0, 0);
      }
    }

    return *this;
  }

  /**
   * Compound matrix division-assignment operator.
   *
   * @param rhs Variable to divide.
   * @return Result of division.
   */
  VariableBlock<Mat>& operator/=(const ScalarLike auto& rhs) {
    for (int row = 0; row < rows(); ++row) {
      for (int col = 0; col < cols(); ++col) {
        (*this)(row, col) /= rhs;
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
  VariableBlock<Mat>& operator+=(const MatrixLike auto& rhs) {
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
  VariableBlock<Mat>& operator+=(const ScalarLike auto& rhs) {
    slp_assert(rows() == 1 && cols() == 1);

    for (int row = 0; row < rows(); ++row) {
      for (int col = 0; col < cols(); ++col) {
        (*this)(row, col) += rhs;
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
  VariableBlock<Mat>& operator-=(const MatrixLike auto& rhs) {
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
  VariableBlock<Mat>& operator-=(const ScalarLike auto& rhs) {
    slp_assert(rows() == 1 && cols() == 1);

    for (int row = 0; row < rows(); ++row) {
      for (int col = 0; col < cols(); ++col) {
        (*this)(row, col) -= rhs;
      }
    }

    return *this;
  }

  /**
   * Implicit conversion operator from 1x1 VariableBlock to Variable.
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
  std::remove_cv_t<Mat> T() const {
    std::remove_cv_t<Mat> result{Mat::empty, cols(), rows()};

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
  int rows() const { return m_row_slice_length; }

  /**
   * Returns the number of columns in the matrix.
   *
   * @return The number of columns in the matrix.
   */
  int cols() const { return m_col_slice_length; }

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
    return (*m_mat)(m_row_slice.start + row * m_row_slice.step,
                    m_col_slice.start + col * m_col_slice.step)
        .value();
  }

  /**
   * Returns a row of the variable column vector.
   *
   * @param index The index of the element to return.
   * @return A row of the variable column vector.
   */
  double value(int index) {
    slp_assert(index >= 0 && index < rows() * cols());
    return value(index / cols(), index % cols());
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
  std::remove_cv_t<Mat> cwise_transform(
      function_ref<Variable(const Variable& x)> unary_op) const {
    std::remove_cv_t<Mat> result{Mat::empty, rows(), cols()};

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

    constexpr iterator(VariableBlock<Mat>* mat, int index) noexcept
        : m_mat{mat}, m_index{index} {}

    constexpr iterator& operator++() noexcept {
      ++m_index;
      return *this;
    }

    constexpr iterator operator++(int) noexcept {
      iterator retval = *this;
      ++(*this);
      return retval;
    }

    constexpr bool operator==(const iterator&) const noexcept = default;

    constexpr reference operator*() const noexcept { return (*m_mat)[m_index]; }

   private:
    VariableBlock<Mat>* m_mat = nullptr;
    int m_index = 0;
  };

  class const_iterator {
   public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = Variable;
    using difference_type = std::ptrdiff_t;
    using pointer = Variable*;
    using const_reference = const Variable&;

    constexpr const_iterator() noexcept = default;

    constexpr const_iterator(const VariableBlock<Mat>* mat, int index) noexcept
        : m_mat{mat}, m_index{index} {}

    constexpr const_iterator& operator++() noexcept {
      ++m_index;
      return *this;
    }

    constexpr const_iterator operator++(int) noexcept {
      const_iterator retval = *this;
      ++(*this);
      return retval;
    }

    constexpr bool operator==(const const_iterator&) const noexcept = default;

    constexpr const_reference operator*() const noexcept {
      return (*m_mat)[m_index];
    }

   private:
    const VariableBlock<Mat>* m_mat = nullptr;
    int m_index = 0;
  };

#endif  // DOXYGEN_SHOULD_SKIP_THIS

  /**
   * Returns begin iterator.
   *
   * @return Begin iterator.
   */
  iterator begin() { return iterator(this, 0); }

  /**
   * Returns end iterator.
   *
   * @return End iterator.
   */
  iterator end() { return iterator(this, rows() * cols()); }

  /**
   * Returns begin iterator.
   *
   * @return Begin iterator.
   */
  const_iterator begin() const { return const_iterator(this, 0); }

  /**
   * Returns end iterator.
   *
   * @return End iterator.
   */
  const_iterator end() const { return const_iterator(this, rows() * cols()); }

  /**
   * Returns begin iterator.
   *
   * @return Begin iterator.
   */
  const_iterator cbegin() const { return const_iterator(this, 0); }

  /**
   * Returns end iterator.
   *
   * @return End iterator.
   */
  const_iterator cend() const { return const_iterator(this, rows() * cols()); }

  /**
   * Returns number of elements in matrix.
   *
   * @return Number of elements in matrix.
   */
  size_t size() const { return rows() * cols(); }

 private:
  Mat* m_mat = nullptr;

  Slice m_row_slice;
  int m_row_slice_length = 0;

  Slice m_col_slice;
  int m_col_slice_length = 0;
};

}  // namespace slp
