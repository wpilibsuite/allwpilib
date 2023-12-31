package edu.wpi.first.wpilibj2.command.async;

import static edu.wpi.first.units.Units.Seconds;

import edu.wpi.first.units.Measure;
import edu.wpi.first.units.Time;
import edu.wpi.first.util.ErrorMessages;

/**
 * An end condition that can be used to complete a loop after some period of time has elapsed.
 */
class TimedEndCondition implements EndCondition {
  private long startTime;
  private final long offset;

  public TimedEndCondition(Measure<Time> duration) {
    ErrorMessages.requireNonNullParam(duration, "duration", "TimedEndCondition");

    offset = (long) (duration.in(Seconds) * 1e9);
  }

  public static TimedEndCondition hasElapsed(Measure<Time> duration) {
    return new TimedEndCondition(duration);
  }

  @Override
  public void reset() {
    startTime = System.nanoTime();
  }

  @Override
  public boolean reached() {
    return System.nanoTime() - offset >= startTime;
  }
}
