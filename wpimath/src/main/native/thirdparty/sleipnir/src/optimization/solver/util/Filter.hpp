// Copyright (c) Sleipnir contributors

#pragma once

#include <algorithm>
#include <cmath>
#include <limits>
#include <utility>

#include <Eigen/Core>
#include <wpi/SmallVector.h>

#include "sleipnir/autodiff/Variable.hpp"

// See docs/algorithms.md#Works_cited for citation definitions.

namespace sleipnir {

/**
 * Filter entry consisting of cost and constraint violation.
 */
struct FilterEntry {
  /// The cost function's value
  double cost = 0.0;

  /// The constraint violation
  double constraintViolation = 0.0;

  constexpr FilterEntry() = default;

  /**
   * Constructs a FilterEntry.
   *
   * @param cost The cost function's value.
   * @param constraintViolation The constraint violation.
   */
  constexpr FilterEntry(double cost, double constraintViolation)
      : cost{cost}, constraintViolation{constraintViolation} {}
};

/**
 * Step filter.
 *
 * See the section on filters in chapter 15 of [1].
 */
class Filter {
 public:
  static constexpr double γCost = 1e-8;
  static constexpr double γConstraint = 1e-5;

  double maxConstraintViolation = 1e4;

  /**
   * Construct an empty filter.
   *
   * @param f The cost function.
   */
  explicit Filter(Variable& f) {
    m_f = &f;

    // Initial filter entry rejects constraint violations above max
    m_filter.emplace_back(std::numeric_limits<double>::infinity(),
                          maxConstraintViolation);
  }

  /**
   * Reset the filter.
   */
  void Reset() {
    m_filter.clear();

    // Initial filter entry rejects constraint violations above max
    m_filter.emplace_back(std::numeric_limits<double>::infinity(),
                          maxConstraintViolation);
  }

  /**
   * Creates a new Sequential Quadratic Programming filter entry.
   *
   * @param c_e The equality constraint values (nonzero means violation).
   */
  FilterEntry MakeEntry(const Eigen::VectorXd& c_e) {
    return FilterEntry{m_f->Value(), c_e.lpNorm<1>()};
  }

  /**
   * Creates a new interior-point method filter entry.
   *
   * @param s The inequality constraint slack variables.
   * @param c_e The equality constraint values (nonzero means violation).
   * @param c_i The inequality constraint values (negative means violation).
   * @param μ The barrier parameter.
   */
  FilterEntry MakeEntry(Eigen::VectorXd& s, const Eigen::VectorXd& c_e,
                        const Eigen::VectorXd& c_i, double μ) {
    return FilterEntry{m_f->Value() - μ * s.array().log().sum(),
                       c_e.lpNorm<1>() + (c_i - s).lpNorm<1>()};
  }

  /**
   * Add a new entry to the filter.
   *
   * @param entry The entry to add to the filter.
   */
  void Add(const FilterEntry& entry) {
    // Remove dominated entries
    erase_if(m_filter, [&](const auto& elem) {
      return entry.cost <= elem.cost &&
             entry.constraintViolation <= elem.constraintViolation;
    });

    m_filter.push_back(entry);
  }

  /**
   * Add a new entry to the filter.
   *
   * @param entry The entry to add to the filter.
   */
  void Add(FilterEntry&& entry) {
    // Remove dominated entries
    erase_if(m_filter, [&](const auto& elem) {
      return entry.cost <= elem.cost &&
             entry.constraintViolation <= elem.constraintViolation;
    });

    m_filter.push_back(entry);
  }

  /**
   * Returns true if the given iterate is accepted by the filter.
   *
   * @param entry The entry to attempt adding to the filter.
   */
  bool TryAdd(const FilterEntry& entry) {
    if (IsAcceptable(entry)) {
      Add(entry);
      return true;
    } else {
      return false;
    }
  }

  /**
   * Returns true if the given iterate is accepted by the filter.
   *
   * @param entry The entry to attempt adding to the filter.
   */
  bool TryAdd(FilterEntry&& entry) {
    if (IsAcceptable(entry)) {
      Add(std::move(entry));
      return true;
    } else {
      return false;
    }
  }

  /**
   * Returns true if the given entry is acceptable to the filter.
   *
   * @param entry The entry to check.
   */
  bool IsAcceptable(const FilterEntry& entry) {
    if (!std::isfinite(entry.cost) ||
        !std::isfinite(entry.constraintViolation)) {
      return false;
    }

    // If current filter entry is better than all prior ones in some respect,
    // accept it
    return std::all_of(m_filter.begin(), m_filter.end(), [&](const auto& elem) {
      return entry.cost <= elem.cost - γCost * elem.constraintViolation ||
             entry.constraintViolation <=
                 (1.0 - γConstraint) * elem.constraintViolation;
    });
  }

 private:
  Variable* m_f = nullptr;
  wpi::SmallVector<FilterEntry> m_filter;
};

}  // namespace sleipnir
