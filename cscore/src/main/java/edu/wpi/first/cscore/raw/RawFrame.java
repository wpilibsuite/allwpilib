// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.cscore.raw;

import edu.wpi.first.cscore.CameraServerJNI;
import java.nio.ByteBuffer;

/**
 * Class for storing raw frame data between image read call.
 *
 * <p>Data is reused for each frame read, rather then reallocating every frame.
 */
public class RawFrame implements AutoCloseable {
  private final long m_framePtr;
  private ByteBuffer m_dataByteBuffer;
  private long m_dataPtr;
  private int m_totalData;
  private int m_width;
  private int m_height;
  private int m_pixelFormat;

  /** Construct a new RawFrame. */
  public RawFrame() {
    m_framePtr = CameraServerJNI.allocateRawFrame();
  }

  /**
   * Close the RawFrame, releasing native resources. Any images currently using the data will be
   * invalidated.
   */
  @Override
  public void close() {
    CameraServerJNI.freeRawFrame(m_framePtr);
  }

  /**
   * Called from JNI to set data in class.
   *
   * @param dataByteBuffer A ByteBuffer pointing to the frame data.
   * @param dataPtr A long (a char* in native code) pointing to the frame data.
   * @param totalData The total length of the data stored in the frame.
   * @param width The width of the frame.
   * @param height The height of the frame.
   * @param pixelFormat The PixelFormat of the frame.
   */
  public void setData(
      ByteBuffer dataByteBuffer,
      long dataPtr,
      int totalData,
      int width,
      int height,
      int pixelFormat) {
    m_dataByteBuffer = dataByteBuffer;
    m_dataPtr = dataPtr;
    m_totalData = totalData;
    m_width = width;
    m_height = height;
    m_pixelFormat = pixelFormat;
  }

  /**
   * Get the pointer to native representation of this frame.
   *
   * @return The pointer to native representation of this frame.
   */
  public long getFramePtr() {
    return m_framePtr;
  }

  /**
   * Get a ByteBuffer pointing to the frame data. This ByteBuffer is backed by the frame directly.
   * Its lifetime is controlled by the frame. If a new frame gets read, it will overwrite the
   * current one.
   *
   * @return A ByteBuffer pointing to the frame data.
   */
  public ByteBuffer getDataByteBuffer() {
    return m_dataByteBuffer;
  }

  /**
   * Get a long (is a char* in native code) pointing to the frame data. This pointer is backed by
   * the frame directly. Its lifetime is controlled by the frame. If a new frame gets read, it will
   * overwrite the current one.
   *
   * @return A long pointing to the frame data.
   */
  public long getDataPtr() {
    return m_dataPtr;
  }

  /**
   * Get the total length of the data stored in the frame.
   *
   * @return The total length of the data stored in the frame.
   */
  public int getTotalData() {
    return m_totalData;
  }

  /**
   * Get the width of the frame.
   *
   * @return The width of the frame.
   */
  public int getWidth() {
    return m_width;
  }

  /**
   * Set the width of the frame.
   *
   * @param width The width of the frame.
   */
  public void setWidth(int width) {
    this.m_width = width;
  }

  /**
   * Get the height of the frame.
   *
   * @return The height of the frame.
   */
  public int getHeight() {
    return m_height;
  }

  /**
   * Set the height of the frame.
   *
   * @param height The height of the frame.
   */
  public void setHeight(int height) {
    this.m_height = height;
  }

  /**
   * Get the PixelFormat of the frame.
   *
   * @return The PixelFormat of the frame.
   */
  public int getPixelFormat() {
    return m_pixelFormat;
  }

  /**
   * Set the PixelFormat of the frame.
   *
   * @param pixelFormat The PixelFormat of the frame.
   */
  public void setPixelFormat(int pixelFormat) {
    this.m_pixelFormat = pixelFormat;
  }
}
