package edu.wpi.cscore;

import java.nio.ByteBuffer;

public class RawFrame implements AutoCloseable {

  long m_framePtr;
  private ByteBuffer m_dataByteBuffer;
  private long m_dataPtr;
  private int m_totalData;
  private int m_width;
  private int m_height;
  private int m_pixelFormat;

  public RawFrame() {
    m_framePtr = CameraServerJNI.allocateRawFrame();
  }

  @Override
  public void close() {
    CameraServerJNI.freeRawFrame(m_framePtr);
  }

  public void setData(ByteBuffer dataByteBuffer, long dataPtr, int totalData, int width, int height, int pixelFormat) {
    m_dataByteBuffer = dataByteBuffer;
    m_dataPtr = dataPtr;
    m_totalData = totalData;
    m_width = width;
    m_height = height;
    m_pixelFormat = pixelFormat;
  }

  /**
   * @return the dataPtr
   */
  public ByteBuffer getDataByteBuffer() {
    return m_dataByteBuffer;
  }

  public long getDataPtr() {
    return m_dataPtr;
  }

  /**
   * @return the totalData
   */
  public int getTotalData() {
    return m_totalData;
  }

  /**
   * @return the width
   */
  public int getWidth() {
    return m_width;
  }

  /**
   * @param width the width to set
   */
  public void setWidth(int width) {
    this.m_width = width;
  }

  /**
   * @return the height
   */
  public int getHeight() {
    return m_height;
  }

  /**
   * @param height the height to set
   */
  public void setHeight(int height) {
    this.m_height = height;
  }

  /**
   * @return the pixelFormat
   */
  public int getPixelFormat() {
    return m_pixelFormat;
  }

  /**
   * @param pixelFormat the pixelFormat to set
   */
  public void setPixelFormat(int pixelFormat) {
    this.m_pixelFormat = pixelFormat;
  }

}
