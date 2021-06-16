// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.cscore;

import org.opencv.core.Mat;

/**
 * A sink for user code to accept video frames as OpenCV images. These sinks require the WPILib
 * OpenCV builds. For an alternate OpenCV, see the documentation how to build your own with RawSink.
 */
public class CvSink extends ImageSink {
  /**
   * Create a sink for accepting OpenCV images. WaitForFrame() must be called on the created sink to
   * get each new image.
   *
   * @param name Source name (arbitrary unique identifier)
   */
  public CvSink(String name) {
    super(CameraServerCvJNI.createCvSink(name));
  }

  /// Create a sink for accepting OpenCV images in a separate thread.
  /// A thread will be created that calls WaitForFrame() and calls the
  /// processFrame() callback each time a new frame arrives.
  /// @param name Source name (arbitrary unique identifier)
  /// @param processFrame Frame processing function; will be called with a
  ///        time=0 if an error occurred.  processFrame should call GetImage()
  ///        or GetError() as needed, but should not call (except in very
  ///        unusual circumstances) WaitForImage().
  // public CvSink(wpi::StringRef name,
  //       std::function<void(uint64_t time)> processFrame) {
  //  super(CameraServerJNI.createCvSinkCallback(name, processFrame));
  // }

  /**
   * Wait for the next frame and get the image. Times out (returning 0) after 0.225 seconds. The
   * provided image will have three 3-bit channels stored in BGR order.
   *
   * @param image Where to store the image.
   * @return Frame time, or 0 on error (call GetError() to obtain the error message)
   */
  public long grabFrame(Mat image) {
    return grabFrame(image, 0.225);
  }

  /**
   * Wait for the next frame and get the image. Times out (returning 0) after timeout seconds. The
   * provided image will have three 3-bit channels stored in BGR order.
   *
   * @param image Where to store the image.
   * @param timeout Retrieval timeout in seconds.
   * @return Frame time, or 0 on error (call GetError() to obtain the error message); the frame time
   *     is in 1 us increments.
   */
  public long grabFrame(Mat image, double timeout) {
    return CameraServerCvJNI.grabSinkFrameTimeout(m_handle, image.nativeObj, timeout);
  }

  /**
   * Wait for the next frame and get the image. May block forever. The provided image will have
   * three 3-bit channels stored in BGR order.
   *
   * @param image Where to store the image.
   * @return Frame time, or 0 on error (call GetError() to obtain the error message); the frame time
   *     is in 1 us increments.
   */
  public long grabFrameNoTimeout(Mat image) {
    return CameraServerCvJNI.grabSinkFrame(m_handle, image.nativeObj);
  }
}
