// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <algorithm>
#include <cmath>
#include <limits>

#include <Eigen/Core>

namespace wpi::math::detail {

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

inline Eigen::VectorXi BalanceMatrixPowerOfTwo(
    const Eigen::Ref<const Eigen::MatrixXd>& matrix) {
  const int n = matrix.rows();
  Eigen::VectorXi scales = Eigen::VectorXi::Zero(n);

  if (n == 0 || matrix.cols() != n) {
    return scales;
  }

  Eigen::MatrixXd work = matrix.cwiseAbs();

  constexpr double kRadix = 2.0;
  constexpr double kFactor = 0.95;
  constexpr int kMinScaleExponent =
      std::numeric_limits<double>::min_exponent +
      std::numeric_limits<double>::digits - 2;
  constexpr int kMaxScaleExponent = -kMinScaleExponent;

  const double sfmin1 =
      std::numeric_limits<double>::min() / std::numeric_limits<double>::epsilon();
  const double sfmin2 = sfmin1 * kRadix;
  const double sfmax2 = 1.0 / sfmin2;

  bool changed = true;
  while (changed) {
    changed = false;

    for (int i = 0; i < n; ++i) {
      double c = work.col(i).stableNorm();
      double r = work.row(i).stableNorm();
      double ca = work.col(i).maxCoeff();
      double ra = work.row(i).maxCoeff();

      if (c == 0.0 || r == 0.0 ||
          !std::isfinite(c + ca + r + ra)) {
        continue;
      }

      double g = r / kRadix;
      double f = 1.0;
      double s = c + r;
      int exponent = 0;

      while (c < g && std::max({f, c, ca}) < sfmax2 &&
             std::min({r, g, ra}) > sfmin2) {
        f *= kRadix;
        c *= kRadix;
        ca *= kRadix;
        r /= kRadix;
        g /= kRadix;
        ra /= kRadix;
        ++exponent;
      }

      g = c / kRadix;

      while (g >= r && std::max(r, ra) < sfmax2 &&
             std::min({f, c, g, ca}) > sfmin2) {
        f /= kRadix;
        c /= kRadix;
        g /= kRadix;
        ca /= kRadix;
        r *= kRadix;
        ra *= kRadix;
        --exponent;
      }

      if (exponent == 0 || c + r >= kFactor * s) {
        continue;
      }

      int newScale = scales[i] + exponent;
      if (newScale <= kMinScaleExponent ||
          newScale >= kMaxScaleExponent) {
        continue;
      }

      work.row(i) /= f;
      work.col(i) *= f;
      scales[i] = newScale;
      changed = true;
    }
  }

  return scales;
}

}  // namespace wpi::math::detail
