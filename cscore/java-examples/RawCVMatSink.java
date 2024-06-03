// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.cscore;

import edu.wpi.cscore.VideoMode.PixelFormat;
import edu.wpi.cscore.raw.RawFrame;
import java.nio.ByteBuffer;
import org.opencv.core.CvType;
import org.opencv.core.Mat;

public class RawCVMatSink extends ImageSink {
  RawFrame frame = new RawFrame();
  Mat tmpMat;
  ByteBuffer origByteBuffer;
  int width;
  int height;
  int pixelFormat;
  int bgrValue = PixelFormat.kBGR.getValue();

  private int getCVFormat(PixelFormat pixelFormat) {
    return switch (pixelFormat) {
      case kYUYV, kRGB565, kY16, kUYVY -> CvType.CV_8UC2;
      case kBGR -> CvType.CV_8UC3;
      case kBGRA -> CvType.CV_8UC4;
      case kGray, kMJPEG, kUnknown -> CvType.CV_8UC1;
    };
  }

  /**
   * Create a sink for accepting OpenCV images. WaitForFrame() must be called on the created sink to
   * get each new image.
   *
   * @param name Source name (arbitrary unique identifier)
   */
  public RawCVMatSink(String name) {
    super(CameraServerJNI.createRawSink(name));
  }

  /**
   * Wait for the next frame and get the image. Times out (returning 0) after 0.225 seconds. The
   * provided image will have three 3-bit channels stored in BGR order.
   *
   * @return Frame time, or 0 on error (call GetError() to obtain the error message)
   */
  public long grabFrame(Mat image) {
    return grabFrame(image, 0.225);
  }

  /**
   * Wait for the next frame and get the image. Times out (returning 0) after timeout seconds. The
   * provided image will have three 3-bit channels stored in BGR order.
   *
   * @return Frame time, or 0 on error (call GetError() to obtain the error message); the frame time
   *     is in 1 us increments.
   */
  public long grabFrame(Mat image, double timeout) {
    frame.setWidth(0);
    frame.setHeight(0);
    frame.setPixelFormat(bgrValue);
    long rv = CameraServerJNI.grabSinkFrameTimeout(m_handle, frame, timeout);
    if (rv <= 0) {
      return rv;
    }

    if (frame.getDataByteBuffer() != origByteBuffer
        || width != frame.getWidth()
        || height != frame.getHeight()
        || pixelFormat != frame.getPixelFormat()) {
      origByteBuffer = frame.getDataByteBuffer();
      height = frame.getHeight();
      width = frame.getWidth();
      pixelFormat = frame.getPixelFormat();
      tmpMat =
          new Mat(
              frame.getHeight(),
              frame.getWidth(),
              getCVFormat(VideoMode.getPixelFormatFromInt(pixelFormat)),
              origByteBuffer);
    }
    tmpMat.copyTo(image);
    return rv;
  }
}
