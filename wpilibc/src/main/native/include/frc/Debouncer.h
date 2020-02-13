#pragma once

#include "frc2/Timer.h"
#include <units/units.h>

namespace frc {
/**
 * A simple debounce filter for boolean streams.  Requires that the boolean change value from
 * baseline for a specified period of time before the filtered value changes.
 */
class Debouncer {
 public:
  /**
   * Creates a new Debouncer.
   *
   * @param seconds  The number of seconds the value must change from baseline for the filtered
   *                 value to change.
   * @param baseline The "baseline" value of the boolean stream.
   */
  Debouncer(units::second_t debounceTime, bool baseline = false);

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
}
