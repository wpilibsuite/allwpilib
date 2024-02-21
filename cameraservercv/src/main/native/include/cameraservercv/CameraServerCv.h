// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <span>
#include <string>
#include <string_view>

#include "cscore.h"
#include "cscore_cv.h"

namespace frc {

/**
 * Singleton class for creating and keeping camera servers.
 *
 * Also publishes camera information to NetworkTables.
 */
class CameraServerCv {
 public:
  /**
   * Get OpenCV access to the primary camera feed.  This allows you to
   * get images from the camera for image processing on the roboRIO.
   *
   * <p>This is only valid to call after a camera feed has been added
   * with startAutomaticCapture() or addServer().
   */
  static cs::CvSink GetVideo();

  /**
   * Get OpenCV access to the specified camera.  This allows you to get
   * images from the camera for image processing on the roboRIO.
   *
   * @param camera Camera (e.g. as returned by startAutomaticCapture).
   */
  static cs::CvSink GetVideo(const cs::VideoSource& camera);

  /**
   * Get OpenCV access to the specified camera.  This allows you to get
   * images from the camera for image processing on the roboRIO.
   *
   * @param camera Camera (e.g. as returned by startAutomaticCapture).
   * @param pixelFormat The desired pixelFormat of captured frames from the
   * camera
   */
  static cs::CvSink GetVideo(const cs::VideoSource& camera,
                             cs::VideoMode::PixelFormat pixelFormat);

  /**
   * Get OpenCV access to the specified camera.  This allows you to get
   * images from the camera for image processing on the roboRIO.
   *
   * @param name Camera name
   */
  static cs::CvSink GetVideo(std::string_view name);

  /**
   * Get OpenCV access to the specified camera.  This allows you to get
   * images from the camera for image processing on the roboRIO.
   *
   * @param name Camera name
   * @param pixelFormat The desired pixelFormat of captured frames from the
   * camera
   */
  static cs::CvSink GetVideo(std::string_view name,
                             cs::VideoMode::PixelFormat pixelFormat);

  /**
   * Create a MJPEG stream with OpenCV input. This can be called to pass custom
   * annotated images to the dashboard.
   *
   * @param name Name to give the stream
   * @param width Width of the image being sent
   * @param height Height of the image being sent
   */
  static cs::CvSource PutVideo(std::string_view name, int width, int height);

 private:
  CameraServerCv() = default;
};

}  // namespace frc
