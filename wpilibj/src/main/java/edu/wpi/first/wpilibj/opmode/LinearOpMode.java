// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.opmode;

import edu.wpi.first.hal.ControlWord;
import edu.wpi.first.wpilibj.DriverStation;
import edu.wpi.first.wpilibj.internal.DriverStationModeThread;
import java.util.concurrent.atomic.AtomicBoolean;

/**
 * An opmode structure for "linear" operation. The user is responsible for implementing any looping
 * behavior; after run() returns it will not be called again on the same object.
 *
 * <p>Lifecycle:
 *
 * <ul>
 *   <li>constructed when opmode selected on driver station
 *   <li>disabledPeriodic() called periodically as long as DS is disabled
 *   <li>when DS transitions from disabled to enabled, start(), run(), and end() are each called
 *       exactly once, in that order
 *   <li>when DS transitions from enabled to disabled, or a different opmode is selected on the
 *       driver station, close() is called; the object is not reused
 * </ul>
 *
 * <p>The user is responsible for exiting run() when the opmode is directed to stop executing. This
 * is indicated by isRunning() returning false. All other methods should be written to return as
 * quickly as possible when isRunning() returns false.
 */
public abstract class LinearOpMode implements OpMode {
  /** Constructor. */
  public LinearOpMode() {}

  /** Called periodically while the opmode is selected on the DS and the robot is disabled. */
  @Override
  public void disabledPeriodic() {}

  /**
   * Called when the opmode is de-selected on the DS. The object is not reused even if the same
   * opmode is selected again (a new object will be created).
   */
  public void close() {}

  /** Called a single time when the robot is enabled after being disabled. */
  public void start() {}

  /**
   * Called once when the robot is enabled, after start() is called. When it returns, end() will be
   * called and it will not be called again until the robot is disabled and enabled again.
   *
   * @throws InterruptedException when interrupted
   */
  public abstract void run() throws InterruptedException;

  /** Called a single time when the robot is disabled after being enabled. */
  public void end() {}

  /**
   * Returns true while this opmode is selected (regardless of enable state). All other functions
   * should be written to return as quickly as possible when this returns false.
   *
   * @return True if opmode selected, false otherwise
   */
  public boolean isRunning() {
    return m_running.get();
  }

  // implements OpMode interface
  @Override
  public final void opmodeRun(long opModeId) throws InterruptedException {
    ControlWord word = new ControlWord();
    DriverStation.refreshControlWordFromCache(word);
    word.setOpModeId(opModeId);

    try (DriverStationModeThread bgThread = new DriverStationModeThread(word)) {
      start();
      run();
      end();
    }
  }

  @Override
  public final void opmodeStop() {
    m_running.set(false);
  }

  @Override
  public final void opmodeClose() {
    close();
  }

  private final AtomicBoolean m_running = new AtomicBoolean(true);
}
