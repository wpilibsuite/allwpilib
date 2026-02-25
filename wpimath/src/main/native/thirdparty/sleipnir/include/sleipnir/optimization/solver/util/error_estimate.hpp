// Copyright (c) Sleipnir contributors

#pragma once

#include <algorithm>

#include <Eigen/Core>
#include <Eigen/SparseCore>

// See docs/algorithms.md#Works_cited for citation definitions

namespace slp {

/// Returns the error estimate using the KKT conditions for Newton's method.
///
/// @tparam Scalar Scalar type.
/// @param g Gradient of the cost function ∇f.
template <typename Scalar>
Scalar error_estimate(const Eigen::Vector<Scalar, Eigen::Dynamic>& g) {
  // Update the error estimate using the KKT conditions from equations (19.5a)
  // through (19.5d) of [1].
  //
  //   ∇f = 0

  return g.template lpNorm<Eigen::Infinity>();
}

/// Returns the error estimate using the KKT conditions for SQP.
///
/// @tparam Scalar Scalar type.
/// @param g Gradient of the cost function ∇f.
/// @param A_e The problem's equality constraint Jacobian Aₑ(x) evaluated at the
///     current iterate.
/// @param c_e The problem's equality constraints cₑ(x) evaluated at the current
///     iterate.
/// @param y Equality constraint dual variables.
template <typename Scalar>
Scalar error_estimate(const Eigen::Vector<Scalar, Eigen::Dynamic>& g,
                      const Eigen::SparseMatrix<Scalar>& A_e,
                      const Eigen::Vector<Scalar, Eigen::Dynamic>& c_e,
                      const Eigen::Vector<Scalar, Eigen::Dynamic>& y) {
  // Update the error estimate using the KKT conditions from equations (19.5a)
  // through (19.5d) of [1].
  //
  //   ∇f − Aₑᵀy = 0
  //   cₑ = 0
  //
  // The error tolerance is the max of the following infinity norms scaled by
  // s_d (see equation (5) of [2]).
  //
  //   ‖∇f − Aₑᵀy‖_∞ / s_d
  //   ‖cₑ‖_∞

  // s_d = max(sₘₐₓ, ‖y‖₁ / m) / sₘₐₓ
  constexpr Scalar s_max(100);
  Scalar s_d =
      std::max(s_max, y.template lpNorm<1>() / Scalar(y.rows())) / s_max;

  return std::max(
      {(g - A_e.transpose() * y).template lpNorm<Eigen::Infinity>() / s_d,
       c_e.template lpNorm<Eigen::Infinity>()});
}

/// Returns the error estimate using the KKT conditions for the interior-point
/// method.
///
/// @tparam Scalar Scalar type.
/// @param g Gradient of the cost function ∇f.
/// @param A_e The problem's equality constraint Jacobian Aₑ(x) evaluated at the
///     current iterate.
/// @param c_e The problem's equality constraints cₑ(x) evaluated at the current
///     iterate.
/// @param A_i The problem's inequality constraint Jacobian Aᵢ(x) evaluated at
///     the current iterate.
/// @param c_i The problem's inequality constraints cᵢ(x) evaluated at the
///     current iterate.
/// @param s Inequality constraint slack variables.
/// @param y Equality constraint dual variables.
/// @param z Inequality constraint dual variables.
/// @param μ Barrier parameter.
template <typename Scalar>
Scalar error_estimate(const Eigen::Vector<Scalar, Eigen::Dynamic>& g,
                      const Eigen::SparseMatrix<Scalar>& A_e,
                      const Eigen::Vector<Scalar, Eigen::Dynamic>& c_e,
                      const Eigen::SparseMatrix<Scalar>& A_i,
                      const Eigen::Vector<Scalar, Eigen::Dynamic>& c_i,
                      const Eigen::Vector<Scalar, Eigen::Dynamic>& s,
                      const Eigen::Vector<Scalar, Eigen::Dynamic>& y,
                      const Eigen::Vector<Scalar, Eigen::Dynamic>& z,
                      Scalar μ) {
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
  constexpr Scalar s_max(100);
  Scalar s_d =
      std::max(s_max, (y.template lpNorm<1>() + z.template lpNorm<1>()) /
                          Scalar(y.rows() + z.rows())) /
      s_max;

  // s_c = max(sₘₐₓ, ‖z‖₁ / n) / sₘₐₓ
  Scalar s_c =
      std::max(s_max, z.template lpNorm<1>() / Scalar(z.rows())) / s_max;

  const auto S = s.asDiagonal();
  const Eigen::Vector<Scalar, Eigen::Dynamic> μe =
      Eigen::Vector<Scalar, Eigen::Dynamic>::Constant(s.rows(), μ);

  return std::max({(g - A_e.transpose() * y - A_i.transpose() * z)
                           .template lpNorm<Eigen::Infinity>() /
                       s_d,
                   (S * z - μe).template lpNorm<Eigen::Infinity>() / s_c,
                   c_e.template lpNorm<Eigen::Infinity>(),
                   (c_i - s).template lpNorm<Eigen::Infinity>()});
}

}  // namespace slp
