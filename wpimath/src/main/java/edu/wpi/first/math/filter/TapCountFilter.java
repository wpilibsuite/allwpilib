package edu.wpi.first.math.filter;

import edu.wpi.first.math.MathSharedStore;

/**
 * A simple tap counting filter for boolean streams. Requires that the boolean change value to true
 * for a specified number of times within a specified time window after the first rising edge before
 * the filtered value changes.
 *
 * <p>The filter activates when the input has risen (transitioned from false to true) the required
 * number of times within the time window. Once activated, the output remains true as long as the
 * input is true. The tap count resets when the time window expires or when the input goes false
 * after activation.
 *
 * <p>Input must be stable; consider using a Debouncer before this filter to avoid counting noise as
 * multiple taps.
 */
public class TapCountFilter {

  private int m_requiredTaps = 0;
  private double m_tapWindowSeconds;

  private double m_firstTapTimeSeconds = 0.0;
  private int m_currentTapCount = 0;

  private boolean m_lastInput = false;

  /**
   * Creates a new TapCountFilter.
   *
   * @param requiredTaps The number of rising edges (taps) required before the output goes true.
   * @param tapWindowSeconds The maximum time window in which all required taps must occur after the
   *     first rising edge.
   */
  public TapCountFilter(int requiredTaps, double tapWindowSeconds) {
    m_requiredTaps = requiredTaps;
    m_tapWindowSeconds = tapWindowSeconds;

    resetTimer();
  }

  private void resetTimer() {
    m_firstTapTimeSeconds = MathSharedStore.getTimestamp();
  }

  private boolean hasElapsed() {
    return MathSharedStore.getTimestamp() - m_firstTapTimeSeconds >= m_tapWindowSeconds;
  }

  /**
   * Applies the tap counter filter to the input stream.
   *
   * @param input The current value of the input stream.
   * @return True if the required number of taps have occurred within the time window and the input
   *     is currently true; false otherwise.
   */
  public boolean calculate(boolean input) {
    if (input && !m_lastInput) {
      if (m_currentTapCount == 0) {
        resetTimer(); // Start timer on first rising edge
      }

      m_currentTapCount++;
    }

    boolean activated = input && m_currentTapCount >= m_requiredTaps;

    boolean timeExpired = hasElapsed() && !activated;
    boolean activationEnded = !input && m_currentTapCount >= m_requiredTaps;

    if (timeExpired || activationEnded) {
      m_currentTapCount = 0;
    }

    m_lastInput = input;

    return activated;
  }

  /**
   * Sets the tap window duration.
   *
   * @param tapWindowSeconds The maximum time window in which all required taps must occur after the
   *     first rising edge.
   */
  public void setTapWindowSeconds(double tapWindowSeconds) {
    m_tapWindowSeconds = tapWindowSeconds;
  }

  /**
   * Gets the tap window duration.
   *
   * @return The maximum time window in which all required taps must occur after the first rising
   *     edge.
   */
  public double getTapWindowSeconds() {
    return m_tapWindowSeconds;
  }

  /**
   * Sets the required number of taps.
   *
   * @param requiredTaps The number of rising edges (taps) required before the output goes true.
   */
  public void setRequiredTaps(int requiredTaps) {
    m_requiredTaps = requiredTaps;
  }

  /**
   * Gets the required number of taps.
   *
   * @return The number of rising edges (taps) required before the output goes true.
   */
  public int getRequiredTaps() {
    return m_requiredTaps;
  }
}
