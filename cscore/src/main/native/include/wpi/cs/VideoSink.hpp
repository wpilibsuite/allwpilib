// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>
#include <string_view>
#include <vector>

#include "wpi/cs/cscore_cpp.hpp"
#include "wpi/cs/VideoProperty.hpp"
#include "wpi/cs/VideoSource.hpp"
#include "wpi/util/json_fwd.hpp"

namespace wpi::cs {

class VideoEvent;

/**
 * A sink for video that accepts a sequence of frames.
 */
class VideoSink {
  friend class VideoEvent;
  friend class VideoSource;

 public:
  enum Kind {
    /// Unknown sink type.
    kUnknown = CS_SINK_UNKNOWN,
    /// MJPEG video sink.
    kMjpeg = CS_SINK_MJPEG,
    /// CV video sink.
    kCv = CS_SINK_CV,
    /// Raw video sink.
    kRaw = CS_SINK_RAW,
  };

  VideoSink() noexcept = default;

  VideoSink(const VideoSink& sink)
      : m_handle(sink.m_handle == 0 ? 0 : CopySink(sink.m_handle, &m_status)) {}

  VideoSink(VideoSink&& other) noexcept : VideoSink() { swap(*this, other); }

  VideoSink& operator=(VideoSink other) noexcept {
    swap(*this, other);
    return *this;
  }

  ~VideoSink() {
    m_status = 0;
    if (m_handle != 0) {
      ReleaseSink(m_handle, &m_status);
    }
  }

  /**
   * Returns true if the VideoSink is valid.
   *
   * @return True if the VideoSink is valid.
   */
  explicit operator bool() const { return m_handle != 0; }

  /**
   * Returns the VideoSink handle.
   *
   * @return The VideoSink handle.
   */
  int GetHandle() const { return m_handle; }

  bool operator==(const VideoSink& other) const {
    return m_handle == other.m_handle;
  }

  /**
   * Get the kind of the sink.
   */
  Kind GetKind() const {
    m_status = 0;
    return static_cast<VideoSink::Kind>(GetSinkKind(m_handle, &m_status));
  }

  /**
   * Get the name of the sink.  The name is an arbitrary identifier
   * provided when the sink is created, and should be unique.
   */
  std::string GetName() const {
    m_status = 0;
    return GetSinkName(m_handle, &m_status);
  }

  /**
   * Get the sink description.  This is sink-kind specific.
   */
  std::string GetDescription() const {
    m_status = 0;
    return GetSinkDescription(m_handle, &m_status);
  }

  /**
   * Get a property of the sink.
   *
   * @param name Property name
   * @return Property (kind Property::kNone if no property with
   *         the given name exists)
   */
  VideoProperty GetProperty(std::string_view name) {
    m_status = 0;
    return VideoProperty{GetSinkProperty(m_handle, name, &m_status)};
  }

  /**
   * Enumerate all properties of this sink.
   */
  std::vector<VideoProperty> EnumerateProperties() const;

  /**
   * Set properties from a JSON configuration string.
   *
   * The format of the JSON input is:
   *
   * <pre>
   * {
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
    return SetSinkConfigJson(m_handle, config, &m_status);
  }

  /**
   * Set properties from a JSON configuration object.
   *
   * @param config configuration
   * @return True if set successfully
   */
  bool SetConfigJson(const wpi::util::json& config) {
    m_status = 0;
    return SetSinkConfigJson(m_handle, config, &m_status);
  }

  /**
   * Get a JSON configuration string.
   *
   * @return JSON configuration string
   */
  std::string GetConfigJson() const {
    m_status = 0;
    return GetSinkConfigJson(m_handle, &m_status);
  }

  /**
   * Get a JSON configuration object.
   *
   * @return JSON configuration object
   */
  wpi::util::json GetConfigJsonObject() const;

  /**
   * Configure which source should provide frames to this sink.  Each sink
   * can accept frames from only a single source, but a single source can
   * provide frames to multiple clients.
   *
   * @param source Source
   */
  void SetSource(VideoSource source) {
    m_status = 0;
    if (!source) {
      SetSinkSource(m_handle, 0, &m_status);
    } else {
      SetSinkSource(m_handle, source.m_handle, &m_status);
    }
  }

  /**
   * Get the connected source.
   *
   * @return Connected source (empty if none connected).
   */
  VideoSource GetSource() const {
    m_status = 0;
    auto handle = GetSinkSource(m_handle, &m_status);
    return VideoSource{handle == 0 ? 0 : CopySource(handle, &m_status)};
  }

  /**
   * Get a property of the associated source.
   *
   * @param name Property name
   * @return Property (kind Property::kNone if no property with
   *         the given name exists or no source connected)
   */
  VideoProperty GetSourceProperty(std::string_view name) {
    m_status = 0;
    return VideoProperty{GetSinkSourceProperty(m_handle, name, &m_status)};
  }

  CS_Status GetLastStatus() const { return m_status; }

  /**
   * Enumerate all existing sinks.
   *
   * @return Vector of sinks.
   */
  static std::vector<VideoSink> EnumerateSinks();

  friend void swap(VideoSink& first, VideoSink& second) noexcept {
    using std::swap;
    swap(first.m_status, second.m_status);
    swap(first.m_handle, second.m_handle);
  }

 protected:
  explicit VideoSink(CS_Sink handle) : m_handle(handle) {}

  mutable CS_Status m_status = 0;
  CS_Sink m_handle{0};
};

}  // namespace wpi::cs
