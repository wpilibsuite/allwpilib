/*----------------------------------------------------------------------------*/
/* Copyright (c) 2014-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include <memory>
#include <string>

#include <wpi/ArrayRef.h>
#include <wpi/Twine.h>

#include "cscore.h"

namespace frc {

/**
 * Singleton class for creating and keeping camera servers.
 *
 * Also publishes camera information to NetworkTables.
 */
class CameraServer {
 public:
  static constexpr uint16_t kBasePort = 1181;
  static constexpr int kSize640x480 = 0;
  static constexpr int kSize320x240 = 1;
  static constexpr int kSize160x120 = 2;

  /**
   * Get the CameraServer instance.
   */
  static CameraServer* GetInstance();

  /**
   * Start automatically capturing images to send to the dashboard.
   *
   * You should call this method to see a camera feed on the dashboard. If you
   * also want to perform vision processing on the roboRIO, use getVideo() to
   * get access to the camera images.
   *
   * The first time this overload is called, it calls StartAutomaticCapture()
   * with device 0, creating a camera named "USB Camera 0".  Subsequent calls
   * increment the device number (e.g. 1, 2, etc).
   */
  cs::UsbCamera StartAutomaticCapture();

  /**
   * Start automatically capturing images to send to the dashboard.
   *
   * This overload calls StartAutomaticCapture() with a name of "USB Camera
   * {dev}".
   *
   * @param dev The device number of the camera interface
   */
  cs::UsbCamera StartAutomaticCapture(int dev);

  /**
   * Start automatically capturing images to send to the dashboard.
   *
   * @param name The name to give the camera
   * @param dev  The device number of the camera interface
   */
  cs::UsbCamera StartAutomaticCapture(const wpi::Twine& name, int dev);

  /**
   * Start automatically capturing images to send to the dashboard.
   *
   * @param name The name to give the camera
   * @param path The device path (e.g. "/dev/video0") of the camera
   */
  cs::UsbCamera StartAutomaticCapture(const wpi::Twine& name,
                                      const wpi::Twine& path);

  /**
   * Start automatically capturing images to send to the dashboard from
   * an existing camera.
   *
   * @param camera Camera
   */
  void StartAutomaticCapture(const cs::VideoSource& camera);

  /**
   * Adds an Axis IP camera.
   *
   * This overload calls AddAxisCamera() with name "Axis Camera".
   *
   * @param host Camera host IP or DNS name (e.g. "10.x.y.11")
   */
  cs::AxisCamera AddAxisCamera(const wpi::Twine& host);

  /**
   * Adds an Axis IP camera.
   *
   * This overload calls AddAxisCamera() with name "Axis Camera".
   *
   * @param host Camera host IP or DNS name (e.g. "10.x.y.11")
   */
  cs::AxisCamera AddAxisCamera(const char* host);

  /**
   * Adds an Axis IP camera.
   *
   * This overload calls AddAxisCamera() with name "Axis Camera".
   *
   * @param host Camera host IP or DNS name (e.g. "10.x.y.11")
   */
  cs::AxisCamera AddAxisCamera(const std::string& host);

  /**
   * Adds an Axis IP camera.
   *
   * This overload calls AddAxisCamera() with name "Axis Camera".
   *
   * @param hosts Array of Camera host IPs/DNS names
   */
  cs::AxisCamera AddAxisCamera(wpi::ArrayRef<std::string> hosts);

  /**
   * Adds an Axis IP camera.
   *
   * This overload calls AddAxisCamera() with name "Axis Camera".
   *
   * @param hosts Array of Camera host IPs/DNS names
   */
  template <typename T>
  cs::AxisCamera AddAxisCamera(std::initializer_list<T> hosts);

  /**
   * Adds an Axis IP camera.
   *
   * @param name The name to give the camera
   * @param host Camera host IP or DNS name (e.g. "10.x.y.11")
   */
  cs::AxisCamera AddAxisCamera(const wpi::Twine& name, const wpi::Twine& host);

  /**
   * Adds an Axis IP camera.
   *
   * @param name The name to give the camera
   * @param host Camera host IP or DNS name (e.g. "10.x.y.11")
   */
  cs::AxisCamera AddAxisCamera(const wpi::Twine& name, const char* host);

  /**
   * Adds an Axis IP camera.
   *
   * @param name The name to give the camera
   * @param host Camera host IP or DNS name (e.g. "10.x.y.11")
   */
  cs::AxisCamera AddAxisCamera(const wpi::Twine& name, const std::string& host);

  /**
   * Adds an Axis IP camera.
   *
   * @param name The name to give the camera
   * @param hosts Array of Camera host IPs/DNS names
   */
  cs::AxisCamera AddAxisCamera(const wpi::Twine& name,
                               wpi::ArrayRef<std::string> hosts);

  /**
   * Adds an Axis IP camera.
   *
   * @param name The name to give the camera
   * @param hosts Array of Camera host IPs/DNS names
   */
  template <typename T>
  cs::AxisCamera AddAxisCamera(const wpi::Twine& name,
                               std::initializer_list<T> hosts);

  /**
   * Get OpenCV access to the primary camera feed.  This allows you to
   * get images from the camera for image processing on the roboRIO.
   *
   * <p>This is only valid to call after a camera feed has been added
   * with startAutomaticCapture() or addServer().
   */
  cs::CvSink GetVideo();

  /**
   * Get OpenCV access to the specified camera.  This allows you to get
   * images from the camera for image processing on the roboRIO.
   *
   * @param camera Camera (e.g. as returned by startAutomaticCapture).
   */
  cs::CvSink GetVideo(const cs::VideoSource& camera);

  /**
   * Get OpenCV access to the specified camera.  This allows you to get
   * images from the camera for image processing on the roboRIO.
   *
   * @param name Camera name
   */
  cs::CvSink GetVideo(const wpi::Twine& name);

  /**
   * Create a MJPEG stream with OpenCV input. This can be called to pass custom
   * annotated images to the dashboard.
   *
   * @param name Name to give the stream
   * @param width Width of the image being sent
   * @param height Height of the image being sent
   */
  cs::CvSource PutVideo(const wpi::Twine& name, int width, int height);

  /**
   * Adds a MJPEG server at the next available port.
   *
   * @param name Server name
   */
  cs::MjpegServer AddServer(const wpi::Twine& name);

  /**
   * Adds a MJPEG server.
   *
   * @param name Server name
   */
  cs::MjpegServer AddServer(const wpi::Twine& name, int port);

  /**
   * Adds an already created server.
   *
   * @param server Server
   */
  void AddServer(const cs::VideoSink& server);

  /**
   * Removes a server by name.
   *
   * @param name Server name
   */
  void RemoveServer(const wpi::Twine& name);

  /**
   * Get server for the primary camera feed.
   *
   * This is only valid to call after a camera feed has been added with
   * StartAutomaticCapture() or AddServer().
   */
  cs::VideoSink GetServer();

  /**
   * Gets a server by name.
   *
   * @param name Server name
   */
  cs::VideoSink GetServer(const wpi::Twine& name);

  /**
   * Adds an already created camera.
   *
   * @param camera Camera
   */
  void AddCamera(const cs::VideoSource& camera);

  /**
   * Removes a camera by name.
   *
   * @param name Camera name
   */
  void RemoveCamera(const wpi::Twine& name);

  /**
   * Sets the size of the image to use. Use the public kSize constants to set
   * the correct mode, or set it directly on a camera and call the appropriate
   * StartAutomaticCapture method.
   *
   * @deprecated Use SetResolution on the UsbCamera returned by
   *             StartAutomaticCapture() instead.
   * @param size The size to use
   */
  void SetSize(int size);

 private:
  CameraServer();
  ~CameraServer();

  struct Impl;
  std::unique_ptr<Impl> m_impl;
};

}  // namespace frc

#include "cameraserver/CameraServer.inc"
