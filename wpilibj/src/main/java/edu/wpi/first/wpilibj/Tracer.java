// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import java.util.HashMap;
import java.util.Map;
import java.util.function.Consumer;

/**
 * A class for keeping track of how much time it takes for different parts of code to execute. This
 * is done with epochs, that are added by calls to {@link #addEpoch(String)}, and can be printed
 * with a call to {@link #printEpochs()}.
 *
 * <p>Epochs are a way to partition the time elapsed so that when overruns occur, one can determine
 * which parts of an operation consumed the most time.
 */
public class Tracer {
  private static final long kMinPrintPeriod = 1000000; // microseconds

  private long m_lastEpochsPrintTime; // microseconds
  private long m_startTime; // microseconds

  private final Map<String, Long> m_epochs = new HashMap<>(); // microseconds

  /** Tracer constructor. */
  public Tracer() {
    resetTimer();
  }

  /** Clears all epochs. */
  public void clearEpochs() {
    m_epochs.clear();
    resetTimer();
  }

  /** Restarts the epoch timer. */
  public void resetTimer() {
    m_startTime = RobotController.getFPGATime();
  }

  /**
   * Adds time since last epoch to the list printed by printEpochs().
   *
   * <p>Epochs are a way to partition the time elapsed so that when overruns occur, one can
   * determine which parts of an operation consumed the most time.
   *
   * <p>This should be called immediately after execution has finished, with a call to this method
   * or {@link #resetTimer()} before execution.
   *
   * @param epochName The name to associate with the epoch.
   */
  public void addEpoch(String epochName) {
    long currentTime = RobotController.getFPGATime();
    m_epochs.put(epochName, currentTime - m_startTime);
    m_startTime = currentTime;
  }

  /** Prints list of epochs added so far and their times to the DriverStation. */
  public void printEpochs() {
    printEpochs(out -> DriverStation.reportWarning(out, false));
  }

  /**
   * Prints list of epochs added so far and their times to the entered String consumer.
   *
   * <p>This overload can be useful for logging to a file, etc.
   *
   * @param output the stream that the output is sent to
   */
  public void printEpochs(Consumer<String> output) {
    long now = RobotController.getFPGATime();
    if (now - m_lastEpochsPrintTime > kMinPrintPeriod) {
      StringBuilder sb = new StringBuilder();
      m_lastEpochsPrintTime = now;
      m_epochs.forEach(
          (key, value) -> {
            sb.append(String.format("\t%s: %.6fs\n", key, value / 1.0e6));
          });
      if (sb.length() > 0) {
        output.accept(sb.toString());
      }
    }
  }
}
