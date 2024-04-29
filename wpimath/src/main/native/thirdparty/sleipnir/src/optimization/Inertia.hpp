// Copyright (c) Sleipnir contributors

#pragma once

#include <cstddef>

#include "sleipnir/util/Concepts.hpp"

namespace sleipnir {

/**
 * Represents the inertia of a matrix (the number of positive, negative, and
 * zero eigenvalues).
 */
class Inertia {
 public:
  size_t positive = 0;
  size_t negative = 0;
  size_t zero = 0;

  constexpr Inertia() = default;

  /**
   * Constructs the Inertia type with the given number of positive, negative,
   * and zero eigenvalues.
   *
   * @param positive The number of positive eigenvalues.
   * @param negative The number of negative eigenvalues.
   * @param zero The number of zero eigenvalues.
   */
  constexpr Inertia(size_t positive, size_t negative, size_t zero)
      : positive{positive}, negative{negative}, zero{zero} {}

  /**
   * Constructs the Inertia type with the inertia of the given LDLT
   * decomposition.
   *
   * @tparam Solver Eigen sparse linear system solver.
   * @param solver The LDLT decomposition of which to compute the inertia.
   */
  template <EigenSolver Solver>
  explicit Inertia(const Solver& solver) {
    const auto& D = solver.vectorD();
    for (int row = 0; row < D.rows(); ++row) {
      if (D(row) > 0.0) {
        ++positive;
      } else if (D(row) < 0.0) {
        ++negative;
      } else {
        ++zero;
      }
    }
  }

  bool operator==(const Inertia&) const = default;
};

}  // namespace sleipnir
