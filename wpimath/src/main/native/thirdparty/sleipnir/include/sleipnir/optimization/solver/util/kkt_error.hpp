// Copyright (c) Sleipnir contributors

#pragma once

#include <Eigen/Core>
#include <Eigen/SparseCore>

// See docs/algorithms.md#Works_cited for citation definitions

namespace slp {

/// Returns the KKT error for Newton's method.
///
/// @tparam Scalar Scalar type.
/// @param g Gradient of the cost function ∇f.
template <typename Scalar>
Scalar kkt_error(const Eigen::Vector<Scalar, Eigen::Dynamic>& g) {
  // Compute the KKT error as the 1-norm of the KKT conditions from equations
  // (19.5a) through (19.5d) of [1].
  //
  //   ∇f = 0

  return g.template lpNorm<1>();
}

/// Returns the KKT error for Sequential Quadratic Programming.
///
/// @tparam Scalar Scalar type.
/// @param g Gradient of the cost function ∇f.
/// @param A_e The problem's equality constraint Jacobian Aₑ(x) evaluated at the
///     current iterate.
/// @param c_e The problem's equality constraints cₑ(x) evaluated at the current
///     iterate.
/// @param y Equality constraint dual variables.
template <typename Scalar>
Scalar kkt_error(const Eigen::Vector<Scalar, Eigen::Dynamic>& g,
                 const Eigen::SparseMatrix<Scalar>& A_e,
                 const Eigen::Vector<Scalar, Eigen::Dynamic>& c_e,
                 const Eigen::Vector<Scalar, Eigen::Dynamic>& y) {
  // Compute the KKT error as the 1-norm of the KKT conditions from equations
  // (19.5a) through (19.5d) of [1].
  //
  //   ∇f − Aₑᵀy = 0
  //   cₑ = 0

  return (g - A_e.transpose() * y).template lpNorm<1>() +
         c_e.template lpNorm<1>();
}

/// Returns the KKT error for the interior-point method.
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
Scalar kkt_error(const Eigen::Vector<Scalar, Eigen::Dynamic>& g,
                 const Eigen::SparseMatrix<Scalar>& A_e,
                 const Eigen::Vector<Scalar, Eigen::Dynamic>& c_e,
                 const Eigen::SparseMatrix<Scalar>& A_i,
                 const Eigen::Vector<Scalar, Eigen::Dynamic>& c_i,
                 const Eigen::Vector<Scalar, Eigen::Dynamic>& s,
                 const Eigen::Vector<Scalar, Eigen::Dynamic>& y,
                 const Eigen::Vector<Scalar, Eigen::Dynamic>& z, Scalar μ) {
  // Compute the KKT error as the 1-norm of the KKT conditions from equations
  // (19.5a) through (19.5d) of [1].
  //
  //   ∇f − Aₑᵀy − Aᵢᵀz = 0
  //   Sz − μe = 0
  //   cₑ = 0
  //   cᵢ − s = 0

  const auto S = s.asDiagonal();
  const Eigen::Vector<Scalar, Eigen::Dynamic> μe =
      Eigen::Vector<Scalar, Eigen::Dynamic>::Constant(s.rows(), μ);

  return (g - A_e.transpose() * y - A_i.transpose() * z).template lpNorm<1>() +
         (S * z - μe).template lpNorm<1>() + c_e.template lpNorm<1>() +
         (c_i - s).template lpNorm<1>();
}

}  // namespace slp
