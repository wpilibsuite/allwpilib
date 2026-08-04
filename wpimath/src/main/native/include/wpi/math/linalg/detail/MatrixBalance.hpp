// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <algorithm>
#include <cmath>
#include <limits>

#include <Eigen/Core>

namespace wpi::math::detail {

/**
 * Divides an integer by two and rounds halves to the nearest even integer.
 *
 * This is used when projecting separate row and column balance exponents onto a
 * single structured scale exponent.
 *
 * @param value The integer to divide.
 * @return value / 2 rounded to the nearest even integer.
 */
inline int RoundHalfToEvenDiv2(int value) {
  int quotient = value / 2;
  int remainder = value % 2;

  if (remainder == 0 || quotient % 2 == 0) {
    return quotient;
  }

  if (value > 0) {
    return quotient + 1;
  } else {
    return quotient - 1;
  }
}

/**
 * Computes diagonal power-of-two balancing exponents for a square matrix.
 *
 * The returned vector s describes the similarity transform
 *   balanced = diag(2^-s) matrix diag(2^s).
 *
 * The algorithm greedily scales one row/column pair at a time to make its row
 * and column norms closer. If the input isn't square, is empty, has a zero row
 * or column, or has nonfinite values in a row/column being considered, the
 * corresponding scale exponents remain zero.
 *
 * The matrix entries themselves are not modified by this function. Powers of
 * two are used so applying the transform with std::ldexp() avoids rounding from
 * the scale factors.
 *
 * @param matrix The matrix to balance.
 * @return The power-of-two scale exponents for each row/column.
 */
inline Eigen::VectorXi BalanceMatrixPowerOfTwo(
    const Eigen::Ref<const Eigen::MatrixXd>& matrix) {
  const int n = matrix.rows();
  Eigen::VectorXi scales = Eigen::VectorXi::Zero(n);

  if (n == 0 || matrix.cols() != n) {
    return scales;
  }

  Eigen::MatrixXd work = matrix.cwiseAbs();

  constexpr double kScaleRadix = 2.0;
  constexpr double kRequiredNormReductionRatio = 0.95;
  constexpr int kMinScaleExponent = std::numeric_limits<double>::min_exponent +
                                    std::numeric_limits<double>::digits - 2;
  constexpr int kMaxScaleExponent = -kMinScaleExponent;

  const double scaleUnderflowThreshold = std::numeric_limits<double>::min() /
                                         std::numeric_limits<double>::epsilon();
  // Keep trial scaling away from underflow and overflow while estimating
  // whether another power-of-two step would help.
  const double safeScaleMin = scaleUnderflowThreshold * kScaleRadix;
  const double safeScaleMax = 1.0 / safeScaleMin;

  bool changed = true;
  while (changed) {
    changed = false;

    for (int i = 0; i < n; ++i) {
      double colNorm = work.col(i).stableNorm();
      double rowNorm = work.row(i).stableNorm();
      double colMax = work.col(i).maxCoeff();
      double rowMax = work.row(i).maxCoeff();

      if (colNorm == 0.0 || rowNorm == 0.0 ||
          !std::isfinite(colNorm + colMax + rowNorm + rowMax)) {
        continue;
      }

      double candidateScale = 1.0;
      double originalNormSum = colNorm + rowNorm;
      int candidateExponent = 0;

      // Increasing the exponent scales row i down and column i up.
      double rowNormScaleDownThreshold = rowNorm / kScaleRadix;
      while (colNorm < rowNormScaleDownThreshold &&
             std::max({candidateScale, colNorm, colMax}) < safeScaleMax &&
             std::min({rowNorm, rowNormScaleDownThreshold, rowMax}) >
                 safeScaleMin) {
        candidateScale *= kScaleRadix;
        colNorm *= kScaleRadix;
        colMax *= kScaleRadix;
        rowNorm /= kScaleRadix;
        rowNormScaleDownThreshold /= kScaleRadix;
        rowMax /= kScaleRadix;
        ++candidateExponent;
      }

      // Decreasing the exponent scales row i up and column i down.
      double colNormScaleDownThreshold = colNorm / kScaleRadix;

      while (colNormScaleDownThreshold >= rowNorm &&
             std::max(rowNorm, rowMax) < safeScaleMax &&
             std::min({candidateScale, colNorm, colNormScaleDownThreshold,
                       colMax}) > safeScaleMin) {
        candidateScale /= kScaleRadix;
        colNorm /= kScaleRadix;
        colNormScaleDownThreshold /= kScaleRadix;
        colMax /= kScaleRadix;
        rowNorm *= kScaleRadix;
        rowMax *= kScaleRadix;
        --candidateExponent;
      }

      if (candidateExponent == 0 ||
          colNorm + rowNorm >= kRequiredNormReductionRatio * originalNormSum) {
        continue;
      }

      int newScale = scales[i] + candidateExponent;
      if (newScale <= kMinScaleExponent || newScale >= kMaxScaleExponent) {
        continue;
      }

      work.row(i) /= candidateScale;
      work.col(i) *= candidateScale;
      scales[i] = newScale;
      changed = true;
    }
  }

  return scales;
}

}  // namespace wpi::math::detail
