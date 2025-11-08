// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "sysid/analysis/OLS.h"

#include <cassert>
#include <cmath>

#include <Eigen/Cholesky>

namespace sysid {

OLSResult OLS(const Eigen::MatrixXd& X, const Eigen::VectorXd& y) {
  assert(X.rows() == y.rows());

  // The linear regression model can be written as follows:
  //
  //   y = Xβ + ε
  //
  // where y is the dependent observed variable, X is the matrix of independent
  // variables, β is a vector of coefficients, and ε is a vector of residuals.
  //
  // We want to find the value of β that minimizes εᵀε.
  //
  //   ε = y − Xβ
  //   εᵀε = (y − Xβ)ᵀ(y − Xβ)
  //
  //   β̂ = argmin (y − Xβ)ᵀ(y − Xβ)
  //         β
  //
  // Take the partial derivative of the cost function with respect to β and set
  // it equal to zero, then solve for β̂ .
  //
  //   0 = −2Xᵀ(y − Xβ̂)
  //   0 = Xᵀ(y − Xβ̂)
  //   0 = Xᵀy − XᵀXβ̂
  //   XᵀXβ̂ = Xᵀy
  //   β̂  = (XᵀX)⁻¹Xᵀy

  // β = (XᵀX)⁻¹Xᵀy
  //
  // XᵀX is guaranteed to be symmetric positive definite, so an LLT
  // decomposition can be used.
  Eigen::MatrixXd β = (X.transpose() * X).llt().solve(X.transpose() * y);

  // Error sum of squares
  double SSE = (y - X * β).squaredNorm();

  // Sample size
  int n = X.rows();

  // Number of explanatory variables
  int p = β.rows();

  // Total sum of squares (total variation in y)
  //
  // From slide 24 of
  // http://www.stat.columbia.edu/~fwood/Teaching/w4315/Fall2009/lecture_11:
  //
  //   SSTO = yᵀy - 1/n yᵀJy
  //
  // where J is a matrix of ones.
  double SSTO =
      (y.transpose() * y - 1.0 / y.rows() * y.transpose() *
                               Eigen::MatrixXd::Ones(y.rows(), y.rows()) * y)
          .value();

  // R² or the coefficient of determination, which represents how much of the
  // total variation (variation in y) can be explained by the regression model
  double rSquared = 1.0 - SSE / SSTO;

  // Adjusted R²
  //
  //                       n − 1
  //   R̅² = 1 − (1 − R²) ---------
  //                     n − p − 1
  //
  // See https://en.wikipedia.org/wiki/Coefficient_of_determination#Adjusted_R2
  double adjRSquared = 1.0 - (1.0 - rSquared) * ((n - 1.0) / (n - p - 1.0));

  // Root-mean-square error
  double RMSE = std::sqrt(SSE / n);

  return {{β.data(), β.data() + β.size()}, adjRSquared, RMSE};
}

}  // namespace sysid
