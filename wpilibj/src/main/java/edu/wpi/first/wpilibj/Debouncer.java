package edu.wpi.first.wpilibj;

/**
 * A simple debounce filter for boolean streams.  Requires that the boolean change value from
 * baseline for a specified period of time before the filtered value changes.
 */
public class Debouncer {

  private final Timer m_timer = new Timer();
  private final double m_debounceTime;
  private final boolean m_baseline;

  /**
   * Creates a new Debouncer.
   *
   * @param seconds  The number of seconds the value must change from baseline for the filtered
   *                 value to change.
   * @param baseline The "baseline" value of the boolean stream.
   */
  public Debouncer(double seconds, boolean baseline) {
    m_debounceTime = seconds;
    m_baseline = baseline;
    m_timer.start();
  }

  /**
   * Creates a new Debouncer.  Baseline value defaulted to "false."
   *
   * @param seconds The number of seconds the value must change from baseline for the filtered
   *                value to change.
   */
  public Debouncer(double seconds) {
    this(seconds, false);
  }

  /**
   * Applies the debouncer to the input stream.
   *
   * @param input The current value of the input stream.
   * @return The debounced value of the input stream.
   */
  public boolean calculate(boolean input) {
    if (input == m_baseline) {
      m_timer.reset();
    }

    if (m_timer.hasElapsed(m_debounceTime)) {
      return !m_baseline;
    } else {
      return m_baseline;
    }
  }
}
