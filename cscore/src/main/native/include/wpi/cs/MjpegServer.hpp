// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>
#include <string_view>

#include "wpi/cs/cscore_cpp.hpp"
#include "wpi/cs/VideoSink.hpp"

namespace wpi::cs {

/**
 * A sink that acts as a MJPEG-over-HTTP network server.
 */
class MjpegServer : public VideoSink {
 public:
  MjpegServer() = default;

  /**
   * Create a MJPEG-over-HTTP server sink.
   *
   * @param name Sink name (arbitrary unique identifier)
   * @param listenAddress TCP listen address (empty string for all addresses)
   * @param port TCP port number
   */
  MjpegServer(std::string_view name, std::string_view listenAddress, int port) {
    m_handle = CreateMjpegServer(name, listenAddress, port, &m_status);
  }

  /**
   * Create a MJPEG-over-HTTP server sink.
   *
   * @param name Sink name (arbitrary unique identifier)
   * @param port TCP port number
   */
  MjpegServer(std::string_view name, int port) : MjpegServer(name, "", port) {}

  /**
   * Get the listen address of the server.
   */
  std::string GetListenAddress() const {
    m_status = 0;
    return wpi::cs::GetMjpegServerListenAddress(m_handle, &m_status);
  }

  /**
   * Get the port number of the server.
   */
  int GetPort() const {
    m_status = 0;
    return wpi::cs::GetMjpegServerPort(m_handle, &m_status);
  }

  /**
   * Set the stream resolution for clients that don't specify it.
   *
   * <p>It is not necessary to set this if it is the same as the source
   * resolution.
   *
   * <p>Setting this different than the source resolution will result in
   * increased CPU usage, particularly for MJPEG source cameras, as it will
   * decompress, resize, and recompress the image, instead of using the
   * camera's MJPEG image directly.
   *
   * @param width width, 0 for unspecified
   * @param height height, 0 for unspecified
   */
  void SetResolution(int width, int height) {
    m_status = 0;
    SetProperty(GetSinkProperty(m_handle, "width", &m_status), width,
                &m_status);
    SetProperty(GetSinkProperty(m_handle, "height", &m_status), height,
                &m_status);
  }

  /**
   * Set the stream frames per second (FPS) for clients that don't specify it.
   *
   * <p>It is not necessary to set this if it is the same as the source FPS.
   *
   * @param fps FPS, 0 for unspecified
   */
  void SetFPS(int fps) {
    m_status = 0;
    SetProperty(GetSinkProperty(m_handle, "fps", &m_status), fps, &m_status);
  }

  /**
   * Set the compression for clients that don't specify it.
   *
   * <p>Setting this will result in increased CPU usage for MJPEG source cameras
   * as it will decompress and recompress the image instead of using the
   * camera's MJPEG image directly.
   *
   * @param quality JPEG compression quality (0-100), -1 for unspecified
   */
  void SetCompression(int quality) {
    m_status = 0;
    SetProperty(GetSinkProperty(m_handle, "compression", &m_status), quality,
                &m_status);
  }

  /**
   * Set the default compression used for non-MJPEG sources.  If not set,
   * 80 is used.  This function has no effect on MJPEG source cameras; use
   * SetCompression() instead to force recompression of MJPEG source images.
   *
   * @param quality JPEG compression quality (0-100)
   */
  void SetDefaultCompression(int quality) {
    m_status = 0;
    SetProperty(GetSinkProperty(m_handle, "default_compression", &m_status),
                quality, &m_status);
  }
};

}  // namespace wpi::cs
