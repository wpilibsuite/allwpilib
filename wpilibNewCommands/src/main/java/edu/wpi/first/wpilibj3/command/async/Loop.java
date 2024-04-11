package edu.wpi.first.wpilibj3.command.async;

import static edu.wpi.first.units.Units.Milliseconds;

import edu.wpi.first.units.Measure;
import edu.wpi.first.units.Time;
import edu.wpi.first.util.ErrorMessages;

public class Loop {
  public Loop(ThrowingRunnable impl, EndCondition endCondition, Measure<Time> period) {
    this.impl = impl;
    this.endCondition = endCondition;
    this.period = period;
  }

  private final ThrowingRunnable impl;
  private final EndCondition endCondition;
  private final Measure<Time> period;

  @SuppressWarnings("BusyWait")
  public void loop() throws Exception {
    var nominalSleepTime = period.in(Milliseconds);
    endCondition.reset();

    while (!endCondition.reached()) {
      long start = System.nanoTime();
      impl.run();
      long elapsedTime = System.nanoTime() - start;

      // account for the runtime of the loop body so that it is called every N milliseconds,
      // rather than have a gap of N milliseconds between one loop end and the next loop begin
      long sleepTime = (long) (nominalSleepTime - elapsedTime / 1_000_000L);

      try {
        Thread.sleep(sleepTime);
      } catch (InterruptedException e) {
        // an interrupt means the underlying thread had a request to be halted.
        // exit the loop and reset the interrupt flag
        Thread.currentThread().interrupt();
        break;
      }
    }
  }

  public static class Builder {
    private final ThrowingRunnable impl;
    private Measure<Time> period = Milliseconds.of(20);

    protected Builder(ThrowingRunnable impl) {
      this.impl = impl;
    }

    /**
     * Sets the period of time between consecutive loop body calls. The default period is 20
     * milliseconds. The specified period must be at least 1 millisecond long.
     *
     * @param period the loop period
     * @return this builder
     */
    public Builder withPeriod(Measure<Time> period) {
      ErrorMessages.requireNonNullParam(period, "period", "withPeriod");
      if (period.in(Milliseconds) <= 0) {
        throw new IllegalArgumentException("Period must be at least 1 millisecond");
      }

      this.period = period;
      return this;
    }

    /**
     * Begins executing the loop immediately, running until the given end condition is met.
     *
     * @param endCondition the end condition that must be reached for the loop to be completed
     * @throws Exception any exception thrown by the loop body
     */
    public void until(EndCondition endCondition) throws Exception {
      new Loop(impl, endCondition, period).loop();
    }

    /**
     * Begins executing the loop immediately, running until the given amount of time has elapsed. If
     * the duration is zero or negative, the loop body will not be executed.
     *
     * @param duration how long the loop should run for
     * @throws Exception any exception thrown by the loop body
     */
    public void forDuration(Measure<Time> duration) throws Exception {
      until(new TimedEndCondition(duration));
    }

    public void forever() throws Exception {
      until(() -> false);
    }
  }
}
