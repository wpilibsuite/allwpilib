/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.vision;

/**
 * A vision thread is a special thread that runs a vision pipeline. These threads are <i>daemon</i> threads; they do not
 * prevent the program from exiting when all other non-daemon threads have finished running.
 *
 * @see VisionPipeline
 * @see VisionRunner
 * @see Thread#setDaemon(boolean)
 */
public class VisionThread extends Thread {

  /**
   * Creates a vision thread that continuously runs a {@link VisionPipeline}.
   *
   * @param visionRunner the runner for a vision pipeline
   */
  public VisionThread(VisionRunner visionRunner) {
    super(visionRunner::runForever, "WPILib vision thread");
    setDaemon(true);
  }

}
