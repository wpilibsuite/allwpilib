// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>

#include "units/time.h"

namespace frc {
/**
 * A simple tap counting filter for boolean streams. Requires that the boolean
 * change value to true for a specified number of times within a specified time
 * window after the first rising edge before the filtered value changes.
 *
 * <p>The filter activates when the input has risen (transitioned from false to
 * true) the required number of times within the time window. Once activated,
 * the output remains true as long as the input is true. The tap count resets
 * when the time window expires or when the input goes false after activation.
 *
 * <p>Input must be stable; consider using a Debouncer before this filter to
 * avoid counting noise as multiple taps.
 */
class WPILIB_DLLEXPORT MultiTapFilter {
 public:
  /**
   * Creates a new MultiTapFilter.
   *
   * @param requiredTaps The number of rising edges (taps) required before the
   *                     output goes true.
   * @param tapWindow The maximum time window in which all required taps must
   *                  occur after the first rising edge.
   */
  explicit MultiTapFilter(int requiredTaps, units::second_t tapWindow);

  /**
   * Applies the tap counter filter to the input stream.
   *
   * @param input The current value of the input stream.
   * @return True if the required number of taps have occurred within the time
   *         window and the input is currently true; false otherwise.
   */
  bool Calculate(bool input);

  /**
   * Sets the tap window duration.
   *
   * @param tapWindow The maximum time window in which all required taps must
   *                  occur after the first rising edge.
   */
  constexpr void SetTapWindow(units::second_t tapWindow) {
    m_tapWindow = tapWindow;
  }

  /**
   * Gets the tap window duration.
   *
   * @return The maximum time window in which all required taps must occur
   *         after the first rising edge.
   */
  constexpr units::second_t GetTapWindow() const { return m_tapWindow; }

  /**
   * Sets the required number of taps.
   *
   * @param requiredTaps The number of rising edges (taps) required before the
   *                     output goes true.
   */
  constexpr void SetRequiredTaps(int requiredTaps) {
    m_requiredTaps = requiredTaps;
  }

  /**
   * Gets the required number of taps.
   *
   * @return The number of rising edges (taps) required before the output goes
   *         true.
   */
  constexpr int GetRequiredTaps() const { return m_requiredTaps; }

 private:
  int m_requiredTaps;
  units::second_t m_tapWindow;

  units::second_t m_firstTapTime;
  int m_currentTapCount = 0;

  bool m_lastInput = false;

  void ResetTimer();

  bool HasElapsed() const;
};
}  // namespace frc
