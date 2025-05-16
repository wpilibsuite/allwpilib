// Copyright (c) Sleipnir contributors

#pragma once

#include <Eigen/Core>

// See docs/algorithms.md#Works_cited for citation definitions

namespace slp {

/**
 * Applies fraction-to-the-boundary rule to a variable and its iterate, then
 * returns a fraction of the iterate step size within (0, 1].
 *
 * @param x The variable.
 * @param p The iterate on the variable.
 * @param τ Fraction-to-the-boundary rule scaling factor within (0, 1].
 * @return Fraction of the iterate step size within (0, 1].
 */
inline double fraction_to_the_boundary_rule(
    const Eigen::Ref<const Eigen::VectorXd>& x,
    const Eigen::Ref<const Eigen::VectorXd>& p, double τ) {
  // α = max(α ∈ (0, 1] : x + αp ≥ (1 − τ)x)
  //
  // where x and τ are positive.
  //
  // x + αp ≥ (1 − τ)x
  // x + αp ≥ x − τx
  // αp ≥ −τx
  //
  // If the inequality is false, p < 0 and α is too big. Find the largest value
  // of α that makes the inequality true.
  //
  // α = −τ/p x
  double α = 1.0;
  for (int i = 0; i < x.rows(); ++i) {
    if (α * p(i) < -τ * x(i)) {
      α = -τ / p(i) * x(i);
    }
  }

  return α;
}

}  // namespace slp
