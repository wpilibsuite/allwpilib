// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <concepts>

#include <Eigen/Core>
#include <Eigen/SparseCore>
#include <fmt/format.h>

// FIXME: Doxygen gives internal inconsistency errors:

//! @cond Doxygen_Suppress

/**
 * Formatter for classes derived from Eigen::DenseBase<Derived> or
 * Eigen::SparseCompressedBase<Derived>.
 */
template <typename Derived, typename CharT>
  requires std::derived_from<Derived, Eigen::DenseBase<Derived>> ||
           std::derived_from<Derived, Eigen::SparseCompressedBase<Derived>>
struct fmt::formatter<Derived, CharT> {
  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx) {
    return m_underlying.parse(ctx);
  }

  template <typename FmtContext>
  auto format(const Derived& mat, FmtContext& ctx) const {
    auto out = ctx.out();

    for (int row = 0; row < mat.rows(); ++row) {
      for (int col = 0; col < mat.cols(); ++col) {
        out = fmt::format_to(out, "  ");
        out = m_underlying.format(mat.coeff(row, col), ctx);
      }

      if (row < mat.rows() - 1) {
        out = fmt::format_to(out, "\n");
      }
    }

    return out;
  }

 private:
  fmt::formatter<typename Derived::Scalar, CharT> m_underlying;
};
//! @endcond
