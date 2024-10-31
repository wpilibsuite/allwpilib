// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <vector>

#include <Eigen/Core>

namespace sysid {

struct OLSResult {
  /// Regression coefficients.
  std::vector<double> coeffs;

  /// R² (coefficient of determination)
  double rSquared = 0.0;

  /// Root-mean-square error
  double rmse = 0.0;
};

/**
 * Performs ordinary least squares multiple regression on the provided data.
 *
 * @param X The independent data in y = Xβ.
 * @param y The dependent data in y = Xβ.
 */
OLSResult OLS(const Eigen::MatrixXd& X, const Eigen::VectorXd& y);

}  // namespace sysid
