// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <string>
#include <vector>

#include "wpi/cs/VideoProperty.hpp"
#include "wpi/cs/cscore_cpp.hpp"

namespace wpi::cs {

class VideoEvent;
class VideoSink;

/**
 * A source for video that provides a sequence of frames.
 */
class VideoSource {
  friend class VideoEvent;
  friend class VideoSink;

 public:
  /**
   * Video source kind.
   */
  enum Kind {
    /// Unknown video source.
    kUnknown = CS_SOURCE_UNKNOWN,
    /// USB video source.
    kUsb = CS_SOURCE_USB,
    /// HTTP video source.
    kHttp = CS_SOURCE_HTTP,
    /// CV video source.
    kCv = CS_SOURCE_CV,
    /// Raw video source.
    kRaw = CS_SOURCE_RAW,
  };

  /** Connection strategy.  Used for SetConnectionStrategy(). */
  enum ConnectionStrategy {
    /**
     * Automatically connect or disconnect based on whether any sinks are
     * connected to this source.  This is the default behavior.
     */
    kConnectionAutoManage = CS_CONNECTION_AUTO_MANAGE,

    /**
     * Try to keep the connection open regardless of whether any sinks are
     * connected.
     */
    kConnectionKeepOpen = CS_CONNECTION_KEEP_OPEN,

    /**
     * Never open the connection.  If this is set when the connection is open,
     * close the connection.
     */
    kConnectionForceClose = CS_CONNECTION_FORCE_CLOSE
  };

  VideoSource() noexcept = default;

  VideoSource(const VideoSource& source)
      : m_handle(
            source.m_handle == 0 ? 0 : CopySource(source.m_handle, &m_status)) {
  }

  VideoSource(VideoSource&& other) noexcept : VideoSource() {
    swap(*this, other);
  }

  VideoSource& operator=(VideoSource other) noexcept {
    swap(*this, other);
    return *this;
  }

  ~VideoSource() {
    m_status = 0;
    if (m_handle != 0) {
      ReleaseSource(m_handle, &m_status);
    }
  }

  explicit operator bool() const { return m_handle != 0; }

  int GetHandle() const { return m_handle; }

  bool operator==(const VideoSource& other) const {
    return m_handle == other.m_handle;
  }

  /**
   * Get the kind of the source.
   */
  Kind GetKind() const {
    m_status = 0;
    return static_cast<VideoSource::Kind>(GetSourceKind(m_handle, &m_status));
  }

  /**
   * Get the name of the source.  The name is an arbitrary identifier
   * provided when the source is created, and should be unique.
   */
  std::string GetName() const {
    m_status = 0;
    return GetSourceName(m_handle, &m_status);
  }

  /**
   * Get the source description.  This is source-kind specific.
   */
  std::string GetDescription() const {
    m_status = 0;
    return GetSourceDescription(m_handle, &m_status);
  }

  /**
   * Get the last time a frame was captured.
   * This uses the same time base as wpi::util::Now().
   *
   * @return Time in 1 us increments.
   */
  uint64_t GetLastFrameTime() const {
    m_status = 0;
    return GetSourceLastFrameTime(m_handle, &m_status);
  }

  /**
   * Sets the connection strategy.  By default, the source will automatically
   * connect or disconnect based on whether any sinks are connected.
   *
   * <p>This function is non-blocking; look for either a connection open or
   * close event or call IsConnected() to determine the connection state.
   *
   * @param strategy connection strategy (auto, keep open, or force close)
   */
  void SetConnectionStrategy(ConnectionStrategy strategy) {
    m_status = 0;
    SetSourceConnectionStrategy(
        m_handle,
        static_cast<CS_ConnectionStrategy>(static_cast<int>(strategy)),
        &m_status);
  }

  /**
   * Is the source currently connected to whatever is providing the images?
   */
  bool IsConnected() const {
    m_status = 0;
    return IsSourceConnected(m_handle, &m_status);
  }

  /**
   * Gets source enable status.  This is determined with a combination of
   * connection strategy and the number of sinks connected.
   *
   * @return True if enabled, false otherwise.
   */
  bool IsEnabled() const {
    m_status = 0;
    return IsSourceEnabled(m_handle, &m_status);
  }

  /** Get a property.
   *
   * @param name Property name
   * @return Property contents (of kind Property::kNone if no property with
   *         the given name exists)
   */
  VideoProperty GetProperty(std::string_view name) {
    m_status = 0;
    return VideoProperty{GetSourceProperty(m_handle, name, &m_status)};
  }

  /**
   * Enumerate all properties of this source.
   */
  std::vector<VideoProperty> EnumerateProperties() const;

  /**
   * Get the current video mode.
   */
  VideoMode GetVideoMode() const {
    m_status = 0;
    return GetSourceVideoMode(m_handle, &m_status);
  }

  /**
   * Set the video mode.
   *
   * @param mode Video mode
   */
  bool SetVideoMode(const VideoMode& mode) {
    m_status = 0;
    return SetSourceVideoMode(m_handle, mode, &m_status);
  }

  /**
   * Set the video mode.
   *
   * @param pixelFormat desired pixel format
   * @param width desired width
   * @param height desired height
   * @param fps desired FPS
   * @return True if set successfully
   */
  bool SetVideoMode(wpi::util::PixelFormat pixelFormat, int width, int height,
                    int fps) {
    m_status = 0;
    return SetSourceVideoMode(
        m_handle, VideoMode{pixelFormat, width, height, fps}, &m_status);
  }

  /**
   * Set the pixel format.
   *
   * @param pixelFormat desired pixel format
   * @return True if set successfully
   */
  bool SetPixelFormat(wpi::util::PixelFormat pixelFormat) {
    m_status = 0;
    return SetSourcePixelFormat(m_handle, pixelFormat, &m_status);
  }

  /**
   * Set the resolution.
   *
   * @param width desired width
   * @param height desired height
   * @return True if set successfully
   */
  bool SetResolution(int width, int height) {
    m_status = 0;
    return SetSourceResolution(m_handle, width, height, &m_status);
  }

  /**
   * Set the frames per second (FPS).
   *
   * @param fps desired FPS
   * @return True if set successfully
   */
  bool SetFPS(int fps) {
    m_status = 0;
    return SetSourceFPS(m_handle, fps, &m_status);
  }

  /**
   * Set video mode and properties from a JSON configuration string.
   *
   * The format of the JSON input is:
   *
   * <pre>
   * {
   *     "pixel format": "MJPEG", "YUYV", etc
   *     "width": video mode width
   *     "height": video mode height
   *     "fps": video mode fps
   *     "brightness": percentage brightness
   *     "white balance": "auto", "hold", or value
   *     "exposure": "auto", "hold", or value
   *     "properties": [
   *         {
   *             "name": property name
   *             "value": property value
   *         }
   *     ]
   * }
   * </pre>
   *
   * @param config configuration
   * @return True if set successfully
   */
  bool SetConfigJson(std::string_view config) {
    m_status = 0;
    return SetSourceConfigJson(m_handle, config, &m_status);
  }

  /**
   * Set video mode and properties from a JSON configuration object.
   *
   * @param config configuration
   * @return True if set successfully
   */
  bool SetConfigJson(const wpi::util::json& config) {
    m_status = 0;
    return SetSourceConfigJson(m_handle, config, &m_status);
  }

  /**
   * Get a JSON configuration string.
   *
   * @return JSON configuration string
   */
  std::string GetConfigJson() const {
    m_status = 0;
    return GetSourceConfigJson(m_handle, &m_status);
  }

  /**
   * Get a JSON configuration object.
   *
   * @return JSON configuration object
   */
  wpi::util::json GetConfigJsonObject() const;

  /**
   * Get the actual FPS.
   *
   * <p>SetTelemetryPeriod() must be called for this to be valid.
   *
   * @return Actual FPS averaged over the telemetry period.
   */
  double GetActualFPS() const {
    m_status = 0;
    return wpi::cs::GetTelemetryAverageValue(
        m_handle, CS_SOURCE_FRAMES_RECEIVED, &m_status);
  }

  /**
   * Get the data rate (in bytes per second).
   *
   * <p>SetTelemetryPeriod() must be called for this to be valid.
   *
   * @return Data rate averaged over the telemetry period.
   */
  double GetActualDataRate() const {
    m_status = 0;
    return wpi::cs::GetTelemetryAverageValue(m_handle, CS_SOURCE_BYTES_RECEIVED,
                                             &m_status);
  }

  /**
   * Enumerate all known video modes for this source.
   */
  std::vector<VideoMode> EnumerateVideoModes() const {
    CS_Status status = 0;
    return EnumerateSourceVideoModes(m_handle, &status);
  }

  CS_Status GetLastStatus() const { return m_status; }

  /**
   * Enumerate all sinks connected to this source.
   *
   * @return Vector of sinks.
   */
  std::vector<VideoSink> EnumerateSinks();

  /**
   * Enumerate all existing sources.
   *
   * @return Vector of sources.
   */
  static std::vector<VideoSource> EnumerateSources();

  friend void swap(VideoSource& first, VideoSource& second) noexcept {
    using std::swap;
    swap(first.m_status, second.m_status);
    swap(first.m_handle, second.m_handle);
  }

 protected:
  explicit VideoSource(CS_Source handle) : m_handle(handle) {}

  mutable CS_Status m_status = 0;

  /// Video source handle.
  CS_Source m_handle{0};
};

}  // namespace wpi::cs
