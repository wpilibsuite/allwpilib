// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.filter;

import edu.wpi.first.math.MathSharedStore;

/**
 * A rising edge counter for boolean streams. Requires that the boolean change value to true for a
 * specified number of times within a specified time window after the first rising edge before the
 * filtered value changes.
 *
 * <p>The filter activates when the input has risen (transitioned from false to true) the required
 * number of times within the time window. Once activated, the output remains true as long as the
 * input is true. The edge count resets when the time window expires or when the input goes false
 * after activation.
 *
 * <p>Input must be stable; consider using a Debouncer before this filter to avoid counting noise as
 * multiple edges.
 */
public class EdgeCounterFilter {
  private int m_requiredEdges;
  private double m_windowTimeSeconds;

  private double m_firstEdgeTimeSeconds;
  private int m_currentCount;

  private boolean m_lastInput;

  /**
   * Creates a new EdgeCounterFilter.
   *
   * @param requiredEdges The number of rising edges required before the output goes true.
   * @param windowTime The maximum number of seconds in which all required edges must occur after
   *     the first rising edge.
   */
  public EdgeCounterFilter(int requiredEdges, double windowTime) {
    m_requiredEdges = requiredEdges;
    m_windowTimeSeconds = windowTime;

    resetTimer();
  }

  private void resetTimer() {
    m_firstEdgeTimeSeconds = MathSharedStore.getTimestamp();
  }

  private boolean hasElapsed() {
    return MathSharedStore.getTimestamp() - m_firstEdgeTimeSeconds >= m_windowTimeSeconds;
  }

  /**
   * Applies the edge counter filter to the input stream.
   *
   * @param input The current value of the input stream.
   * @return True if the required number of edges have occurred within the time window and the input
   *     is currently true; false otherwise.
   */
  public boolean calculate(boolean input) {
    boolean enoughEdges = m_currentCount >= m_requiredEdges;

    boolean expired = hasElapsed() && !enoughEdges;
    boolean activationEnded = !input && enoughEdges;

    if (expired || activationEnded) {
      m_currentCount = 0;
    }

    if (input && !m_lastInput) {
      if (m_currentCount == 0) {
        resetTimer(); // Start timer on first rising edge
      }

      m_currentCount++;
    }

    m_lastInput = input;

    return input && m_currentCount >= m_requiredEdges;
  }

  /**
   * Sets the time window duration.
   *
   * @param windowTime The maximum window of seconds in which all required edges must
   *     occur after the first rising edge.
   */
  public void setWindowTime(double windowTime) {
    m_windowTimeSeconds = windowTime;
  }

  /**
   * Gets the time window duration.
   *
   * @return The maximum window of seconds in which all required edges must occur after the first
   *     rising edge.
   */
  public double getWindowTime() {
    return m_windowTimeSeconds;
  }

  /**
   * Sets the required number of edges.
   *
   * @param requiredEdges The number of rising edges required before the output goes true.
   */
  public void setRequiredEdges(int requiredEdges) {
    m_requiredEdges = requiredEdges;
  }

  /**
   * Gets the required number of edges.
   *
   * @return The number of rising edges required before the output goes true.
   */
  public int getRequiredEdges() {
    return m_requiredEdges;
  }
}
