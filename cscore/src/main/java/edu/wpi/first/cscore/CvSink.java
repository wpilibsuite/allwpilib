// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.cscore;

import edu.wpi.first.util.PixelFormat;
import edu.wpi.first.util.RawFrame;
import edu.wpi.first.util.TimestampSource;
import java.nio.ByteBuffer;
import org.opencv.core.CvType;
import org.opencv.core.Mat;

/**
 * A sink for user code to accept video frames as OpenCV images. These sinks require the WPILib
 * OpenCV builds. For an alternate OpenCV, see the documentation how to build your own with RawSink.
 */
public class CvSink extends ImageSink {
  private final RawFrame m_frame = new RawFrame();
  private Mat m_tmpMat;
  private ByteBuffer m_origByteBuffer;
  private int m_width;
  private int m_height;
  private PixelFormat m_pixelFormat;

  @Override
  public void close() {
    if (m_tmpMat != null) {
      m_tmpMat.release();
    }
    m_frame.close();
    super.close();
  }

  private int getCVFormat(PixelFormat pixelFormat) {
    return switch (pixelFormat) {
      case kYUYV, kRGB565, kY16, kUYVY -> CvType.CV_8UC2;
      case kBGR -> CvType.CV_8UC3;
      case kBGRA -> CvType.CV_8UC4;
      case kGray, kMJPEG, kUnknown -> CvType.CV_8UC1;
    };
  }

  /**
   * Create a sink for accepting OpenCV images. grabFrame() must be called on the created sink to
   * get each new image.
   *
   * @param name Sink name (arbitrary unique identifier)
   * @param pixelFormat Source pixel format
   */
  public CvSink(String name, PixelFormat pixelFormat) {
    super(CameraServerJNI.createRawSink(name, true));
    m_pixelFormat = pixelFormat;
    OpenCvLoader.forceStaticLoad();
  }

  /**
   * Create a sink for accepting OpenCV images. WaitForFrame() must be called on the created sink to
   * get each new image. Defaults to kBGR for pixelFormat
   *
   * @param name Source name (arbitrary unique identifier)
   */
  public CvSink(String name) {
    this(name, PixelFormat.kBGR);
  }

  /**
   * Wait for the next frame and get the image. Times out (returning 0) after 0.225 seconds. The
   * provided image will have the pixelFormat this class was constructed with.
   *
   * @param image Where to store the image.
   * @return Frame time, or 0 on error (call GetError() to obtain the error message)
   */
  public long grabFrame(Mat image) {
    return grabFrame(image, 0.225);
  }

  /**
   * Wait for the next frame and get the image. Times out (returning 0) after timeout seconds. The
   * provided image will have the pixelFormat this class was constructed with.
   *
   * @param image Where to store the image.
   * @param timeout Retrieval timeout in seconds.
   * @return Frame time, or 0 on error (call GetError() to obtain the error message); the frame time
   *     is in 1 us increments.
   */
  public long grabFrame(Mat image, double timeout) {
    long rv = grabFrameDirect(timeout);
    if (rv <= 0) {
      return rv;
    }
    m_tmpMat.copyTo(image);
    return rv;
  }

  /**
   * Wait for the next frame and get the image. May block forever. The provided image will have the
   * pixelFormat this class was constructed with.
   *
   * @param image Where to store the image.
   * @return Frame time, or 0 on error (call GetError() to obtain the error message); the frame time
   *     is in 1 us increments.
   */
  public long grabFrameNoTimeout(Mat image) {
    long rv = grabFrameNoTimeoutDirect();
    if (rv <= 0) {
      return rv;
    }
    m_tmpMat.copyTo(image);
    return rv;
  }

  /**
   * Get the direct backing mat for this sink.
   *
   * <p>This mat can be invalidated any time any of the grab* methods are called, or when the CvSink
   * is closed.
   *
   * @return The backing mat.
   */
  public Mat getDirectMat() {
    return m_tmpMat;
  }

  /**
   * Wait for the next frame and store the image. Times out (returning 0) after 0.225 seconds. The
   * provided image will have the pixelFormat this class was constructed with. Use getDirectMat() to
   * grab the image.
   *
   * @return Frame time, or 0 on error (call GetError() to obtain the error message)
   */
  public long grabFrameDirect() {
    return grabFrameDirect(0.225);
  }

  /**
   * Wait for the next frame and store the image. Times out (returning 0) after timeout seconds. The
   * provided image will have the pixelFormat this class was constructed with. Use getDirectMat() to
   * grab the image.
   *
   * @param timeout Retrieval timeout in seconds.
   * @return Frame time, or 0 on error (call GetError() to obtain the error message); the frame time
   *     is in 1 us increments.
   */
  @SuppressWarnings("PMD.CompareObjectsWithEquals")
  public long grabFrameDirect(double timeout) {
    m_frame.setInfo(0, 0, 0, m_pixelFormat);
    long rv =
        CameraServerJNI.grabRawSinkFrameTimeout(m_handle, m_frame, m_frame.getNativeObj(), timeout);
    if (rv <= 0) {
      return rv;
    }

    if (m_frame.getData() != m_origByteBuffer
        || m_width != m_frame.getWidth()
        || m_height != m_frame.getHeight()
        || m_pixelFormat != m_frame.getPixelFormat()) {
      m_origByteBuffer = m_frame.getData();
      m_height = m_frame.getHeight();
      m_width = m_frame.getWidth();
      m_pixelFormat = m_frame.getPixelFormat();
      if (m_frame.getStride() == 0) {
        m_tmpMat =
            new Mat(
                m_frame.getHeight(),
                m_frame.getWidth(),
                getCVFormat(m_pixelFormat),
                m_origByteBuffer);
      } else {
        m_tmpMat =
            new Mat(
                m_frame.getHeight(),
                m_frame.getWidth(),
                getCVFormat(m_pixelFormat),
                m_origByteBuffer,
                m_frame.getStride());
      }
    }
    return rv;
  }

  /**
   * Wait for the next frame and store the image. May block forever. The provided image will have
   * the pixelFormat this class was constructed with. Use getDirectMat() to grab the image.
   *
   * @return Frame time, or 0 on error (call GetError() to obtain the error message); the frame time
   *     is in 1 us increments.
   */
  @SuppressWarnings("PMD.CompareObjectsWithEquals")
  public long grabFrameNoTimeoutDirect() {
    m_frame.setInfo(0, 0, 0, m_pixelFormat);
    long rv = CameraServerJNI.grabRawSinkFrame(m_handle, m_frame, m_frame.getNativeObj());
    if (rv <= 0) {
      return rv;
    }

    if (m_frame.getData() != m_origByteBuffer
        || m_width != m_frame.getWidth()
        || m_height != m_frame.getHeight()
        || m_pixelFormat != m_frame.getPixelFormat()) {
      m_origByteBuffer = m_frame.getData();
      m_height = m_frame.getHeight();
      m_width = m_frame.getWidth();
      m_pixelFormat = m_frame.getPixelFormat();
      if (m_frame.getStride() == 0) {
        m_tmpMat =
            new Mat(
                m_frame.getHeight(),
                m_frame.getWidth(),
                getCVFormat(m_pixelFormat),
                m_origByteBuffer);
      } else {
        m_tmpMat =
            new Mat(
                m_frame.getHeight(),
                m_frame.getWidth(),
                getCVFormat(m_pixelFormat),
                m_origByteBuffer,
                m_frame.getStride());
      }
    }
    return rv;
  }

  /**
   * Get the last time a frame was grabbed. This uses the same time base as wpi::Now().
   *
   * @return Time in 1 us increments.
   */
  public long getLastFrameTime() {
    return m_frame.getTimestamp();
  }

  /**
   * Get the time source for the timestamp the last frame was grabbed at.
   *
   * @return Time source
   */
  public TimestampSource getLastFrameTimeSource() {
    return m_frame.getTimestampSource();
  }
}
