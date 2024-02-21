// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.cameraservercv;

import edu.wpi.first.cameraserver.CameraServer;
import edu.wpi.first.cscore.VideoException;
import edu.wpi.first.cscore.VideoSink;
import edu.wpi.first.cscore.VideoSource;
import edu.wpi.first.cscorecv.CvSink;
import edu.wpi.first.cscorecv.CvSource;
import edu.wpi.first.util.PixelFormat;

/**
 * Singleton class for creating and keeping camera servers. Also publishes camera information to
 * NetworkTables.
 */
public final class CameraServerCv {
  private CameraServerCv() {}

  /**
   * Get OpenCV access to the primary camera feed. This allows you to get images from the camera for
   * image processing on the roboRIO.
   *
   * <p>This is only valid to call after a camera feed has been added with startAutomaticCapture()
   * or addServer().
   *
   * @return OpenCV sink for the primary camera feed
   */
  public static CvSink getVideo() {
    VideoSource source = CameraServer.Internal.getInternalPrimarySource();
    if (source == null) {
      throw new VideoException("no camera available");
    }
    return getVideo(source);
  }

  /**
   * Get OpenCV access to the specified camera. This allows you to get images from the camera for
   * image processing on the roboRIO.
   *
   * @param camera Camera (e.g. as returned by startAutomaticCapture).
   * @return OpenCV sink for the specified camera
   */
  public static CvSink getVideo(VideoSource camera) {
    return getVideo(camera, PixelFormat.kBGR);
  }

  /**
   * Get OpenCV access to the specified camera. This allows you to get images from the camera for
   * image processing on the roboRIO.
   *
   * @param camera Camera (e.g. as returned by startAutomaticCapture).
   * @param pixelFormat Desired pixelFormat of the camera
   * @return OpenCV sink for the specified camera
   */
  public static CvSink getVideo(VideoSource camera, PixelFormat pixelFormat) {
    String name = "opencv_" + camera.getName();

    VideoSink sink = CameraServer.Internal.getInternalCvSink(name);
    if (sink != null) {
      return (CvSink) sink;
    }

    CvSink newsink = new CvSink(name, pixelFormat);
    newsink.setSource(camera);
    CameraServer.addServer(newsink);
    return newsink;
  }

  /**
   * Get OpenCV access to the specified camera. This allows you to get images from the camera for
   * image processing on the roboRIO.
   *
   * @param name Camera name
   * @return OpenCV sink for the specified camera
   */
  public static CvSink getVideo(String name) {
    VideoSource source = CameraServer.Internal.getInternalVideoSource(name);
    return getVideo(source);
  }

  /**
   * Get OpenCV access to the specified camera. This allows you to get images from the camera for
   * image processing on the roboRIO.
   *
   * @param name Camera name
   * @param pixelFormat Desired pixelFormat of the camera
   * @return OpenCV sink for the specified camera
   */
  public static CvSink getVideo(String name, PixelFormat pixelFormat) {
    VideoSource source = CameraServer.Internal.getInternalVideoSource(name);
    return getVideo(source, pixelFormat);
  }

  /**
   * Create a MJPEG stream with OpenCV input. This can be called to pass custom annotated images to
   * the dashboard.
   *
   * @param name Name to give the stream
   * @param width Width of the image being sent
   * @param height Height of the image being sent
   * @return OpenCV source for the MJPEG stream
   */
  public static CvSource putVideo(String name, int width, int height) {
    CvSource source = new CvSource(name, PixelFormat.kMJPEG, width, height, 30);
    CameraServer.startAutomaticCapture(source);
    return source;
  }
}
