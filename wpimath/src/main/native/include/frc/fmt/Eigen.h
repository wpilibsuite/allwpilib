// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <fmt/format.h>

#include "Eigen/Core"
#include "Eigen/SparseCore"

/**
 * Formatter for Eigen::Matrix<double, Rows, Cols>.
 *
 * @tparam Rows Number of rows.
 * @tparam Cols Number of columns.
 * @tparam Args Defaulted template arguments to Eigen::Matrix<>.
 */
template <int Rows, int Cols, int... Args>
struct fmt::formatter<Eigen::Matrix<double, Rows, Cols, Args...>> {
  /**
   * Storage for format specifier.
   */
  char presentation = 'f';

  /**
   * Format string parser.
   *
   * @param ctx Format string context.
   */
  constexpr auto parse(fmt::format_parse_context& ctx) {
    auto it = ctx.begin(), end = ctx.end();
    if (it != end && (*it == 'f' || *it == 'e')) {
      presentation = *it++;
    }

    if (it != end && *it != '}') {
      throw fmt::format_error("invalid format");
    }

    return it;
  }

  /**
   * Writes out a formatted matrix.
   *
   * @tparam FormatContext Format string context type.
   * @param mat Matrix to format.
   * @param ctx Format string context.
   */
  template <typename FormatContext>
  auto format(const Eigen::Matrix<double, Rows, Cols, Args...>& mat,
              FormatContext& ctx) {
    auto out = ctx.out();
    for (int i = 0; i < mat.rows(); ++i) {
      for (int j = 0; j < mat.cols(); ++j) {
        out = fmt::format_to(out, "  {:f}", mat(i, j));
      }

      if (i < mat.rows() - 1) {
        out = fmt::format_to(out, "\n");
      }
    }

    return out;
  }
};

/**
 * Formatter for Eigen::SparseMatrix<double>.
 *
 * @tparam Options Union of bit flags controlling the storage scheme.
 * @tparam StorageIndex The type of the indices.
 */
template <int Options, typename StorageIndex>
struct fmt::formatter<Eigen::SparseMatrix<double, Options, StorageIndex>> {
  /**
   * Storage for format specifier.
   */
  char presentation = 'f';

  /**
   * Format string parser.
   *
   * @param ctx Format string context.
   */
  constexpr auto parse(fmt::format_parse_context& ctx) {
    auto it = ctx.begin(), end = ctx.end();
    if (it != end && (*it == 'f' || *it == 'e')) {
      presentation = *it++;
    }

    if (it != end && *it != '}') {
      throw fmt::format_error("invalid format");
    }

    return it;
  }

  /**
   * Writes out a formatted matrix.
   *
   * @tparam FormatContext Format string context type.
   * @param mat Matrix to format.
   * @param ctx Format string context.
   */
  template <typename FormatContext>
  auto format(const Eigen::SparseMatrix<double, Options, StorageIndex>& mat,
              FormatContext& ctx) {
    auto out = ctx.out();
    for (int i = 0; i < mat.rows(); ++i) {
      for (int j = 0; j < mat.cols(); ++j) {
        out = fmt::format_to(out, "  {:f}", mat.coeff(i, j));
      }

      if (i < mat.rows() - 1) {
        out = fmt::format_to(out, "\n");
      }
    }

    return out;
  }
};
