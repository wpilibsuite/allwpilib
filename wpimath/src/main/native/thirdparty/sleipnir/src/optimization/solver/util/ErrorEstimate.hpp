// Copyright (c) Sleipnir contributors

#pragma once

#include <algorithm>

#include <Eigen/Core>
#include <Eigen/SparseCore>

// See docs/algorithms.md#Works_cited for citation definitions

namespace sleipnir {

/**
 * Returns the error estimate using the KKT conditions for the interior-point
 * method.
 *
 * @param g Gradient of the cost function ∇f.
 * @param A_e The problem's equality constraint Jacobian Aₑ(x) evaluated at the
 *   current iterate.
 * @param c_e The problem's equality constraints cₑ(x) evaluated at the current
 *   iterate.
 * @param A_i The problem's inequality constraint Jacobian Aᵢ(x) evaluated at
 *   the current iterate.
 * @param c_i The problem's inequality constraints cᵢ(x) evaluated at the
 *   current iterate.
 * @param s Inequality constraint slack variables.
 * @param y Equality constraint dual variables.
 * @param z Inequality constraint dual variables.
 * @param μ Barrier parameter.
 */
inline double ErrorEstimate(const Eigen::VectorXd& g,
                            const Eigen::SparseMatrix<double>& A_e,
                            const Eigen::VectorXd& c_e,
                            const Eigen::SparseMatrix<double>& A_i,
                            const Eigen::VectorXd& c_i,
                            const Eigen::VectorXd& s, const Eigen::VectorXd& y,
                            const Eigen::VectorXd& z, double μ) {
  int numEqualityConstraints = A_e.rows();
  int numInequalityConstraints = A_i.rows();

  // Update the error estimate using the KKT conditions from equations (19.5a)
  // through (19.5d) of [1].
  //
  //   ∇f − Aₑᵀy − Aᵢᵀz = 0
  //   Sz − μe = 0
  //   cₑ = 0
  //   cᵢ − s = 0
  //
  // The error tolerance is the max of the following infinity norms scaled by
  // s_d and s_c (see equation (5) of [2]).
  //
  //   ‖∇f − Aₑᵀy − Aᵢᵀz‖_∞ / s_d
  //   ‖Sz − μe‖_∞ / s_c
  //   ‖cₑ‖_∞
  //   ‖cᵢ − s‖_∞

  // s_d = max(sₘₐₓ, (‖y‖₁ + ‖z‖₁) / (m + n)) / sₘₐₓ
  constexpr double s_max = 100.0;
  double s_d =
      std::max(s_max, (y.lpNorm<1>() + z.lpNorm<1>()) /
                          (numEqualityConstraints + numInequalityConstraints)) /
      s_max;

  // s_c = max(sₘₐₓ, ‖z‖₁ / n) / sₘₐₓ
  double s_c =
      std::max(s_max, z.lpNorm<1>() / numInequalityConstraints) / s_max;

  const auto S = s.asDiagonal();
  const Eigen::VectorXd e = Eigen::VectorXd::Ones(s.rows());

  return std::max({(g - A_e.transpose() * y - A_i.transpose() * z)
                           .lpNorm<Eigen::Infinity>() /
                       s_d,
                   (S * z - μ * e).lpNorm<Eigen::Infinity>() / s_c,
                   c_e.lpNorm<Eigen::Infinity>(),
                   (c_i - s).lpNorm<Eigen::Infinity>()});
}

}  // namespace sleipnir
