// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>

#include "units/time.h"

namespace frc {
/**
 * A rising edge counter for boolean streams. Requires that the boolean change
 * value to true for a specified number of times within a specified time window
 * after the first rising edge before the filtered value changes.
 *
 * The filter activates when the input has risen (transitioned from false to
 * true) the required number of times within the time window. Once activated,
 * the output remains true as long as the input is true. The edge count resets
 * when the time window expires or when the input goes false after activation.
 *
 * Input must be stable; consider using a Debouncer before this filter to avoid
 * counting noise as multiple edges.
 */
class WPILIB_DLLEXPORT EdgeCounterFilter {
 public:
  /**
   * Creates a new EdgeCounterFilter.
   *
   * @param requiredEdges The number of rising edges required before the output
   *                      goes true.
   * @param windowTime The maximum time window in which all required edges must
   *               occur after the first rising edge.
   */
  explicit EdgeCounterFilter(int requiredEdges, units::second_t windowTime);

  /**
   * Applies the edge counter filter to the input stream.
   *
   * @param input The current value of the input stream.
   * @return True if the required number of edges have occurred within the time
   *         window and the input is currently true; false otherwise.
   */
  bool Calculate(bool input);

  /**
   * Sets the time window duration.
   *
   * @param window The maximum time window in which all required edges must
   *               occur after the first rising edge.
   */
  constexpr void SetWindowTime(units::second_t time) { m_windowTime = time; }

  /**
   * Gets the time window duration.
   *
   * @return The maximum time window in which all required edges must occur
   *         after the first rising edge.
   */
  constexpr units::second_t GetWindowTime() const { return m_windowTime; }

  /**
   * Sets the required number of edges.
   *
   * @param requiredEdges The number of rising edges required before the output
   *                      goes true.
   */
  constexpr void SetRequiredEdges(int requiredEdges) {
    m_requiredEdges = requiredEdges;
  }

  /**
   * Gets the required number of edges.
   *
   * @return The number of rising edges required before the output goes true.
   */
  constexpr int GetRequiredEdges() const { return m_requiredEdges; }

 private:
  int m_requiredEdges;
  units::second_t m_windowTime;

  units::second_t m_firstEdgeTime;
  int m_currentCount = 0;

  bool m_lastInput = false;

  void ResetTimer();

  bool HasElapsed() const;
};
}  // namespace frc
