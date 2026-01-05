// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <initializer_list>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "wpi/cs/cscore_cpp.hpp"
#include "wpi/cs/VideoCamera.hpp"

namespace wpi::cs {

/**
 * A source that represents a MJPEG-over-HTTP (IP) camera.
 */
class HttpCamera : public VideoCamera {
 public:
  /**
   * HTTP camera kind.
   */
  enum HttpCameraKind {
    /// Unknown camera kind.
    kUnknown = CS_HTTP_UNKNOWN,
    /// MJPG Streamer camera.
    kMJPGStreamer = CS_HTTP_MJPGSTREAMER,
    /// CS Core camera.
    kCSCore = CS_HTTP_CSCORE,
  };

  /**
   * Create a source for a MJPEG-over-HTTP (IP) camera.
   *
   * @param name Source name (arbitrary unique identifier)
   * @param url Camera URL (e.g. "http://10.x.y.11/video/stream.mjpg")
   * @param kind Camera kind (e.g. kAxis)
   */
  HttpCamera(std::string_view name, std::string_view url,
             HttpCameraKind kind = kUnknown) {
    m_handle = CreateHttpCamera(
        name, url, static_cast<CS_HttpCameraKind>(static_cast<int>(kind)),
        &m_status);
  }

  /**
   * Create a source for a MJPEG-over-HTTP (IP) camera.
   *
   * @param name Source name (arbitrary unique identifier)
   * @param url Camera URL (e.g. "http://10.x.y.11/video/stream.mjpg")
   * @param kind Camera kind (e.g. kAxis)
   */
  HttpCamera(std::string_view name, const char* url,
             HttpCameraKind kind = kUnknown) {
    m_handle = CreateHttpCamera(
        name, url, static_cast<CS_HttpCameraKind>(static_cast<int>(kind)),
        &m_status);
  }

  /**
   * Create a source for a MJPEG-over-HTTP (IP) camera.
   *
   * @param name Source name (arbitrary unique identifier)
   * @param url Camera URL (e.g. "http://10.x.y.11/video/stream.mjpg")
   * @param kind Camera kind (e.g. kAxis)
   */
  HttpCamera(std::string_view name, const std::string& url,
             HttpCameraKind kind = kUnknown)
      : HttpCamera(name, std::string_view{url}, kind) {}

  /**
   * Create a source for a MJPEG-over-HTTP (IP) camera.
   *
   * @param name Source name (arbitrary unique identifier)
   * @param urls Array of Camera URLs
   * @param kind Camera kind (e.g. kAxis)
   */
  HttpCamera(std::string_view name, std::span<const std::string> urls,
             HttpCameraKind kind = kUnknown) {
    m_handle = CreateHttpCamera(
        name, urls, static_cast<CS_HttpCameraKind>(static_cast<int>(kind)),
        &m_status);
  }

  /**
   * Create a source for a MJPEG-over-HTTP (IP) camera.
   *
   * @param name Source name (arbitrary unique identifier)
   * @param urls Array of Camera URLs
   * @param kind Camera kind (e.g. kAxis)
   */
  template <typename T>
  HttpCamera(std::string_view name, std::initializer_list<T> urls,
             HttpCameraKind kind = kUnknown) {
    std::vector<std::string> vec;
    vec.reserve(urls.size());
    for (const auto& url : urls) {
      vec.emplace_back(url);
    }
    m_handle = CreateHttpCamera(
        name, vec, static_cast<CS_HttpCameraKind>(static_cast<int>(kind)),
        &m_status);
  }

  /**
   * Get the kind of HTTP camera.
   *
   * <p>Autodetection can result in returning a different value than the camera
   * was created with.
   */
  HttpCameraKind GetHttpCameraKind() const {
    m_status = 0;
    return static_cast<HttpCameraKind>(
        static_cast<int>(::wpi::cs::GetHttpCameraKind(m_handle, &m_status)));
  }

  /**
   * Change the URLs used to connect to the camera.
   */
  void SetUrls(std::span<const std::string> urls) {
    m_status = 0;
    ::wpi::cs::SetHttpCameraUrls(m_handle, urls, &m_status);
  }

  /**
   * Change the URLs used to connect to the camera.
   */
  template <typename T>
  void SetUrls(std::initializer_list<T> urls) {
    std::vector<std::string> vec;
    vec.reserve(urls.size());
    for (const auto& url : urls) {
      vec.emplace_back(url);
    }
    m_status = 0;
    ::wpi::cs::SetHttpCameraUrls(m_handle, vec, &m_status);
  }

  /**
   * Get the URLs used to connect to the camera.
   */
  std::vector<std::string> GetUrls() const {
    m_status = 0;
    return ::wpi::cs::GetHttpCameraUrls(m_handle, &m_status);
  }
};

}  // namespace wpi::cs
