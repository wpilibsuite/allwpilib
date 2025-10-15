// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util;

import java.nio.ByteBuffer;

/**
 * Class for storing raw frame data between image read call.
 *
 * <p>Data is reused for each frame read, rather then reallocating every frame.
 */
public class RawFrame implements AutoCloseable {
  private long m_nativeObj;
  private ByteBuffer m_data;
  private int m_width;
  private int m_height;
  private int m_stride;
  private PixelFormat m_pixelFormat = PixelFormat.kUnknown;
  private long m_time;
  private TimestampSource m_timeSource = TimestampSource.kUnknown;

  /** Construct a new empty RawFrame. */
  public RawFrame() {
    m_nativeObj = WPIUtilJNI.allocateRawFrame();
  }

  /**
   * Close the RawFrame, releasing native resources. Any images currently using the data will be
   * invalidated.
   */
  @Override
  public void close() {
    WPIUtilJNI.freeRawFrame(m_nativeObj);
    m_nativeObj = 0;
  }

  /**
   * Called from JNI to set data in class.
   *
   * @param data A native ByteBuffer pointing to the frame data.
   * @param width The width of the frame, in pixels
   * @param height The height of the frame, in pixels
   * @param stride The number of bytes in each row of image data
   * @param pixelFormat The PixelFormat of the frame
   */
  void setDataJNI(
      ByteBuffer data, int width, int height, int stride, int pixelFormat, long time, int timeSrc) {
    m_data = data;
    m_width = width;
    m_height = height;
    m_stride = stride;
    m_pixelFormat = PixelFormat.getFromInt(pixelFormat);
    m_time = time;
    m_timeSource = TimestampSource.getFromInt(timeSrc);
  }

  /**
   * Called from JNI to set info in class.
   *
   * @param width The width of the frame, in pixels
   * @param height The height of the frame, in pixels
   * @param stride The number of bytes in each row of image data
   * @param pixelFormat The PixelFormat of the frame
   */
  void setInfoJNI(int width, int height, int stride, int pixelFormat, long time, int timeSrc) {
    m_width = width;
    m_height = height;
    m_stride = stride;
    m_pixelFormat = PixelFormat.getFromInt(pixelFormat);
    m_time = time;
    m_timeSource = TimestampSource.getFromInt(timeSrc);
  }

  /**
   * Set frame data.
   *
   * @param data A native ByteBuffer pointing to the frame data.
   * @param width The width of the frame, in pixels
   * @param height The height of the frame, in pixels
   * @param stride The number of bytes in each row of image data
   * @param pixelFormat The PixelFormat of the frame
   */
  public void setData(ByteBuffer data, int width, int height, int stride, PixelFormat pixelFormat) {
    if (!data.isDirect()) {
      throw new UnsupportedOperationException("ByteBuffer must be direct");
    }
    m_data = data;
    m_width = width;
    m_height = height;
    m_stride = stride;
    m_pixelFormat = pixelFormat;
    WPIUtilJNI.setRawFrameData(
        m_nativeObj, data, data.limit(), width, height, stride, pixelFormat.getValue());
  }

  /**
   * Call to set frame information.
   *
   * @param width The width of the frame, in pixels
   * @param height The height of the frame, in pixels
   * @param stride The number of bytes in each row of image data
   * @param pixelFormat The PixelFormat of the frame
   */
  public void setInfo(int width, int height, int stride, PixelFormat pixelFormat) {
    m_width = width;
    m_height = height;
    m_stride = stride;
    m_pixelFormat = pixelFormat;
    WPIUtilJNI.setRawFrameInfo(
        m_nativeObj,
        m_data != null ? m_data.limit() : 0,
        width,
        height,
        stride,
        pixelFormat.getValue());
  }

  /**
   * Update this frame's timestamp info.
   *
   * @param frameTime the time this frame was grabbed at. This uses the same time base as
   *     wpi::Now(), in us.
   * @param frameTimeSource the time source for the timestamp this frame was grabbed at.
   */
  public void setTimeInfo(long frameTime, TimestampSource frameTimeSource) {
    m_time = frameTime;
    m_timeSource = frameTimeSource;
    WPIUtilJNI.setRawFrameTime(m_nativeObj, frameTime, frameTimeSource.getValue());
  }

  /**
   * Get the pointer to native representation of this frame.
   *
   * @return The pointer to native representation of this frame.
   */
  public long getNativeObj() {
    return m_nativeObj;
  }

  /**
   * Get a ByteBuffer pointing to the frame data. This ByteBuffer is backed by the frame directly.
   * Its lifetime is controlled by the frame. If a new frame gets read, it will overwrite the
   * current one.
   *
   * @return A ByteBuffer pointing to the frame data.
   */
  public ByteBuffer getData() {
    return m_data;
  }

  /**
   * Get a long (is a uint8_t* in native code) pointing to the frame data. This pointer is backed by
   * the frame directly. Its lifetime is controlled by the frame. If a new frame gets read, it will
   * overwrite the current one.
   *
   * @return A long pointing to the frame data.
   */
  public long getDataPtr() {
    return WPIUtilJNI.getRawFrameDataPtr(m_nativeObj);
  }

  /**
   * Get the total size of the data stored in the frame, in bytes.
   *
   * @return The total size of the data stored in the frame.
   */
  public int getSize() {
    return m_data != null ? m_data.limit() : 0;
  }

  /**
   * Get the width of the image.
   *
   * @return The width of the image, in pixels.
   */
  public int getWidth() {
    return m_width;
  }

  /**
   * Get the height of the image.
   *
   * @return The height of the image, in pixels.
   */
  public int getHeight() {
    return m_height;
  }

  /**
   * Get the number of bytes in each row of image data.
   *
   * @return The image data stride, in bytes.
   */
  public int getStride() {
    return m_stride;
  }

  /**
   * Get the PixelFormat of the frame.
   *
   * @return The PixelFormat of the frame.
   */
  public PixelFormat getPixelFormat() {
    return m_pixelFormat;
  }

  /**
   * Get the time this frame was grabbed at. This uses the same time base as wpi::Now(), in us.
   *
   * @return Time in 1 us increments.
   */
  public long getTimestamp() {
    return m_time;
  }

  /**
   * Get the time source for the timestamp this frame was grabbed at.
   *
   * @return Time source
   */
  public TimestampSource getTimestampSource() {
    return m_timeSource;
  }
}
