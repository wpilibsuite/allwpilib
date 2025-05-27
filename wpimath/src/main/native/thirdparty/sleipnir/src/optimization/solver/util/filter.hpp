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

/**
 * Filter entry consisting of cost and constraint violation.
 */
struct FilterEntry {
  /// The cost function's value
  double cost = 0.0;

  /// The constraint violation
  double constraint_violation = 0.0;

  constexpr FilterEntry() = default;

  /**
   * Constructs a FilterEntry.
   *
   * @param cost The cost function's value.
   * @param constraint_violation The constraint violation.
   */
  constexpr FilterEntry(double cost, double constraint_violation)
      : cost{cost}, constraint_violation{constraint_violation} {}

  /**
   * Constructs a Newton's method filter entry.
   *
   * @param f The cost function value.
   */
  explicit FilterEntry(double f) : FilterEntry{f, 0.0} {}

  /**
   * Constructs a Sequential Quadratic Programming filter entry.
   *
   * @param f The cost function value.
   * @param c_e The equality constraint values (nonzero means violation).
   */
  FilterEntry(double f, const Eigen::VectorXd& c_e)
      : FilterEntry{f, c_e.lpNorm<1>()} {}

  /**
   * Constructs an interior-point method filter entry.
   *
   * @param f The cost function value.
   * @param s The inequality constraint slack variables.
   * @param c_e The equality constraint values (nonzero means violation).
   * @param c_i The inequality constraint values (negative means violation).
   * @param μ The barrier parameter.
   */
  FilterEntry(double f, Eigen::VectorXd& s, const Eigen::VectorXd& c_e,
              const Eigen::VectorXd& c_i, double μ)
      : FilterEntry{f - μ * s.array().log().sum(),
                    c_e.lpNorm<1>() + (c_i - s).lpNorm<1>()} {}
};

/**
 * Step filter.
 *
 * See the section on filters in chapter 15 of [1].
 */
class Filter {
 public:
  double max_constraint_violation = 1e4;

  /**
   * Constructs an empty filter.
   */
  Filter() {
    // Initial filter entry rejects constraint violations above max
    m_filter.emplace_back(std::numeric_limits<double>::infinity(),
                          max_constraint_violation);
  }

  /**
   * Resets the filter.
   */
  void reset() {
    m_filter.clear();

    // Initial filter entry rejects constraint violations above max
    m_filter.emplace_back(std::numeric_limits<double>::infinity(),
                          max_constraint_violation);
  }

  /**
   * Adds a new entry to the filter.
   *
   * @param entry The entry to add to the filter.
   */
  void add(const FilterEntry& entry) {
    // Remove dominated entries
    erase_if(m_filter, [&](const auto& elem) {
      return entry.cost <= elem.cost &&
             entry.constraint_violation <= elem.constraint_violation;
    });

    m_filter.push_back(entry);
  }

  /**
   * Adds a new entry to the filter.
   *
   * @param entry The entry to add to the filter.
   */
  void add(FilterEntry&& entry) {
    // Remove dominated entries
    erase_if(m_filter, [&](const auto& elem) {
      return entry.cost <= elem.cost &&
             entry.constraint_violation <= elem.constraint_violation;
    });

    m_filter.push_back(entry);
  }

  /**
   * Returns true if the given iterate is accepted by the filter.
   *
   * @param entry The entry to attempt adding to the filter.
   * @param α The step size (0, 1].
   */
  bool try_add(const FilterEntry& entry, double α) {
    if (is_acceptable(entry, α)) {
      add(entry);
      return true;
    } else {
      return false;
    }
  }

  /**
   * Returns true if the given iterate is accepted by the filter.
   *
   * @param entry The entry to attempt adding to the filter.
   * @param α The step size (0, 1].
   */
  bool try_add(FilterEntry&& entry, double α) {
    if (is_acceptable(entry, α)) {
      add(std::move(entry));
      return true;
    } else {
      return false;
    }
  }

  /**
   * Returns true if the given entry is acceptable to the filter.
   *
   * @param entry The entry to check.
   * @param α The step size (0, 1].
   */
  bool is_acceptable(const FilterEntry& entry, double α) {
    if (!std::isfinite(entry.cost) ||
        !std::isfinite(entry.constraint_violation)) {
      return false;
    }

    double ϕ = std::pow(α, 1.5);

    // If current filter entry is better than all prior ones in some respect,
    // accept it.
    //
    // See equation (2.13) of [4].
    return std::ranges::all_of(m_filter, [&](const auto& elem) {
      return entry.cost <= elem.cost - ϕ * γ_cost * elem.constraint_violation ||
             entry.constraint_violation <=
                 (1.0 - ϕ * γ_constraint) * elem.constraint_violation;
    });
  }

  /**
   * Returns the most recently added filter entry.
   *
   * @return The most recently added filter entry.
   */
  const FilterEntry& back() const { return m_filter.back(); }

 private:
  static constexpr double γ_cost = 1e-8;
  static constexpr double γ_constraint = 1e-5;

  gch::small_vector<FilterEntry> m_filter;
};

}  // namespace slp
