// Copyright (c) Sleipnir contributors

#pragma once

#include <concepts>
#include <limits>
#include <utility>

#include "sleipnir/util/Assert.hpp"
#include "sleipnir/util/SymbolExports.hpp"

namespace sleipnir {

namespace slicing {

struct none_t {};
static inline constexpr none_t _;

}  // namespace slicing

class SLEIPNIR_DLLEXPORT Slice {
 public:
  /// Start index (inclusive).
  int start = 0;

  /// Stop index (exclusive).
  int stop = 0;

  /// Step.
  int step = 1;

  /**
   * Constructs a Slice.
   */
  constexpr Slice() = default;

  /**
   * Constructs a slice.
   *
   * @param stop Slice stop index (exclusive).
   */
  template <typename Stop>
    requires std::same_as<Stop, slicing::none_t> ||
             std::convertible_to<Stop, int>
  constexpr Slice(Stop stop)  // NOLINT
      : Slice(slicing::_, std::move(stop), 1) {}

  /**
   * Constructs a slice.
   *
   * @param start Slice start index (inclusive).
   * @param stop Slice stop index (exclusive).
   */
  template <typename Start, typename Stop>
    requires(std::same_as<Start, slicing::none_t> ||
             std::convertible_to<Start, int>) &&
            (std::same_as<Stop, slicing::none_t> ||
             std::convertible_to<Stop, int>)
  constexpr Slice(Start start, Stop stop)
      : Slice(std::move(start), std::move(stop), 1) {}

  /**
   * Constructs a slice.
   *
   * @param start Slice start index (inclusive).
   * @param stop Slice stop index (exclusive).
   * @param step Slice step.
   */
  template <typename Start, typename Stop, typename Step>
    requires(std::same_as<Start, slicing::none_t> ||
             std::convertible_to<Start, int>) &&
            (std::same_as<Stop, slicing::none_t> ||
             std::convertible_to<Stop, int>) &&
            (std::same_as<Step, slicing::none_t> ||
             std::convertible_to<Step, int>)
  constexpr Slice(Start start, Stop stop, Step step) {
    if constexpr (std::same_as<Step, slicing::none_t>) {
      this->step = 1;
    } else {
      Assert(step != 0);

      this->step = step;
    }

    // Avoid UB for step = -step if step is INT_MIN
    if (this->step == std::numeric_limits<int>::min()) {
      this->step = -std::numeric_limits<int>::max();
    }

    if constexpr (std::same_as<Start, slicing::none_t>) {
      if (this->step < 0) {
        this->start = std::numeric_limits<int>::max();
      } else {
        this->start = 0;
      }
    } else {
      this->start = start;
    }

    if constexpr (std::same_as<Stop, slicing::none_t>) {
      if (this->step < 0) {
        this->stop = std::numeric_limits<int>::min();
      } else {
        this->stop = std::numeric_limits<int>::max();
      }
    } else {
      this->stop = stop;
    }
  }

  /**
   * Adjusts start and end slice indices assuming a sequence of the specified
   * length.
   *
   * @param length The sequence length.
   * @return The slice length.
   */
  constexpr int Adjust(int length) {
    Assert(step != 0);
    Assert(step >= -std::numeric_limits<int>::max());

    if (start < 0) {
      start += length;

      if (start < 0) {
        start = (step < 0) ? -1 : 0;
      }
    } else if (start >= length) {
      start = (step < 0) ? length - 1 : length;
    }

    if (stop < 0) {
      stop += length;

      if (stop < 0) {
        stop = (step < 0) ? -1 : 0;
      }
    } else if (stop >= length) {
      stop = (step < 0) ? length - 1 : length;
    }

    if (step < 0) {
      if (stop < start) {
        return (start - stop - 1) / -step + 1;
      } else {
        return 0;
      }
    } else {
      if (start < stop) {
        return (stop - start - 1) / step + 1;
      } else {
        return 0;
      }
    }
  }
};

}  // namespace sleipnir
