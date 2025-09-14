// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.opmode;

import java.util.concurrent.atomic.AtomicBoolean;

/**
 * An opmode structure for "linear" operation. The user is responsible for implementing any looping
 * behavior; after run() returns it will not be called again on the same object.
 *
 * <p>Lifecycle: - constructed when opmode selected on driver station - disabledPeriodic() called
 * periodically as long as DS is disabled - when DS transitions from disabled to enabled, start(),
 * run(), and end() are each called exactly once, in that order - when DS transitions from enabled
 * to disabled, or a different opmode is selected on the driver station, close() is called; the
 * object is not reused
 *
 * <p>The user is responsible for exiting run() when the opmode is directed to stop executing. This
 * is indicated
 */
public abstract class LinearOpMode implements OpMode {
  /** Called periodically while the opmode is selected on the DS (robot is disabled). */
  @Override
  public void disabledPeriodic() {}

  /**
   * Called when the opmode is de-selected on the DS. The object is not reused even if the same
   * opmode is selected again (a new object will be created).
   */
  public void close() {}

  public void start() {
    // this code is called when the opmode starts (robot is enabled)
  }

  // this code is called once to run the opmode (robot is enabled)
  public abstract void run() throws InterruptedException;

  public void end() {
    // this code is called when the opmode ends (robot is disabled)
  }

  public boolean isRunning() {
    return m_running.get();
  }

  // implements OpMode interface
  @Override
  public final void opmodeRun(long opModeId) throws InterruptedException {
    start();
    run();
    end();
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
