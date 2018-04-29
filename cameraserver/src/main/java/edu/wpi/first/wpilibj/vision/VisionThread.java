/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.vision;

import edu.wpi.cscore.VideoSource;

/**
 * A vision thread is a special thread that runs a vision pipeline. It is a <i>daemon</i> thread;
 * it does not prevent the program from exiting when all other non-daemon threads
 * have finished running.
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
  public VisionThread(VisionRunner<?> visionRunner) {
    super(visionRunner::runForever, "WPILib Vision Thread");
    setDaemon(true);
  }

  /**
   * Creates a new vision thread that continuously runs the given vision pipeline. This is
   * equivalent to {@code new VisionThread(new VisionRunner<>(videoSource, pipeline, listener))}.
   *
   * @param videoSource the source for images the pipeline should process
   * @param pipeline    the pipeline to run
   * @param listener    the listener to copy outputs from the pipeline after it runs
   * @param <P>         the type of the pipeline
   */
  public <P extends VisionPipeline> VisionThread(VideoSource videoSource,
                                                 P pipeline,
                                                 VisionRunner.Listener<? super P> listener) {
    this(new VisionRunner<>(videoSource, pipeline, listener));
  }

}
