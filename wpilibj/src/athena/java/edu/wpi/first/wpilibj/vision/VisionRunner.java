/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.vision;

import edu.wpi.cscore.CvSink;
import edu.wpi.cscore.VideoSource;
import edu.wpi.first.wpilibj.DriverStation;
import edu.wpi.first.wpilibj.RobotBase;
import org.opencv.core.Mat;

/**
 * A vision runner is a convenient wrapper object to make it easy to run vision pipelines from robot code. The easiest
 * way to use this is to run it in a {@link VisionThread} and use the listener to take snapshots the outputs of the
 * pipeline.
 *
 * @see VisionPipeline
 * @see edu.wpi.first.wpilibj.vision
 */
public class VisionRunner {

  private final CvSink cvSink = new CvSink("VisionRunner CvSink");
  private final VisionPipeline pipeline;
  private final Mat image = new Mat();
  private final Listener listener;

  /**
   * Listener interface for a callback that should run after a pipeline has processed its input.
   */
  @FunctionalInterface
  public interface Listener {

    /**
     * Called when the pipeline has run. This shouldn't take much time to run because it will delay later calls
     * to the pipeline's {@link VisionPipeline#process() process} method. Copying the outputs and code that
     * uses the copies should be <i>synchronized</i> on the same mutex to prevent multiple threads from reading and
     * writing to the same memory at the same time.
     */
    void copyPipelineOutputs();

  }

  /**
   * Creates a new vision runner. It will take images from the {@code videoSource}, send them to
   * the {@code pipeline}, and call the {@code listener} when the pipeline has finished to alert
   * user code when it is safe to access the
   *
   * @param videoSource the video source to use to supply images for the pipeline
   * @param pipeline    the vision pipeline to run
   * @param listener    a function to call after the pipeline has finished running
   */
  public VisionRunner(VideoSource videoSource, VisionPipeline pipeline, Listener listener) {
    this.pipeline = pipeline;
    this.listener = listener;
    cvSink.setSource(videoSource);
  }

  /**
   * Runs the pipeline one time, giving it the next image from the video source specified in the constructor.
   * This will block until the source either has an image or throws an error. If the source successfully
   * supplied a frame, the pipeline's image input will be set, the pipeline will run, and the listener
   * specified in the constructor will be called to notify it that the pipeline ran.
   */
  public void runOnce() {
    long frameTime = cvSink.grabFrame(image);
    if (frameTime == 0) {
      // There was an error, report it
      String error = cvSink.getError();
      DriverStation.reportError(error, true);
    } else {
      // No errors, process the image
      pipeline.setImageInput(image);
      pipeline.process();
      listener.copyPipelineOutputs();
    }
  }

  /**
   * A convenience method that calls {@link #runOnce()} in an infinite loop. This is intended
   * to be run in a dedicated thread.
   *
   * <p><strong>Do not call this method directly from the main thread.</strong></p>
   *
   * @throws IllegalStateException if this is called from the main robot thread
   */
  public void runForever() {
    if (Thread.currentThread().getId() == RobotBase.MAIN_THREAD_ID) {
      throw new IllegalStateException("VisionRunner.runForever() cannot be called from the main robot thread");
    }
    while (true) {
      runOnce();
    }
  }

}
