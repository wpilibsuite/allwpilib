/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <units/units.h>

#include "frc2/Timer.h"

namespace frc {
/**
 * A simple debounce filter for boolean streams.  Requires that the boolean
 * change value from baseline for a specified period of time before the filtered
 * value changes.
 */
class Debouncer {
 public:
  /**
   * Creates a new Debouncer.
   *
   * @param seconds  The number of seconds the value must change from baseline
   * for the filtered value to change.
   * @param baseline The "baseline" value of the boolean stream.
   */
  explicit Debouncer(units::second_t debounceTime, bool baseline = false);

  /**
   * Applies the debouncer to the input stream.
   *
   * @param input The current value of the input stream.
   * @return The debounced value of the input stream.
   */
  bool Calculate(bool input);

 private:
  frc2::Timer m_timer;
  units::second_t m_debounceTime;
  bool m_baseline;
};
}  // namespace frc
