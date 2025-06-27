// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <concepts>
#include <initializer_list>
#include <type_traits>

#include <Eigen/Core>
#include <gcem.hpp>

namespace frc {

template <typename T>
concept EigenMatrixLike = std::derived_from<T, Eigen::MatrixBase<T>>;

/**
 * Compile-time wrapper for Eigen::Matrix.
 *
 * @tparam Rows Rows of matrix.
 * @tparam Cols Columns of matrix.
 */
template <typename Scalar, int Rows, int Cols>
class ct_matrix {
 public:
  constexpr ct_matrix() = default;

  /**
   * Constructs a scalar VariableMatrix from a nested list of Variables.
   *
   * @param list The nested list of Variables.
   */
  constexpr ct_matrix(  // NOLINT (runtime/explicit)
      std::initializer_list<std::initializer_list<Scalar>> list)
      : m_storage{list} {}

  template <typename Derived>
    requires std::derived_from<Derived, Eigen::MatrixBase<Derived>>
  // NOLINTNEXTLINE (runtime/explicit)
  constexpr ct_matrix(const Derived& mat) : m_storage{mat} {}

  /**
   * Returns number of rows.
   *
   * @return Number of rows.
   */
  constexpr int rows() const { return m_storage.rows(); }

  /**
   * Returns number of columns.
   *
   * @return Number of columns.
   */
  constexpr int cols() const { return m_storage.cols(); }

  /**
   * Returns reference to matrix element.
   *
   * @param row Row index.
   * @param col Column index.
   */
  constexpr const Scalar& operator()(int row, int col) const {
    return m_storage(row, col);
  }

  /**
   * Returns reference to matrix element.
   *
   * @param row Row index.
   * @param col Column index.
   */
  constexpr Scalar& operator()(int row, int col) { return m_storage(row, col); }

  /**
   * Returns reference to matrix element.
   *
   * @param index Index.
   */
  constexpr const Scalar& operator()(int index) const
    requires(Rows == 1 || Cols == 1)
  {
    return m_storage(index);
  }

  /**
   * Returns reference to matrix element.
   *
   * @param index Index.
   */
  constexpr Scalar& operator()(int index)
    requires(Rows == 1 || Cols == 1)
  {
    return m_storage(index);
  }

  /**
   * Constexpr version of Eigen's scalar multiplication operator.
   *
   * @param lhs LHS scalar.
   * @param rhs RHS matrix.
   * @return Result of multiplication.
   */
  friend constexpr ct_matrix<Scalar, Rows, Cols> operator*(
      Scalar lhs, const ct_matrix<Scalar, Rows, Cols>& rhs) {
    if (std::is_constant_evaluated()) {
      ct_matrix<Scalar, Rows, Cols> result;

      for (int i = 0; i < rhs.rows(); ++i) {
        for (int j = 0; j < rhs.cols(); ++j) {
          result(i, j) = lhs * rhs(i, j);
        }
      }

      return result;
    } else {
      return lhs * rhs.m_storage;
    }
  }

  /**
   * Constexpr version of Eigen's matrix multiplication operator.
   *
   * @tparam Cols2 Columns of RHS matrix.
   * @param lhs LHS matrix.
   * @param rhs RHS matrix.
   * @return Result of multiplication.
   */
  template <int Cols2>
  friend constexpr ct_matrix<Scalar, Rows, Cols2> operator*(
      const ct_matrix<Scalar, Rows, Cols>& lhs,
      const ct_matrix<Scalar, Rows, Cols2>& rhs) {
    if (std::is_constant_evaluated()) {
      ct_matrix<Scalar, Rows, Cols2> result;

      for (int i = 0; i < lhs.rows(); ++i) {
        for (int j = 0; j < rhs.cols(); ++j) {
          Scalar sum = 0.0;
          for (int k = 0; k < lhs.cols(); ++k) {
            sum += lhs(i, k) * rhs(k, j);
          }
          result(i, j) = sum;
        }
      }

      return result;
    } else {
      return lhs.m_storage * rhs.storage();
    }
  }

  /**
   * Constexpr version of Eigen's matrix addition operator.
   *
   * @param lhs LHS matrix.
   * @param rhs RHS matrix.
   * @return Result of addition.
   */
  friend constexpr ct_matrix<Scalar, Rows, Cols> operator+(
      const ct_matrix<Scalar, Rows, Cols>& lhs,
      const ct_matrix<Scalar, Rows, Cols>& rhs) {
    if (std::is_constant_evaluated()) {
      ct_matrix<Scalar, Rows, Cols> result;

      for (int row = 0; row < rhs.rows(); ++row) {
        for (int col = 0; col < rhs.cols(); ++col) {
          result(row, col) = lhs(row, col) + rhs(row, col);
        }
      }

      return result;
    } else {
      return lhs.m_storage + rhs.m_storage;
    }
  }

  /**
   * Constexpr version of Eigen's matrix subtraction operator.
   *
   * @param lhs LHS matrix.
   * @param rhs RHS matrix.
   * @return Result of subtraction.
   */
  friend constexpr ct_matrix<Scalar, Rows, Cols> operator-(
      const ct_matrix<Scalar, Rows, Cols>& lhs,
      const ct_matrix<Scalar, Rows, Cols>& rhs) {
    if (std::is_constant_evaluated()) {
      ct_matrix<Scalar, Rows, Cols> result;

      for (int row = 0; row < rhs.rows(); ++row) {
        for (int col = 0; col < rhs.cols(); ++col) {
          result(row, col) = lhs(row, col) - rhs(row, col);
        }
      }

      return result;
    } else {
      return lhs.m_storage - rhs.m_storage;
    }
  }

  /**
   * Constexpr version of Eigen's transpose member function.
   *
   * @return Transpose of matrix.
   */
  constexpr ct_matrix<Scalar, Cols, Rows> transpose() const {
    if (std::is_constant_evaluated()) {
      ct_matrix<Scalar, Cols, Rows> result;

      for (int row = 0; row < rows(); ++row) {
        for (int col = 0; col < cols(); ++col) {
          result(col, row) = (*this)(row, col);
        }
      }

      return result;
    } else {
      return m_storage.transpose().eval();
    }
  }

  /**
   * Constexpr version of Eigen's identity function.
   *
   * @return Identity matrix of the specified size.
   */
  static constexpr ct_matrix<Scalar, Rows, Cols> Identity()
    requires(Rows != Eigen::Dynamic && Cols != Eigen::Dynamic)
  {
    if (std::is_constant_evaluated()) {
      ct_matrix<Scalar, Rows, Cols> result;

      for (int row = 0; row < Rows; ++row) {
        for (int col = 0; col < Cols; ++col) {
          if (row == col) {
            result(row, row) = 1.0;
          } else {
            result(row, col) = 0.0;
          }
        }
      }

      return result;
    } else {
      return Eigen::Matrix<Scalar, Rows, Cols>::Identity();
    }
  }

  /**
   * Constexpr version of Eigen's vector dot member function.
   *
   * @tparam RhsRows Rows of RHS vector.
   * @tparam RhsCols Columns of RHS vector.
   * @param rhs RHS vector.
   * @return Dot product of two vectors.
   */
  template <int RhsRows, int RhsCols>
    requires(Rows == 1 || Cols == 1) && (RhsRows == 1 || RhsCols == 1) &&
            (Rows * Cols == RhsRows * RhsCols)
  constexpr Scalar dot(const ct_matrix<Scalar, RhsRows, RhsCols>& rhs) const {
    if (std::is_constant_evaluated()) {
      Scalar sum = 0.0;

      for (int index = 0; index < rows() * rhs.cols(); ++index) {
        sum += (*this)(index)*rhs(index);
      }

      return sum;
    } else {
      return m_storage.dot(rhs.storage());
    }
  }

  /**
   * Constexpr version of Eigen's norm member function.
   *
   * @return Norm of matrix.
   */
  constexpr Scalar norm() const {
    if (std::is_constant_evaluated()) {
      Scalar sum = 0.0;

      for (int row = 0; row < rows(); ++row) {
        for (int col = 0; col < cols(); ++col) {
          sum += (*this)(row, col) * (*this)(row, col);
        }
      }

      return gcem::sqrt(sum);
    } else {
      return m_storage.norm();
    }
  }

  /**
   * Constexpr version of Eigen's 3D vector cross member function.
   *
   * @param rhs RHS vector.
   * @return Cross product of two vectors.
   */
  constexpr ct_matrix<Scalar, 3, 1> cross(const ct_matrix<Scalar, 3, 1>& rhs)
    requires(Rows == 3 && Cols == 1)
  {
    return Eigen::Vector3d{{(*this)(1) * rhs(2) - rhs(1) * (*this)(2),
                            rhs(0) * (*this)(2) - (*this)(0) * rhs(2),
                            (*this)(0) * rhs(1) - rhs(0) * (*this)(1)}};
  }

  /**
   * Constexpr version of Eigen's 2x2 matrix determinant member function.
   *
   * @return Determinant of matrix.
   */
  constexpr Scalar determinant() const
    requires(Rows == 2 && Cols == 2)
  {
    // |a  b|
    // |c  d| = ad - bc
    Scalar a = (*this)(0, 0);
    Scalar b = (*this)(0, 1);
    Scalar c = (*this)(1, 0);
    Scalar d = (*this)(1, 1);
    return a * d - b * c;
  }

  /**
   * Constexpr version of Eigen's 3x3 matrix determinant member function.
   *
   * @return Determinant of matrix.
   */
  constexpr Scalar determinant() const
    requires(Rows == 3 && Cols == 3)
  {
    // |a  b  c|
    // |d  e  f| = aei + bfg + cdh - ceg - bdi - afh
    // |g  h  i|
    Scalar a = (*this)(0, 0);
    Scalar b = (*this)(0, 1);
    Scalar c = (*this)(0, 2);
    Scalar d = (*this)(1, 0);
    Scalar e = (*this)(1, 1);
    Scalar f = (*this)(1, 2);
    Scalar g = (*this)(2, 0);
    Scalar h = (*this)(2, 1);
    Scalar i = (*this)(2, 2);
    return a * e * i + b * f * g + c * d * h - c * e * g - b * d * i -
           a * f * h;
  }

  /**
   * Returns the internal Eigen matrix.
   *
   * @return The internal Eigen matrix.
   */
  constexpr const Eigen::Matrix<Scalar, Rows, Cols>& storage() const {
    return m_storage;
  }

  /**
   * Implicit cast to an Eigen matrix.
   */
  constexpr operator Eigen::Matrix<Scalar, Rows, Cols>() const {  // NOLINT
    return m_storage;
  }

 private:
  Eigen::Matrix<Scalar, Rows, Cols> m_storage;
};

template <typename Derived>
  requires std::derived_from<Derived, Eigen::MatrixBase<Derived>>
ct_matrix(const Derived&)
    -> ct_matrix<typename Derived::Scalar, Derived::RowsAtCompileTime,
                 Derived::ColsAtCompileTime>;

template <typename Scalar, int Rows>
using ct_vector = ct_matrix<Scalar, Rows, 1>;

template <typename Scalar, int Cols>
using ct_row_vector = ct_matrix<Scalar, 1, Cols>;

using ct_matrix2d = ct_matrix<double, 2, 2>;
using ct_matrix3d = ct_matrix<double, 3, 3>;
using ct_vector2d = ct_vector<double, 2>;
using ct_vector3d = ct_vector<double, 3>;

}  // namespace frc
