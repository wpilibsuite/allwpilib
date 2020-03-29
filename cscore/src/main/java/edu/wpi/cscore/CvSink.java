/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.cscore;

import org.opencv.core.Mat;

/**
 * A sink for user code to accept video frames as OpenCV images.
 * These sinks require the WPILib OpenCV builds.
 * For an alternate OpenCV, see the documentation how to build your own
 * with RawSink.
 */
public class CvSink extends ImageSink {
  /**
   * Create a sink for accepting OpenCV images.
   * WaitForFrame() must be called on the created sink to get each new
   * image.
   *
   * @param name Source name (arbitrary unique identifier)
   */
  public CvSink(String name) {
    super(CameraServerCvJNI.createCvSink(name));
  }

  /**
   * Wait for the next frame and get the image.
   * Times out (returning 0) after 0.225 seconds.
   * The provided image will have three 3-bit channels stored in BGR order.
   *
   * @return Frame time, or 0 on error (call GetError() to obtain the error
   *         message)
   */
  public long grabFrame(Mat image) {
    return grabFrame(image, 0.225);
  }

  /**
   * Wait for the next frame and get the image.
   * Times out (returning 0) after timeout seconds.
   * The provided image will have three 3-bit channels stored in BGR order.
   *
   * @return Frame time, or 0 on error (call GetError() to obtain the error
   *         message); the frame time is in 1 us increments.
   */
  public long grabFrame(Mat image, double timeout) {
    return CameraServerCvJNI.grabSinkFrameTimeout(m_handle, image.nativeObj, timeout);
  }

  /**
   * Wait for the next frame and get the image.  May block forever.
   * The provided image will have three 3-bit channels stored in BGR order.
   *
   * @return Frame time, or 0 on error (call GetError() to obtain the error
   *         message); the frame time is in 1 us increments.
   */
  public long grabFrameNoTimeout(Mat image) {
    return CameraServerCvJNI.grabSinkFrame(m_handle, image.nativeObj);
  }
}
