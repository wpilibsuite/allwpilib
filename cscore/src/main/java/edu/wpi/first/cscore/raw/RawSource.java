// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.cscore.raw;

import edu.wpi.first.cscore.CameraServerJNI;
import edu.wpi.first.cscore.ImageSource;
import edu.wpi.first.cscore.VideoMode;
import edu.wpi.first.util.PixelFormat;
import edu.wpi.first.util.RawFrame;
import java.nio.ByteBuffer;

/**
 * A source for user code to provide video frames as raw bytes.
 *
 * <p>This is a complex API, most cases should use CvSource.
 */
public class RawSource extends ImageSource {
  /**
   * Create a raw frame source.
   *
   * @param name Source name (arbitrary unique identifier)
   * @param mode Video mode being generated
   */
  public RawSource(String name, VideoMode mode) {
    super(
        CameraServerJNI.createRawSource(
            name, false, mode.pixelFormat.getValue(), mode.width, mode.height, mode.fps));
  }

  /**
   * Create a raw frame source.
   *
   * @param name Source name (arbitrary unique identifier)
   * @param pixelFormat Pixel format
   * @param width width
   * @param height height
   * @param fps fps
   */
  public RawSource(String name, PixelFormat pixelFormat, int width, int height, int fps) {
    super(CameraServerJNI.createRawSource(name, false, pixelFormat.getValue(), width, height, fps));
  }

  /**
   * Put a raw image and notify sinks.
   *
   * @param image raw frame image
   */
  public void putFrame(RawFrame image) {
    CameraServerJNI.putRawSourceFrame(m_handle, image.getNativeObj());
  }

  /**
   * Put a raw image and notify sinks.
   *
   * @param data raw frame native data pointer
   * @param size total size in bytes
   * @param width frame width
   * @param height frame height
   * @param stride size of each row in bytes
   * @param pixelFormat pixel format
   */
  protected void putFrame(
      long data, int size, int width, int height, int stride, PixelFormat pixelFormat) {
    CameraServerJNI.putRawSourceFrameData(
        m_handle, data, size, width, height, stride, pixelFormat.getValue());
  }

  /**
   * Put a raw image and notify sinks.
   *
   * @param data raw frame native ByteBuffer
   * @param width frame width
   * @param height frame height
   * @param stride size of each row in bytes
   * @param pixelFormat pixel format
   */
  public void putFrame(
      ByteBuffer data, int width, int height, int stride, PixelFormat pixelFormat) {
    if (!data.isDirect()) {
      throw new UnsupportedOperationException("ByteBuffer must be direct");
    }
    CameraServerJNI.putRawSourceFrameBB(
        m_handle, data, data.limit(), width, height, stride, pixelFormat.getValue());
  }
}
