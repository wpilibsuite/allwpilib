// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <span>
#include <vector>

#include "wpi/filterdesigner/model/Stage.hpp"

namespace wpi::filterdesigner {

/** How @ref ApplyFilter primes the cascade before the first sample. */
enum class FilterStart {
  /**
   * Zero state, so the output opens with the filter's startup transient.
   * Correct whenever the first sample really is the first the filter would
   * ever see: a generated impulse or step, or a log read from its start.
   */
  Zero,
  /**
   * Steady state for the first finite sample. Meant for a sliding window over a
   * signal that was already running before the window opened: such a source
   * re-filters its whole buffer on every revision, and a zero start would
   * redraw the startup transient at the leading edge as the window slides,
   * which the equivalent filter running on the robot never shows.
   */
  SteadyState
};

/**
 * Runs @a samples through a @c BiquadFilter cascade built from @a sections.
 *
 * A non-finite input sample is a gap, not a value: it is reported as NaN and
 * never enters the recursive state, so it costs its own sample and nothing
 * else. Feeding it to the cascade would leave NaN in the section state and
 * turn every later output non-finite too.
 *
 * @param samples  Input samples.
 * @param sections Cascade to apply. Empty returns @a samples unchanged.
 * @param start    Initial filter state; see @ref FilterStart.
 * @return Filtered samples, same length as @a samples, NaN wherever the
 *         input was not finite.
 */
std::vector<double> ApplyFilter(std::span<const double> samples,
                                const Sections& sections,
                                FilterStart start = FilterStart::Zero);

}  // namespace wpi::filterdesigner
