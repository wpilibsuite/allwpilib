// Copyright (c) Sleipnir contributors

#pragma once

#include <Eigen/Core>

namespace slp {

/// Represents the inertia of a matrix (the number of positive, negative, and
/// zero eigenvalues).
class Inertia {
 public:
  /// The number of positive eigenvalues.
  int positive = 0;
  /// The number of negative eigenvalues.
  int negative = 0;
  /// The number of zero eigenvalues.
  int zero = 0;

  constexpr Inertia() = default;

  /// Constructs Inertia with the given number of positive, negative, and zero
  /// eigenvalues.
  ///
  /// @param positive The number of positive eigenvalues.
  /// @param negative The number of negative eigenvalues.
  /// @param zero The number of zero eigenvalues.
  constexpr Inertia(int positive, int negative, int zero)
      : positive{positive}, negative{negative}, zero{zero} {}

  /// Constructs Inertia from the D matrix of an LDLT decomposition
  /// (see https://en.wikipedia.org/wiki/Sylvester's_law_of_inertia).
  ///
  /// @tparam Scalar Scalar type.
  /// @param D The D matrix of an LDLT decomposition in vector form.
  template <typename Scalar>
  explicit Inertia(const Eigen::Vector<Scalar, Eigen::Dynamic>& D) {
    for (const auto& elem : D) {
      if (elem > Scalar(0)) {
        ++positive;
      } else if (elem < Scalar(0)) {
        ++negative;
      } else {
        ++zero;
      }
    }
  }

  /// Constructs Inertia from the D matrix of an LDLT decomposition
  /// (see https://en.wikipedia.org/wiki/Sylvester's_law_of_inertia).
  ///
  /// @tparam Scalar Scalar type.
  /// @param D The D matrix of an LDLT decomposition in vector form.
  template <typename Scalar>
  explicit Inertia(
      const Eigen::Diagonal<
          const Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic>>& D) {
    for (const auto& elem : D) {
      if (elem > Scalar(0)) {
        ++positive;
      } else if (elem < Scalar(0)) {
        ++negative;
      } else {
        ++zero;
      }
    }
  }

  /// Inertia equality operator.
  ///
  /// @return True if Inertia is equal.
  bool operator==(const Inertia&) const = default;
};

}  // namespace slp
