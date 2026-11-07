// Copyright (c) Sleipnir contributors

#pragma once

#include <Eigen/Core>

// See docs/algorithms.md#Works_cited for citation definitions

namespace slp {

/// Applies fraction-to-the-boundary rule to a variable and its iterate, then
/// returns a fraction of the iterate step size within (0, 1].
///
/// @tparam Scalar Scalar type.
/// @param x The variable.
/// @param p The iterate on the variable.
/// @param τ Fraction-to-the-boundary rule scaling factor within (0, 1].
/// @return Fraction of the iterate step size within (0, 1].
template <typename Scalar>
Scalar fraction_to_the_boundary_rule(
    const Eigen::Vector<Scalar, Eigen::Dynamic>& x,
    const Eigen::Vector<Scalar, Eigen::Dynamic>& p, Scalar τ) {
  // The fraction-to-the-boundary rule is defined as:
  //
  //   α = max(α ∈ (0, 1] : x + αp ≥ (1 − τ)x)   (1)
  //
  // where x and τ are positive. Rearranging the inequality in (1) gives
  //
  //   x + αp ≥ (1 − τ)x
  //   x + αp ≥ x − τx
  //   αp ≥ −τx                                  (2)
  //
  // (2) is false if p < 0 and α is sufficiently large. Let p < 0.
  //
  //   αp ≥ −τx
  //   α ≤ −τxᵢ/pᵢ for i in range(x.rows())      (3)
  //
  // When (2) is false, find the largest α for which (3) is true.
  Scalar α(1);
  for (int i = 0; i < x.rows(); ++i) {
    if (α * p[i] < -τ * x[i]) {
      // α = −τx/p is (3)'s upper bound
      α = -τ * x[i] / p[i];
    }
  }

  return α;
}

}  // namespace slp
