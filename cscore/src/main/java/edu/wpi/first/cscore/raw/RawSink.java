// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.cscore.raw;

import edu.wpi.first.cscore.CameraServerJNI;
import edu.wpi.first.cscore.ImageSink;
import edu.wpi.first.util.RawFrame;

/**
 * A sink for user code to accept video frames as raw bytes.
 *
 * <p>This is a complex API, most cases should use CvSink.
 */
public class RawSink extends ImageSink {
  /**
   * Create a sink for accepting raw images.
   *
   * <p>grabFrame() must be called on the created sink to get each new image.
   *
   * @param name Source name (arbitrary unique identifier)
   */
  public RawSink(String name) {
    super(CameraServerJNI.createRawSink(name, false));
  }

  /**
   * Wait for the next frame and get the image. Times out (returning 0) after 0.225 seconds. The
   * provided image will have three 8-bit channels stored in BGR order.
   *
   * @param frame The frame object in which to store the image.
   * @return Frame time, or 0 on error (call getError() to obtain the error message); the frame time
   *     is in the same time base as wpi::Now(), and is in 1 us increments.
   */
  public long grabFrame(RawFrame frame) {
    return grabFrame(frame, 0.225);
  }

  /**
   * Wait for the next frame and get the image. Times out (returning 0) after timeout seconds. The
   * provided image will have three 8-bit channels stored in BGR order.
   *
   * @param frame The frame object in which to store the image.
   * @param timeout The frame timeout in seconds.
   * @return Frame time, or 0 on error (call getError() to obtain the error message); the frame time
   *     is in the same time base as wpi::Now(), and is in 1 us increments.
   */
  public long grabFrame(RawFrame frame, double timeout) {
    return CameraServerJNI.grabRawSinkFrameTimeout(m_handle, frame, frame.getNativeObj(), timeout);
  }

  /**
   * Wait for the next frame and get the image. May block forever. The provided image will have
   * three 8-bit channels stored in BGR order.
   *
   * @param frame The frame object in which to store the image.
   * @return Frame time, or 0 on error (call getError() to obtain the error message); the frame time
   *     is in the same time base as wpi::Now(), and is in 1 us increments.
   */
  public long grabFrameNoTimeout(RawFrame frame) {
    return CameraServerJNI.grabRawSinkFrame(m_handle, frame, frame.getNativeObj());
  }
}
