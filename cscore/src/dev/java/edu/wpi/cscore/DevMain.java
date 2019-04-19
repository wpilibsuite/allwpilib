/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.cscore;

import java.nio.ByteBuffer;

import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.osgi.OpenCVInterface;

import edu.wpi.cscore.VideoMode.PixelFormat;
import edu.wpi.first.wpiutil.RuntimeDetector;

public final class DevMain {
  /**
   * Main method.
   */
  public static void main(String[] args) {
    System.out.println("Hello World!");
    System.out.println(RuntimeDetector.getPlatformPath());
    System.out.println(CameraServerJNI.getHostname());

    int usbCamera = CameraServerJNI.createUsbCameraDev("Camera", 0);

    int rawSink = CameraServerJNI.createRawSink("Raw Sink");

    int mjpeg = CameraServerJNI.createMjpegServer("Camera", "", 1181);

    int cvSource = CameraServerJNI.createCvSource("Raw Sink", PixelFormat.kGray.getValue(), 640, 360, 30);


    int mjpeg2 = CameraServerJNI.createMjpegServer("Camera", "", 1182);

    int rawSource = CameraServerJNI.createRawSource("Raw Sink", PixelFormat.kGray.getValue(), 640, 360, 30);

    CameraServerJNI.setSinkSource(rawSink, usbCamera);

    CameraServerJNI.setSinkSource(mjpeg, cvSource);
    CameraServerJNI.setSinkSource(mjpeg2, rawSource);

    System.out.println(cvSource);


    RawFrame raw = new RawFrame();

    ByteBuffer origByteBuffer = null;

    Mat mat = null;


    while (true) {
      raw.setPixelFormat(PixelFormat.kUnknown.getValue());
      raw.setWidth(0);
      raw.setHeight(0);
      long timeout = CameraServerJNI.grabSinkFrame(rawSink, raw);
      if (timeout > 0) {
        if (raw.getDataByteBuffer() != origByteBuffer) {
          origByteBuffer = raw.getDataByteBuffer();
          mat = new Mat(raw.getHeight(), raw.getWidth(), CvType.CV_8UC1, origByteBuffer);
        }
        CameraServerJNI.putSourceFrame(cvSource, mat.nativeObj);

        CameraServerJNI.putRawSourceFrame(rawSource, raw.getDataPtr(), raw.getWidth(), raw.getHeight(), raw.getPixelFormat(), raw.getTotalData());
      }
    }

  }

  private DevMain() {
  }
}
