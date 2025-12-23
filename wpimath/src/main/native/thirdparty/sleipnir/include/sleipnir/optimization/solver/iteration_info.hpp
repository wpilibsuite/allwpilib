// Copyright (c) Sleipnir contributors

#pragma once

#include <Eigen/Core>
#include <Eigen/SparseCore>

namespace slp {

/// Solver iteration information exposed to an iteration callback.
///
/// @tparam Scalar Scalar type.
template <typename Scalar>
struct IterationInfo {
  /// The solver iteration.
  int iteration;

  /// The decision variables.
  const Eigen::Vector<Scalar, Eigen::Dynamic>& x;

  /// The gradient of the cost function.
  const Eigen::SparseVector<Scalar>& g;

  /// The Hessian of the Lagrangian.
  const Eigen::SparseMatrix<Scalar>& H;

  /// The equality constraint Jacobian.
  const Eigen::SparseMatrix<Scalar>& A_e;

  /// The inequality constraint Jacobian.
  const Eigen::SparseMatrix<Scalar>& A_i;
};

}  // namespace slp
