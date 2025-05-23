// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "frc/EigenCore.h"

namespace frc {

/**
 * Returns numerical Jacobian with respect to x for f(x).
 *
 * @tparam Rows Number of rows in result of f(x).
 * @tparam Cols Number of columns in result of f(x).
 * @param f     Vector-valued function from which to compute Jacobian.
 * @param x     Vector argument.
 */
template <int Rows, int Cols, typename F>
auto NumericalJacobian(F&& f, const Vectord<Cols>& x) {
  constexpr double kEpsilon = 1e-5;
  Matrixd<Rows, Cols> result;
  result.setZero();

  // It's more expensive, but +- epsilon will be more accurate
  for (int i = 0; i < Cols; ++i) {
    Vectord<Cols> dX_plus = x;
    dX_plus(i) += kEpsilon;
    Vectord<Cols> dX_minus = x;
    dX_minus(i) -= kEpsilon;
    result.col(i) = (f(dX_plus) - f(dX_minus)) / (kEpsilon * 2.0);
  }

  return result;
}

/**
 * Returns numerical Jacobian with respect to x for f(x).
 *
 * @param f     Vector-valued function from which to compute Jacobian.
 * @param x     Vector argument.
 */

template <typename F>
Eigen::MatrixXd NumericalJacobian(F&& f, const Eigen::VectorXd& x) {
  constexpr double kEpsilon = 1e-5;
  Eigen::MatrixXd result;

  // It's more expensive, but +- epsilon will be more accurate
  for (int i = 0; i < x.rows(); ++i) {
    Eigen::VectorXd dX_plus = x;
    dX_plus(i) += kEpsilon;
    Eigen::VectorXd dX_minus = x;
    dX_minus(i) -= kEpsilon;
    Eigen::VectorXd partialDerivative =
        (f(dX_plus) - f(dX_minus)) / (kEpsilon * 2.0);
    if (i == 0) {
      result.resize(partialDerivative.rows(), x.rows());
      result.setZero();
    }
    result.col(i) = partialDerivative;
  }

  return result;
}

/**
 * Returns numerical Jacobian with respect to x for f(x, u, ...).
 *
 * @tparam Rows    Number of rows in result of f(x, u, ...).
 * @tparam States  Number of rows in x.
 * @tparam Inputs  Number of rows in u.
 * @tparam F       Function object type.
 * @tparam Args... Types of remaining arguments to f(x, u, ...).
 * @param f        Vector-valued function from which to compute Jacobian.
 * @param x        State vector.
 * @param u        Input vector.
 * @param args     Remaining arguments to f(x, u, ...).
 */
template <int Rows, int States, int Inputs, typename F, typename... Args>
auto NumericalJacobianX(F&& f, const Vectord<States>& x,
                        const Vectord<Inputs>& u, Args&&... args) {
  return NumericalJacobian<Rows, States>(
      [&](const Vectord<States>& x) { return f(x, u, args...); }, x);
}

/**
 * Returns numerical Jacobian with respect to x for f(x, u, ...).
 *
 * @tparam F       Function object type.
 * @tparam Args... Types of remaining arguments to f(x, u, ...).
 * @param f        Vector-valued function from which to compute Jacobian.
 * @param x        State vector.
 * @param u        Input vector.
 * @param args     Remaining arguments to f(x, u, ...).
 */
template <typename F, typename... Args>
auto NumericalJacobianX(F&& f, const Eigen::VectorXd& x,
                        const Eigen::VectorXd& u, Args&&... args) {
  return NumericalJacobian(
      [&](const Eigen::VectorXd& x) { return f(x, u, args...); }, x);
}

/**
 * Returns numerical Jacobian with respect to u for f(x, u, ...).
 *
 * @tparam Rows    Number of rows in result of f(x, u, ...).
 * @tparam States  Number of rows in x.
 * @tparam Inputs  Number of rows in u.
 * @tparam F       Function object type.
 * @tparam Args... Types of remaining arguments to f(x, u, ...).
 * @param f        Vector-valued function from which to compute Jacobian.
 * @param x        State vector.
 * @param u        Input vector.
 * @param args     Remaining arguments to f(x, u, ...).
 */
template <int Rows, int States, int Inputs, typename F, typename... Args>
auto NumericalJacobianU(F&& f, const Vectord<States>& x,
                        const Vectord<Inputs>& u, Args&&... args) {
  return NumericalJacobian<Rows, Inputs>(
      [&](const Vectord<Inputs>& u) { return f(x, u, args...); }, u);
}

/**
 * Returns numerical Jacobian with respect to u for f(x, u, ...).
 *
 * @tparam F       Function object type.
 * @tparam Args... Types of remaining arguments to f(x, u, ...).
 * @param f        Vector-valued function from which to compute Jacobian.
 * @param x        State vector.
 * @param u        Input vector.
 * @param args     Remaining arguments to f(x, u, ...).
 */
template <typename F, typename... Args>
auto NumericalJacobianU(F&& f, const Eigen::VectorXd& x,
                        const Eigen::VectorXd& u, Args&&... args) {
  return NumericalJacobian(
      [&](const Eigen::VectorXd& u) { return f(x, u, args...); }, u);
}

}  // namespace frc
