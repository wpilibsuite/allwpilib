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

import edu.wpi.cscore.VideoMode.PixelFormat;
import edu.wpi.first.wpiutil.RuntimeDetector;

public final class DevMain {
  public static int GetCVFormat(PixelFormat pixelFormat) {
    int type = 0;
    switch (pixelFormat) {
    case kYUYV:
    case kRGB565:
      type = CvType.CV_8UC2;
      break;
    case kBGR:
      type = CvType.CV_8UC3;
      break;
    case kGray:
    case kMJPEG:
    default:
      type = CvType.CV_8UC1;
      break;
    }
    return type;
  }

  /**
   * Main method.
   */
  public static void main(String[] args) {
    System.out.println("Hello World!");
    System.out.println(RuntimeDetector.getPlatformPath());
    System.out.println(CameraServerJNI.getHostname());

    UsbCamera usbCameraObj = new UsbCamera("Cam", 0);

    CameraServerCvJNI.forceLoad();


    usbCameraObj.setPixelFormat(PixelFormat.kYUYV);

    RawCVMatSink rawSink = new RawCVMatSink("Sink");

    MjpegServer server = new MjpegServer("Hello", 1181);

    RawCVMatSource cvSource = new RawCVMatSource("CvSource", PixelFormat.kGray, 640, 360, 30);

    server.setSource(cvSource);

    rawSink.setSource(usbCameraObj);

    Mat mat = new Mat();

    while (true) {
      if (rawSink.grabFrame(mat) > 0) {
        cvSource.putFrame(mat);
      }
    }



    // int usbCamera = usbCameraObj.m_handle;

    // //int usbCamera = CameraServerJNI.createUsbCameraDev("Camera", 0);

    // int rawSink = CameraServerJNI.createRawSink("Raw Sink");

    // int mjpeg = CameraServerJNI.createMjpegServer("Camera", "", 1181);

    // int cvSource = CameraServerJNI.createCvSource("Raw Sink", PixelFormat.kGray.getValue(), 640, 360, 30);

    // int mjpeg2 = CameraServerJNI.createMjpegServer("Camera", "", 1182);

    // int rawSource = CameraServerJNI.createRawSource("Raw Sink", PixelFormat.kGray.getValue(), 640, 360, 30);

    // CameraServerJNI.setSinkSource(rawSink, usbCamera);

    // CameraServerJNI.setSinkSource(mjpeg, cvSource);
    // CameraServerJNI.setSinkSource(mjpeg2, rawSource);

    // System.out.println(cvSource);

    // //CameraServerJNI.setSourceVideoMode(usbCamera, PixelFormat.kBGR.getValue(), 640, 360, 30);


    // RawFrame raw = new RawFrame();

    // ByteBuffer origByteBuffer = null;

    // Mat mat = null;

    // int width = 0;
    // int height = 0;
    // int pixelFormat = 0;


    // while (true) {
    //   raw.setPixelFormat(PixelFormat.kBGR.getValue());
    //   raw.setWidth(0);
    //   raw.setHeight(0);
    //   long timeout = CameraServerJNI.grabSinkFrame(rawSink, raw);
    //   if (timeout > 0) {
    //     if (raw.getDataByteBuffer() != origByteBuffer || width != raw.getWidth() || height != raw.getHeight() || pixelFormat != raw.getPixelFormat()) {
    //       origByteBuffer = raw.getDataByteBuffer();
    //       height = raw.getHeight();
    //       width = raw.getWidth();
    //       pixelFormat = raw.getPixelFormat();
    //       mat = new Mat(raw.getHeight(), raw.getWidth(), GetCVFormat(VideoMode.getPixelFormatFromInt(pixelFormat)), origByteBuffer);
    //     }
    //     CameraServerJNI.putSourceFrame(cvSource, mat.nativeObj);

    //     CameraServerJNI.putRawSourceFrame(rawSource, raw);
    //   }
    // }

  }

  private DevMain() {
  }
}
