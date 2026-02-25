// Copyright (c) Sleipnir contributors

#pragma once

#include <algorithm>
#include <cmath>
#include <limits>
#include <utility>

#include <Eigen/Core>
#include <gch/small_vector.hpp>

// See docs/algorithms.md#Works_cited for citation definitions.

namespace slp {

/// Filter entry consisting of cost and constraint violation.
///
/// @tparam Scalar Scalar type.
template <typename Scalar>
struct FilterEntry {
  /// Type alias for dense vector.
  using DenseVector = Eigen::Vector<Scalar, Eigen::Dynamic>;

  /// The cost function's value
  Scalar cost{0};

  /// The constraint violation
  Scalar constraint_violation{0};

  constexpr FilterEntry() = default;

  /// Constructs a FilterEntry.
  ///
  /// @param cost The cost function's value.
  /// @param constraint_violation The constraint violation.
  constexpr FilterEntry(Scalar cost, Scalar constraint_violation)
      : cost{cost}, constraint_violation{constraint_violation} {}

  /// Constructs a Newton's method filter entry.
  ///
  /// @param f The cost function value.
  explicit FilterEntry(Scalar f) : FilterEntry{f, Scalar(0)} {}

  /// Constructs a Sequential Quadratic Programming filter entry.
  ///
  /// @param f The cost function value.
  /// @param c_e The equality constraint values (nonzero means violation).
  FilterEntry(Scalar f, const DenseVector& c_e)
      : FilterEntry{f, c_e.template lpNorm<1>()} {}

  /// Constructs an interior-point method filter entry.
  ///
  /// @param f The cost function value.
  /// @param s The inequality constraint slack variables.
  /// @param c_e The equality constraint values (nonzero means violation).
  /// @param c_i The inequality constraint values (negative means violation).
  /// @param μ The barrier parameter.
  FilterEntry(Scalar f, DenseVector& s, const DenseVector& c_e,
              const DenseVector& c_i, Scalar μ)
      : FilterEntry{f - μ * s.array().log().sum(),
                    c_e.template lpNorm<1>() + (c_i - s).template lpNorm<1>()} {
  }
};

/// Step filter.
///
/// See the section on filters in chapter 15 of [1].
///
/// @tparam Scalar Scalar type.
template <typename Scalar>
class Filter {
 public:
  /// The maximum constraint violation
  Scalar max_constraint_violation{1e4};

  /// Constructs an empty filter.
  Filter() {
    // Initial filter entry rejects constraint violations above max
    m_filter.emplace_back(std::numeric_limits<Scalar>::infinity(),
                          max_constraint_violation);
  }

  /// Resets the filter.
  void reset() {
    m_filter.clear();

    // Initial filter entry rejects constraint violations above max
    m_filter.emplace_back(std::numeric_limits<Scalar>::infinity(),
                          max_constraint_violation);
  }

  /// Adds a new entry to the filter.
  ///
  /// @param entry The entry to add to the filter.
  void add(const FilterEntry<Scalar>& entry) {
    // Remove dominated entries
    erase_if(m_filter, [&](const auto& elem) {
      return entry.cost <= elem.cost &&
             entry.constraint_violation <= elem.constraint_violation;
    });

    m_filter.push_back(entry);
  }

  /// Adds a new entry to the filter.
  ///
  /// @param entry The entry to add to the filter.
  void add(FilterEntry<Scalar>&& entry) {
    // Remove dominated entries
    erase_if(m_filter, [&](const auto& elem) {
      return entry.cost <= elem.cost &&
             entry.constraint_violation <= elem.constraint_violation;
    });

    m_filter.push_back(entry);
  }

  /// Returns true if the given iterate is accepted by the filter.
  ///
  /// @param entry The entry to attempt adding to the filter.
  /// @param α The step size (0, 1].
  /// @return True if the given iterate is accepted by the filter.
  bool try_add(const FilterEntry<Scalar>& entry, Scalar α) {
    if (is_acceptable(entry, α)) {
      add(entry);
      return true;
    } else {
      return false;
    }
  }

  /// Returns true if the given iterate is accepted by the filter.
  ///
  /// @param entry The entry to attempt adding to the filter.
  /// @param α The step size (0, 1].
  /// @return True if the given iterate is accepted by the filter.
  bool try_add(FilterEntry<Scalar>&& entry, Scalar α) {
    if (is_acceptable(entry, α)) {
      add(std::move(entry));
      return true;
    } else {
      return false;
    }
  }

  /// Returns true if the given entry is acceptable to the filter.
  ///
  /// @param entry The entry to check.
  /// @param α The step size (0, 1].
  /// @return True if the given entry is acceptable to the filter.
  bool is_acceptable(const FilterEntry<Scalar>& entry, Scalar α) {
    using std::isfinite;
    using std::pow;

    if (!isfinite(entry.cost) || !isfinite(entry.constraint_violation)) {
      return false;
    }

    Scalar ϕ = pow(α, Scalar(1.5));

    // If current filter entry is better than all prior ones in some respect,
    // accept it.
    //
    // See equation (2.13) of [4].
    return std::ranges::all_of(m_filter, [&](const auto& elem) {
      return entry.cost <= elem.cost - ϕ * γ_cost * elem.constraint_violation ||
             entry.constraint_violation <=
                 (Scalar(1) - ϕ * γ_constraint) * elem.constraint_violation;
    });
  }

  /// Returns the most recently added filter entry.
  ///
  /// @return The most recently added filter entry.
  const FilterEntry<Scalar>& back() const { return m_filter.back(); }

 private:
  static constexpr Scalar γ_cost{1e-8};
  static constexpr Scalar γ_constraint{1e-5};

  gch::small_vector<FilterEntry<Scalar>> m_filter;
};

}  // namespace slp
