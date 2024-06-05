// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <optional>
#include <span>
#include <string>
#include <string_view>

#include <wpi/deprecated.h>

#include "cscore.h"

namespace frc {

class CameraServerCv;

/**
 * Singleton class for creating and keeping camera servers.
 *
 * Also publishes camera information to NetworkTables.
 */
class CameraServer {
 public:
  /// CameraServer base port.
  static constexpr uint16_t kBasePort = 1181;

  friend class CameraServerCv;

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
  static cs::UsbCamera StartAutomaticCapture();

  /**
   * Start automatically capturing images to send to the dashboard.
   *
   * This overload calls StartAutomaticCapture() with a name of "USB Camera
   * {dev}".
   *
   * @param dev The device number of the camera interface
   */
  static cs::UsbCamera StartAutomaticCapture(int dev);

  /**
   * Start automatically capturing images to send to the dashboard.
   *
   * @param name The name to give the camera
   * @param dev  The device number of the camera interface
   */
  static cs::UsbCamera StartAutomaticCapture(std::string_view name, int dev);

  /**
   * Start automatically capturing images to send to the dashboard.
   *
   * @param name The name to give the camera
   * @param path The device path (e.g. "/dev/video0") of the camera
   */
  static cs::UsbCamera StartAutomaticCapture(std::string_view name,
                                             std::string_view path);

  /**
   * Start automatically capturing images to send to the dashboard from
   * an existing camera.
   *
   * @param camera Camera
   */
  static cs::MjpegServer StartAutomaticCapture(const cs::VideoSource& camera);

  WPI_IGNORE_DEPRECATED
  /**
   * Adds an Axis IP camera.
   *
   * This overload calls AddAxisCamera() with name "Axis Camera".
   *
   * @param host Camera host IP or DNS name (e.g. "10.x.y.11")
   * @deprecated Call StartAutomaticCapture with a HttpCamera instead.
   */
  [[deprecated("Call StartAutomaticCapture with a HttpCamera instead.")]]
  static cs::AxisCamera AddAxisCamera(std::string_view host);

  /**
   * Adds an Axis IP camera.
   *
   * This overload calls AddAxisCamera() with name "Axis Camera".
   *
   * @param host Camera host IP or DNS name (e.g. "10.x.y.11")
   * @deprecated Call StartAutomaticCapture with a HttpCamera instead.
   */
  [[deprecated("Call StartAutomaticCapture with a HttpCamera instead.")]]
  static cs::AxisCamera AddAxisCamera(const char* host);

  /**
   * Adds an Axis IP camera.
   *
   * This overload calls AddAxisCamera() with name "Axis Camera".
   *
   * @param host Camera host IP or DNS name (e.g. "10.x.y.11")
   * @deprecated Call StartAutomaticCapture with a HttpCamera instead.
   */
  [[deprecated("Call StartAutomaticCapture with a HttpCamera instead.")]]
  static cs::AxisCamera AddAxisCamera(const std::string& host);

  /**
   * Adds an Axis IP camera.
   *
   * This overload calls AddAxisCamera() with name "Axis Camera".
   *
   * @param hosts Array of Camera host IPs/DNS names
   * @deprecated Call StartAutomaticCapture with a HttpCamera instead.
   */
  [[deprecated("Call StartAutomaticCapture with a HttpCamera instead.")]]
  static cs::AxisCamera AddAxisCamera(std::span<const std::string> hosts);

  /**
   * Adds an Axis IP camera.
   *
   * This overload calls AddAxisCamera() with name "Axis Camera".
   *
   * @param hosts Array of Camera host IPs/DNS names
   * @deprecated Call StartAutomaticCapture with a HttpCamera instead.
   */
  template <typename T>
  [[deprecated("Call StartAutomaticCapture with a HttpCamera instead.")]]
  static cs::AxisCamera AddAxisCamera(std::initializer_list<T> hosts);

  /**
   * Adds an Axis IP camera.
   *
   * @param name The name to give the camera
   * @param host Camera host IP or DNS name (e.g. "10.x.y.11")
   * @deprecated Call StartAutomaticCapture with a HttpCamera instead.
   */
  [[deprecated("Call StartAutomaticCapture with a HttpCamera instead.")]]
  static cs::AxisCamera AddAxisCamera(std::string_view name,
                                      std::string_view host);

  /**
   * Adds an Axis IP camera.
   *
   * @param name The name to give the camera
   * @param host Camera host IP or DNS name (e.g. "10.x.y.11")
   * @deprecated Call StartAutomaticCapture with a HttpCamera instead.
   */
  [[deprecated("Call StartAutomaticCapture with a HttpCamera instead.")]]
  static cs::AxisCamera AddAxisCamera(std::string_view name, const char* host);

  /**
   * Adds an Axis IP camera.
   *
   * @param name The name to give the camera
   * @param host Camera host IP or DNS name (e.g. "10.x.y.11")
   * @deprecated Call StartAutomaticCapture with a HttpCamera instead.
   */
  [[deprecated("Call StartAutomaticCapture with a HttpCamera instead.")]]
  static cs::AxisCamera AddAxisCamera(std::string_view name,
                                      const std::string& host);

  /**
   * Adds an Axis IP camera.
   *
   * @param name The name to give the camera
   * @param hosts Array of Camera host IPs/DNS names
   * @deprecated Call StartAutomaticCapture with a HttpCamera instead.
   */
  [[deprecated("Call StartAutomaticCapture with a HttpCamera instead.")]]
  static cs::AxisCamera AddAxisCamera(std::string_view name,
                                      std::span<const std::string> hosts);

  /**
   * Adds an Axis IP camera.
   *
   * @param name The name to give the camera
   * @param hosts Array of Camera host IPs/DNS names
   * @deprecated Call StartAutomaticCapture with a HttpCamera instead.
   */
  template <typename T>
  [[deprecated("Call StartAutomaticCapture with a HttpCamera instead.")]]
  static cs::AxisCamera AddAxisCamera(std::string_view name,
                                      std::initializer_list<T> hosts);
  WPI_UNIGNORE_DEPRECATED

  /**
   * Adds a virtual camera for switching between two streams.  Unlike the
   * other addCamera methods, this returns a VideoSink rather than a
   * VideoSource.  Calling SetSource() on the returned object can be used
   * to switch the actual source of the stream.
   */
  static cs::MjpegServer AddSwitchedCamera(std::string_view name);

  /**
   * Adds a MJPEG server at the next available port.
   *
   * @param name Server name
   */
  static cs::MjpegServer AddServer(std::string_view name);

  /**
   * Adds a MJPEG server.
   *
   * @param name Server name
   * @param port Port number
   */
  static cs::MjpegServer AddServer(std::string_view name, int port);

  /**
   * Adds an already created server.
   *
   * @param server Server
   */
  static void AddServer(const cs::VideoSink& server);

  /**
   * Removes a server by name.
   *
   * @param name Server name
   */
  static void RemoveServer(std::string_view name);

  /**
   * Get server for the primary camera feed.
   *
   * This is only valid to call after a camera feed has been added with
   * StartAutomaticCapture() or AddServer().
   */
  static cs::VideoSink GetServer();

  /**
   * Gets a server by name.
   *
   * @param name Server name
   */
  static cs::VideoSink GetServer(std::string_view name);

  /**
   * Adds an already created camera.
   *
   * @param camera Camera
   */
  static void AddCamera(const cs::VideoSource& camera);

  /**
   * Removes a camera by name.
   *
   * @param name Camera name
   */
  static void RemoveCamera(std::string_view name);

 private:
  CameraServer() = default;

  static cs::VideoSource GetInternalPrimarySource();
  static cs::VideoSource GetInternalVideoSource(std::string_view name);
  static std::optional<CS_Sink> GetInternalCvSink(std::string_view name);
};

}  // namespace frc

#include "cameraserver/CameraServer.inc"
