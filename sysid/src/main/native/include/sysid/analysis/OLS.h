// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <cstddef>
#include <tuple>
#include <vector>

#include <Eigen/Core>

namespace sysid {

/**
 * Performs ordinary least squares multiple regression on the provided data and
 * returns a vector of coefficients along with the r-squared (coefficient of
 * determination) and RMSE (stardard deviation of the residuals) of the fit.
 *
 * @param X The independent data in y = Xβ.
 * @param y The dependent data in y = Xβ.
 */
std::tuple<std::vector<double>, double, double> OLS(const Eigen::MatrixXd& X,
                                                    const Eigen::VectorXd& y);

}  // namespace sysid
