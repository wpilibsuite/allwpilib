// Copyright (c) Sleipnir contributors

#pragma once

#include <Eigen/Core>
#include <Eigen/SparseCore>

// See docs/algorithms.md#Works_cited for citation definitions

namespace slp {

/**
 * Returns true if the problem's equality constraints are locally infeasible.
 *
 * @param A_e The problem's equality constraint Jacobian Aₑ(x) evaluated at the
 *   current iterate.
 * @param c_e The problem's equality constraints cₑ(x) evaluated at the current
 *   iterate.
 */
inline bool is_equality_locally_infeasible(
    const Eigen::SparseMatrix<double>& A_e, const Eigen::VectorXd& c_e) {
  // The equality constraints are locally infeasible if
  //
  //   Aₑᵀcₑ → 0
  //   ‖cₑ‖ > ε
  //
  // See "Infeasibility detection" in section 6 of [3].
  return A_e.rows() > 0 && (A_e.transpose() * c_e).norm() < 1e-6 &&
         c_e.norm() > 1e-2;
}

/**
 * Returns true if the problem's inequality constraints are locally infeasible.
 *
 * @param A_i The problem's inequality constraint Jacobian Aᵢ(x) evaluated at
 *   the current iterate.
 * @param c_i The problem's inequality constraints cᵢ(x) evaluated at the
 *   current iterate.
 */
inline bool is_inequality_locally_infeasible(
    const Eigen::SparseMatrix<double>& A_i, const Eigen::VectorXd& c_i) {
  // The inequality constraints are locally infeasible if
  //
  //   Aᵢᵀcᵢ⁺ → 0
  //   ‖cᵢ⁺‖ > ε
  //
  // where cᵢ⁺ = min(cᵢ, 0).
  //
  // See "Infeasibility detection" in section 6 of [3].
  //
  // cᵢ⁺ is used instead of cᵢ⁻ from the paper to follow the convention that
  // feasible inequality constraints are ≥ 0.
  if (A_i.rows() > 0) {
    Eigen::VectorXd c_i_plus = c_i.cwiseMin(0.0);
    if ((A_i.transpose() * c_i_plus).norm() < 1e-6 && c_i_plus.norm() > 1e-6) {
      return true;
    }
  }

  return false;
}

}  // namespace slp
