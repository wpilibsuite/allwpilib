// Copyright (c) Sleipnir contributors

#pragma once

#include <concepts>
#include <type_traits>
#include <utility>

#include <Eigen/Core>

#include "sleipnir/autodiff/Slice.hpp"
#include "sleipnir/autodiff/Variable.hpp"
#include "sleipnir/util/Assert.hpp"
#include "sleipnir/util/FunctionRef.hpp"

namespace sleipnir {

/**
 * A submatrix of autodiff variables with reference semantics.
 *
 * @tparam Mat The type of the matrix whose storage this class points to.
 */
template <typename Mat>
class VariableBlock {
 public:
  VariableBlock(const VariableBlock<Mat>& values) = default;

  /**
   * Assigns a VariableBlock to the block.
   *
   * @param values VariableBlock of values.
   */
  VariableBlock<Mat>& operator=(const VariableBlock<Mat>& values) {
    if (this == &values) {
      return *this;
    }

    if (m_mat == nullptr) {
      m_mat = values.m_mat;
      m_rowSlice = values.m_rowSlice;
      m_rowSliceLength = values.m_rowSliceLength;
      m_colSlice = values.m_colSlice;
      m_colSliceLength = values.m_colSliceLength;
    } else {
      Assert(Rows() == values.Rows());
      Assert(Cols() == values.Cols());

      for (int row = 0; row < Rows(); ++row) {
        for (int col = 0; col < Cols(); ++col) {
          (*this)(row, col) = values(row, col);
        }
      }
    }

    return *this;
  }

  VariableBlock(VariableBlock<Mat>&&) = default;

  /**
   * Assigns a VariableBlock to the block.
   *
   * @param values VariableBlock of values.
   */
  VariableBlock<Mat>& operator=(VariableBlock<Mat>&& values) {
    if (this == &values) {
      return *this;
    }

    if (m_mat == nullptr) {
      m_mat = values.m_mat;
      m_rowSlice = values.m_rowSlice;
      m_rowSliceLength = values.m_rowSliceLength;
      m_colSlice = values.m_colSlice;
      m_colSliceLength = values.m_colSliceLength;
    } else {
      Assert(Rows() == values.Rows());
      Assert(Cols() == values.Cols());

      for (int row = 0; row < Rows(); ++row) {
        for (int col = 0; col < Cols(); ++col) {
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
        m_rowSlice{0, mat.Rows(), 1},
        m_rowSliceLength{m_rowSlice.Adjust(mat.Rows())},
        m_colSlice{0, mat.Cols(), 1},
        m_colSliceLength{m_colSlice.Adjust(mat.Cols())} {}

  /**
   * Constructs a Variable block pointing to a subset of the given matrix.
   *
   * @param mat The matrix to which to point.
   * @param rowOffset The block's row offset.
   * @param colOffset The block's column offset.
   * @param blockRows The number of rows in the block.
   * @param blockCols The number of columns in the block.
   */
  VariableBlock(Mat& mat, int rowOffset, int colOffset, int blockRows,
                int blockCols)
      : m_mat{&mat},
        m_rowSlice{rowOffset, rowOffset + blockRows, 1},
        m_rowSliceLength{m_rowSlice.Adjust(mat.Rows())},
        m_colSlice{colOffset, colOffset + blockCols, 1},
        m_colSliceLength{m_colSlice.Adjust(mat.Cols())} {}

  /**
   * Constructs a Variable block pointing to a subset of the given matrix.
   *
   * Note that the slices are taken as is rather than adjusted.
   *
   * @param mat The matrix to which to point.
   * @param rowSlice The block's row slice.
   * @param rowSliceLength The block's row length.
   * @param colSlice The block's column slice.
   * @param colSliceLength The block's column length.
   */
  VariableBlock(Mat& mat, Slice rowSlice, int rowSliceLength, Slice colSlice,
                int colSliceLength)
      : m_mat{&mat},
        m_rowSlice{std::move(rowSlice)},
        m_rowSliceLength{rowSliceLength},
        m_colSlice{std::move(colSlice)},
        m_colSliceLength{colSliceLength} {}

  /**
   * Assigns a double to the block.
   *
   * This only works for blocks with one row and one column.
   */
  VariableBlock<Mat>& operator=(double value) {
    Assert(Rows() == 1 && Cols() == 1);

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
  void SetValue(double value) {
    Assert(Rows() == 1 && Cols() == 1);

    (*this)(0, 0).SetValue(value);
  }

  /**
   * Assigns an Eigen matrix to the block.
   *
   * @param values Eigen matrix of values to assign.
   */
  template <typename Derived>
  VariableBlock<Mat>& operator=(const Eigen::MatrixBase<Derived>& values) {
    Assert(Rows() == values.rows());
    Assert(Cols() == values.cols());

    for (int row = 0; row < Rows(); ++row) {
      for (int col = 0; col < Cols(); ++col) {
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
  void SetValue(const Eigen::MatrixBase<Derived>& values) {
    Assert(Rows() == values.rows());
    Assert(Cols() == values.cols());

    for (int row = 0; row < Rows(); ++row) {
      for (int col = 0; col < Cols(); ++col) {
        (*this)(row, col).SetValue(values(row, col));
      }
    }
  }

  /**
   * Assigns a VariableMatrix to the block.
   *
   * @param values VariableMatrix of values.
   */
  VariableBlock<Mat>& operator=(const Mat& values) {
    Assert(Rows() == values.Rows());
    Assert(Cols() == values.Cols());

    for (int row = 0; row < Rows(); ++row) {
      for (int col = 0; col < Cols(); ++col) {
        (*this)(row, col) = values(row, col);
      }
    }
    return *this;
  }

  /**
   * Assigns a VariableMatrix to the block.
   *
   * @param values VariableMatrix of values.
   */
  VariableBlock<Mat>& operator=(Mat&& values) {
    Assert(Rows() == values.Rows());
    Assert(Cols() == values.Cols());

    for (int row = 0; row < Rows(); ++row) {
      for (int col = 0; col < Cols(); ++col) {
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
   */
  Variable& operator()(int row, int col)
    requires(!std::is_const_v<Mat>)
  {
    Assert(row >= 0 && row < Rows());
    Assert(col >= 0 && col < Cols());
    return (*m_mat)(m_rowSlice.start + row * m_rowSlice.step,
                    m_colSlice.start + col * m_colSlice.step);
  }

  /**
   * Returns a scalar subblock at the given row and column.
   *
   * @param row The scalar subblock's row.
   * @param col The scalar subblock's column.
   */
  const Variable& operator()(int row, int col) const {
    Assert(row >= 0 && row < Rows());
    Assert(col >= 0 && col < Cols());
    return (*m_mat)(m_rowSlice.start + row * m_rowSlice.step,
                    m_colSlice.start + col * m_colSlice.step);
  }

  /**
   * Returns a scalar subblock at the given row.
   *
   * @param row The scalar subblock's row.
   */
  Variable& operator()(int row)
    requires(!std::is_const_v<Mat>)
  {
    Assert(row >= 0 && row < Rows() * Cols());
    return (*this)(row / Cols(), row % Cols());
  }

  /**
   * Returns a scalar subblock at the given row.
   *
   * @param row The scalar subblock's row.
   */
  const Variable& operator()(int row) const {
    Assert(row >= 0 && row < Rows() * Cols());
    return (*this)(row / Cols(), row % Cols());
  }

  /**
   * Returns a block of the variable matrix.
   *
   * @param rowOffset The row offset of the block selection.
   * @param colOffset The column offset of the block selection.
   * @param blockRows The number of rows in the block selection.
   * @param blockCols The number of columns in the block selection.
   */
  VariableBlock<Mat> Block(int rowOffset, int colOffset, int blockRows,
                           int blockCols) {
    Assert(rowOffset >= 0 && rowOffset <= Rows());
    Assert(colOffset >= 0 && colOffset <= Cols());
    Assert(blockRows >= 0 && blockRows <= Rows() - rowOffset);
    Assert(blockCols >= 0 && blockCols <= Cols() - colOffset);
    return (*this)({rowOffset, rowOffset + blockRows, 1},
                   {colOffset, colOffset + blockCols, 1});
  }

  /**
   * Returns a block slice of the variable matrix.
   *
   * @param rowOffset The row offset of the block selection.
   * @param colOffset The column offset of the block selection.
   * @param blockRows The number of rows in the block selection.
   * @param blockCols The number of columns in the block selection.
   */
  const VariableBlock<const Mat> Block(int rowOffset, int colOffset,
                                       int blockRows, int blockCols) const {
    Assert(rowOffset >= 0 && rowOffset <= Rows());
    Assert(colOffset >= 0 && colOffset <= Cols());
    Assert(blockRows >= 0 && blockRows <= Rows() - rowOffset);
    Assert(blockCols >= 0 && blockCols <= Cols() - colOffset);
    return (*this)({rowOffset, rowOffset + blockRows, 1},
                   {colOffset, colOffset + blockCols, 1});
  }

  /**
   * Returns a slice of the variable matrix.
   *
   * @param rowSlice The row slice.
   * @param colSlice The column slice.
   */
  VariableBlock<Mat> operator()(Slice rowSlice, Slice colSlice) {
    int rowSliceLength = rowSlice.Adjust(m_rowSliceLength);
    int colSliceLength = colSlice.Adjust(m_colSliceLength);
    return VariableBlock{
        *m_mat,
        {m_rowSlice.start + rowSlice.start * m_rowSlice.step,
         m_rowSlice.start + rowSlice.stop, m_rowSlice.step * rowSlice.step},
        rowSliceLength,
        {m_colSlice.start + colSlice.start * m_colSlice.step,
         m_colSlice.start + colSlice.stop, m_colSlice.step * colSlice.step},
        colSliceLength};
  }

  /**
   * Returns a slice of the variable matrix.
   *
   * @param rowSlice The row slice.
   * @param colSlice The column slice.
   */
  const VariableBlock<const Mat> operator()(Slice rowSlice,
                                            Slice colSlice) const {
    int rowSliceLength = rowSlice.Adjust(m_rowSliceLength);
    int colSliceLength = colSlice.Adjust(m_colSliceLength);
    return VariableBlock{
        *m_mat,
        {m_rowSlice.start + rowSlice.start * m_rowSlice.step,
         m_rowSlice.start + rowSlice.stop, m_rowSlice.step * rowSlice.step},
        rowSliceLength,
        {m_colSlice.start + colSlice.start * m_colSlice.step,
         m_colSlice.start + colSlice.stop, m_colSlice.step * colSlice.step},
        colSliceLength};
  }

  /**
   * Returns a slice of the variable matrix.
   *
   * The given slices aren't adjusted. This overload is for Python bindings
   * only.
   *
   * @param rowSlice The row slice.
   * @param rowSliceLength The row slice length.
   * @param colSlice The column slice.
   * @param colSliceLength The column slice length.
   */
  VariableBlock<Mat> operator()(Slice rowSlice, int rowSliceLength,
                                Slice colSlice, int colSliceLength) {
    return VariableBlock{
        *m_mat,
        {m_rowSlice.start + rowSlice.start * m_rowSlice.step,
         m_rowSlice.start + rowSlice.stop, m_rowSlice.step * rowSlice.step},
        rowSliceLength,
        {m_colSlice.start + colSlice.start * m_colSlice.step,
         m_colSlice.start + colSlice.stop, m_colSlice.step * colSlice.step},
        colSliceLength};
  }

  /**
   * Returns a slice of the variable matrix.
   *
   * The given slices aren't adjusted. This overload is for Python bindings
   * only.
   *
   * @param rowSlice The row slice.
   * @param rowSliceLength The row slice length.
   * @param colSlice The column slice.
   * @param colSliceLength The column slice length.
   */
  const VariableBlock<const Mat> operator()(Slice rowSlice, int rowSliceLength,
                                            Slice colSlice,
                                            int colSliceLength) const {
    return VariableBlock{
        *m_mat,
        {m_rowSlice.start + rowSlice.start * m_rowSlice.step,
         m_rowSlice.start + rowSlice.stop, m_rowSlice.step * rowSlice.step},
        rowSliceLength,
        {m_colSlice.start + colSlice.start * m_colSlice.step,
         m_colSlice.start + colSlice.stop, m_colSlice.step * colSlice.step},
        colSliceLength};
  }

  /**
   * Returns a segment of the variable vector.
   *
   * @param offset The offset of the segment.
   * @param length The length of the segment.
   */
  VariableBlock<Mat> Segment(int offset, int length) {
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
  const VariableBlock<Mat> Segment(int offset, int length) const {
    Assert(offset >= 0 && offset < Rows() * Cols());
    Assert(length >= 0 && length <= Rows() * Cols() - offset);
    return Block(offset, 0, length, 1);
  }

  /**
   * Returns a row slice of the variable matrix.
   *
   * @param row The row to slice.
   */
  VariableBlock<Mat> Row(int row) {
    Assert(row >= 0 && row < Rows());
    return Block(row, 0, 1, Cols());
  }

  /**
   * Returns a row slice of the variable matrix.
   *
   * @param row The row to slice.
   */
  VariableBlock<const Mat> Row(int row) const {
    Assert(row >= 0 && row < Rows());
    return Block(row, 0, 1, Cols());
  }

  /**
   * Returns a column slice of the variable matrix.
   *
   * @param col The column to slice.
   */
  VariableBlock<Mat> Col(int col) {
    Assert(col >= 0 && col < Cols());
    return Block(0, col, Rows(), 1);
  }

  /**
   * Returns a column slice of the variable matrix.
   *
   * @param col The column to slice.
   */
  VariableBlock<const Mat> Col(int col) const {
    Assert(col >= 0 && col < Cols());
    return Block(0, col, Rows(), 1);
  }

  /**
   * Compound matrix multiplication-assignment operator.
   *
   * @param rhs Variable to multiply.
   */
  VariableBlock<Mat>& operator*=(const VariableBlock<Mat>& rhs) {
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
   * Compound matrix multiplication-assignment operator (only enabled when lhs
   * is a scalar).
   *
   * @param rhs Variable to multiply.
   */
  VariableBlock& operator*=(double rhs) {
    for (int row = 0; row < Rows(); ++row) {
      for (int col = 0; col < Cols(); ++col) {
        (*this)(row, col) *= rhs;
      }
    }

    return *this;
  }

  /**
   * Compound matrix division-assignment operator (only enabled when rhs
   * is a scalar).
   *
   * @param rhs Variable to divide.
   */
  VariableBlock<Mat>& operator/=(const VariableBlock<Mat>& rhs) {
    Assert(rhs.Rows() == 1 && rhs.Cols() == 1);

    for (int row = 0; row < Rows(); ++row) {
      for (int col = 0; col < Cols(); ++col) {
        (*this)(row, col) /= rhs(0, 0);
      }
    }

    return *this;
  }

  /**
   * Compound matrix division-assignment operator (only enabled when rhs
   * is a scalar).
   *
   * @param rhs Variable to divide.
   */
  VariableBlock<Mat>& operator/=(double rhs) {
    for (int row = 0; row < Rows(); ++row) {
      for (int col = 0; col < Cols(); ++col) {
        (*this)(row, col) /= rhs;
      }
    }

    return *this;
  }

  /**
   * Compound addition-assignment operator.
   *
   * @param rhs Variable to add.
   */
  VariableBlock<Mat>& operator+=(const VariableBlock<Mat>& rhs) {
    for (int row = 0; row < Rows(); ++row) {
      for (int col = 0; col < Cols(); ++col) {
        (*this)(row, col) += rhs(row, col);
      }
    }

    return *this;
  }

  /**
   * Compound subtraction-assignment operator.
   *
   * @param rhs Variable to subtract.
   */
  VariableBlock<Mat>& operator-=(const VariableBlock<Mat>& rhs) {
    for (int row = 0; row < Rows(); ++row) {
      for (int col = 0; col < Cols(); ++col) {
        (*this)(row, col) -= rhs(row, col);
      }
    }

    return *this;
  }

  /**
   * Returns the transpose of the variable matrix.
   */
  std::remove_cv_t<Mat> T() const {
    std::remove_cv_t<Mat> result{Cols(), Rows()};

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
  int Rows() const { return m_rowSliceLength; }

  /**
   * Returns number of columns in the matrix.
   */
  int Cols() const { return m_colSliceLength; }

  /**
   * Returns an element of the variable matrix.
   *
   * @param row The row of the element to return.
   * @param col The column of the element to return.
   */
  double Value(int row, int col) {
    Assert(row >= 0 && row < Rows());
    Assert(col >= 0 && col < Cols());
    return (*m_mat)(m_rowSlice.start + row * m_rowSlice.step,
                    m_colSlice.start + col * m_colSlice.step)
        .Value();
  }

  /**
   * Returns a row of the variable column vector.
   *
   * @param index The index of the element to return.
   */
  double Value(int index) {
    Assert(index >= 0 && index < Rows() * Cols());
    return Value(index / Cols(), index % Cols());
  }

  /**
   * Returns the contents of the variable matrix.
   */
  Eigen::MatrixXd Value() {
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
  std::remove_cv_t<Mat> CwiseTransform(
      function_ref<Variable(const Variable& x)> unaryOp) const {
    std::remove_cv_t<Mat> result{Rows(), Cols()};

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

    iterator(VariableBlock<Mat>* mat, int row, int col)
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
    VariableBlock<Mat>* m_mat;
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

    const_iterator(const VariableBlock<Mat>* mat, int row, int col)
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
    const VariableBlock<Mat>* m_mat;
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
  size_t size() const { return Rows() * Cols(); }

 private:
  Mat* m_mat = nullptr;

  Slice m_rowSlice;
  int m_rowSliceLength = 0;

  Slice m_colSlice;
  int m_colSliceLength = 0;
};

}  // namespace sleipnir
