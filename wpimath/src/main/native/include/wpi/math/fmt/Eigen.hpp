// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <concepts>
#include <format>

#include <Eigen/Core>
#include <Eigen/SparseCore>

// FIXME: Doxygen gives internal inconsistency errors:

//! @cond Doxygen_Suppress

template <class R>
  requires std::derived_from<R, Eigen::DenseBase<R>> ||
               std::derived_from<R, Eigen::SparseCompressedBase<R>>
constexpr std::range_format std::format_kind<R> = std::range_format::disabled;

// MSVC won't compile without these
template <typename Scalar, int Rows, int Cols, int Options, int MaxRows,
          int MaxCols>
constexpr std::range_format std::format_kind<
    Eigen::Matrix<Scalar, Rows, Cols, Options, MaxRows, MaxCols>> =
    std::range_format::disabled;
template <typename Scalar, int Rows, int Cols, int Options, int MaxRows,
          int MaxCols>
constexpr std::range_format std::format_kind<
    Eigen::Array<Scalar, Rows, Cols, Options, MaxRows, MaxCols>> =
    std::range_format::disabled;

/**
 * Formatter for classes derived from Eigen::DenseBase<Derived> or
 * Eigen::SparseCompressedBase<Derived>.
 */
template <typename Derived, typename CharT>
  requires std::derived_from<Derived, Eigen::DenseBase<Derived>> ||
           std::derived_from<Derived, Eigen::SparseCompressedBase<Derived>>
struct std::formatter<Derived, CharT> {
  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx) {
    return m_underlying.parse(ctx);
  }

  template <typename FmtContext>
  auto format(const Derived& mat, FmtContext& ctx) const {
    auto out = ctx.out();

    for (int row = 0; row < mat.rows(); ++row) {
      for (int col = 0; col < mat.cols(); ++col) {
        out = std::format_to(out, "  ");
        out = m_underlying.format(mat.coeff(row, col), ctx);
      }

      if (row < mat.rows() - 1) {
        out = std::format_to(out, "\n");
      }
    }

    return out;
  }

 private:
  std::formatter<typename Derived::Scalar, CharT> m_underlying;
};
//! @endcond
