// Copyright (c) Sleipnir contributors

#pragma once

#include <algorithm>
#include <cmath>
#include <limits>

#include <Eigen/Core>
#include <Eigen/SparseCore>
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
  explicit constexpr FilterEntry(Scalar cost,
                                 Scalar constraint_violation = Scalar(0))
      : cost{cost}, constraint_violation{constraint_violation} {}

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

  /// Returns true if this filter entry is dominated by another.
  ///
  /// @param entry The other entry.
  /// @return True if this filter entry is dominated by another.
  constexpr bool dominated_by(const FilterEntry<Scalar>& entry) const {
    return entry.cost <= cost &&
           entry.constraint_violation <= constraint_violation;
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
  /// Type alias for dense vector.
  using DenseVector = Eigen::Vector<Scalar, Eigen::Dynamic>;

  /// Type alias for sparse vector.
  using SparseVector = Eigen::SparseVector<Scalar>;

  /// The minimum constraint violation
  Scalar min_constraint_violation;

  /// The maximum constraint violation
  Scalar max_constraint_violation;

  /// Constructs an empty filter.
  ///
  /// @param initial_constraint_violation The optimization problem's initial
  ///     constraint violation.
  explicit constexpr Filter(Scalar initial_constraint_violation = Scalar(0)) {
    using std::max;

    min_constraint_violation =
        Scalar(1e-4) * max(Scalar(1), initial_constraint_violation);
    max_constraint_violation =
        Scalar(1e4) * max(Scalar(1), initial_constraint_violation);
  }

  /// Resets the filter.
  void reset() {
    m_filter.clear();
    m_last_rejection_due_to_filter = false;
  }

  /// Returns true if the given trial entry is acceptable to the filter.
  ///
  /// @param current_entry The entry corresponding to the current iterate.
  /// @param trial_entry The entry corresponding to the trial iterate.
  /// @param p_x Decision variable primal step.
  /// @param g Cost function gradient.
  /// @param α The step size (0, 1].
  /// @return True if the given entry is acceptable to the filter.
  bool try_add(const FilterEntry<Scalar>& current_entry,
               const FilterEntry<Scalar>& trial_entry, const DenseVector& p_x,
               const SparseVector& g, Scalar α) {
    using std::isfinite;
    using std::pow;

    // Reject steps with nonfinite cost or constraint violation above maximum
    if (!isfinite(trial_entry.cost) ||
        trial_entry.constraint_violation > max_constraint_violation) {
      return false;
    }

    Scalar g_p_x = g.transpose() * p_x;

    // Switching condition
    constexpr Scalar s_ϕ(2.3);
    constexpr Scalar s_θ(1.1);
    bool switching_condition =
        g_p_x < Scalar(0) &&
        α * pow(-g_p_x, s_ϕ) > pow(current_entry.constraint_violation, s_θ);

    // Armijo condition
    constexpr Scalar η_ϕ(1e-8);
    bool armijo_condition =
        trial_entry.cost <= current_entry.cost + η_ϕ * α * g_p_x;

    // Sufficient decrease condition
    //
    // See equation (2.13) of [4].
    Scalar ϕ = pow(α, Scalar(1.5));
    bool sufficient_decrease =
        trial_entry.cost <=
            current_entry.cost -
                ϕ * γ_cost * current_entry.constraint_violation ||
        trial_entry.constraint_violation <=
            (Scalar(1) - ϕ * γ_constraint) * current_entry.constraint_violation;

    // If constraint violation is below threshold and switching condition is
    // true, check Armijo condition for step rejection. Otherwise, check
    // sufficient decrease condition.
    if (current_entry.constraint_violation <= min_constraint_violation &&
        switching_condition) {
      if (!armijo_condition) {
        m_last_rejection_due_to_filter = false;
        return false;
      }
    } else if (!sufficient_decrease) {
      m_last_rejection_due_to_filter = false;
      return false;
    }

    // Reject steps in filter (i.e., dominated by any filter entry)
    if (in_filter(trial_entry)) {
      m_last_rejection_due_to_filter = true;
      return false;
    }

    // Augment filter with accepted iterate if switching condition or Armijo
    // condition are false
    if (!switching_condition || !armijo_condition) {
      add(FilterEntry{
          current_entry.cost - ϕ * γ_cost * current_entry.constraint_violation,
          (Scalar(1) - ϕ * γ_constraint) * current_entry.constraint_violation});
    }

    return true;
  }

  /// Returns true if the most recent trial entry rejection was due to the
  /// filter.
  ///
  /// @return True if the most recent trial entry rejection was due to the
  /// filter.
  bool last_rejection_due_to_filter() const {
    return m_last_rejection_due_to_filter;
  }

 private:
  static constexpr Scalar γ_cost{1e-8};
  static constexpr Scalar γ_constraint{1e-5};

  gch::small_vector<FilterEntry<Scalar>> m_filter;

  bool m_last_rejection_due_to_filter = false;

  /// Adds a new entry to the filter.
  ///
  /// @param entry The entry to add to the filter.
  void add(const FilterEntry<Scalar>& entry) {
    // Remove dominated entries
    erase_if(m_filter,
             [&](const auto& elem) { return elem.dominated_by(entry); });

    m_filter.push_back(entry);
  }

  /// Returns true if the given entry is in the filter.
  ///
  /// @param entry The entry.
  /// @return True if the given entry is in the filter.
  bool in_filter(const FilterEntry<Scalar>& entry) const {
    // An entry is in the filter if it's dominated by any filter entry
    return std::any_of(m_filter.begin(), m_filter.end(), [&](const auto& elem) {
      return entry.dominated_by(elem);
    });
  }
};

}  // namespace slp
