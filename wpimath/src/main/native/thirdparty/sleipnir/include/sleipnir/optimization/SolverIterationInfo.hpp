// Copyright (c) Sleipnir contributors

#pragma once

#include <Eigen/Core>
#include <Eigen/SparseCore>

namespace sleipnir {

/**
 * Solver iteration information exposed to a user callback.
 */
struct SolverIterationInfo {
  /// The solver iteration.
  int iteration;

  /// The decision variables.
  const Eigen::VectorXd& x;

  /// The inequality constraint slack variables.
  const Eigen::VectorXd& s;

  /// The gradient of the cost function.
  const Eigen::SparseVector<double>& g;

  /// The Hessian of the Lagrangian.
  const Eigen::SparseMatrix<double>& H;

  /// The equality constraint Jacobian.
  const Eigen::SparseMatrix<double>& A_e;

  /// The inequality constraint Jacobian.
  const Eigen::SparseMatrix<double>& A_i;
};

}  // namespace sleipnir
