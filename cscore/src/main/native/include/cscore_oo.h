// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef CSCORE_CSCORE_OO_H_
#define CSCORE_CSCORE_OO_H_

#include <functional>
#include <initializer_list>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <wpi/deprecated.h>

#include "cscore_cpp.h"

namespace cs {

/**
 * @defgroup cscore_oo cscore C++ object-oriented API
 *
 * Recommended interface for C++, identical to Java API.
 *
 * <p>The classes are RAII and handle reference counting internally.
 *
 * @{
 */

// Forward declarations so friend declarations work correctly
class ImageSource;
class VideoEvent;
class VideoSink;
class VideoSource;

/**
 * A source or sink property.
 */
class VideoProperty {
  friend class ImageSource;
  friend class VideoEvent;
  friend class VideoSink;
  friend class VideoSource;

 public:
  enum Kind {
    /// No specific property.
    kNone = CS_PROP_NONE,
    /// Boolean property.
    kBoolean = CS_PROP_BOOLEAN,
    /// Integer property.
    kInteger = CS_PROP_INTEGER,
    /// String property.
    kString = CS_PROP_STRING,
    /// Enum property.
    kEnum = CS_PROP_ENUM
  };

  VideoProperty() = default;

  /**
   * Returns property name.
   *
   * @return Property name.
   */
  std::string GetName() const {
    m_status = 0;
    return GetPropertyName(m_handle, &m_status);
  }

  /**
   * Returns property kind.
   *
   * @return Property kind.
   */
  Kind GetKind() const { return m_kind; }

  /**
   * Returns true if property is valid.
   *
   * @return True if property is valid.
   */
  explicit operator bool() const { return m_kind != kNone; }

  /**
   * Returns true if property is a boolean.
   *
   * @return True if property is a boolean.
   */
  bool IsBoolean() const { return m_kind == kBoolean; }

  /**
   * Returns true if property is an integer.
   *
   * @return True if property is an integer.
   */
  bool IsInteger() const { return m_kind == kInteger; }

  /**
   * Returns true if property is a string.
   *
   * @return True if property is a string.
   */
  bool IsString() const { return m_kind == kString; }

  /**
   * Returns true if property is an enum.
   *
   * @return True if property is an enum.
   */
  bool IsEnum() const { return m_kind == kEnum; }

  /**
   * Returns property value.
   *
   * @return Property value.
   */
  int Get() const {
    m_status = 0;
    return GetProperty(m_handle, &m_status);
  }

  /**
   * Sets property value.
   *
   * @param value Property value.
   */
  void Set(int value) {
    m_status = 0;
    SetProperty(m_handle, value, &m_status);
  }

  /**
   * Returns property minimum value.
   *
   * @return Property minimum value.
   */
  int GetMin() const {
    m_status = 0;
    return GetPropertyMin(m_handle, &m_status);
  }

  /**
   * Returns property maximum value.
   *
   * @return Property maximum value.
   */
  int GetMax() const {
    m_status = 0;
    return GetPropertyMax(m_handle, &m_status);
  }

  /**
   * Returns property step size.
   *
   * @return Property step size.
   */
  int GetStep() const {
    m_status = 0;
    return GetPropertyStep(m_handle, &m_status);
  }

  /**
   * Returns property default value.
   *
   * @return Property default value.
   */
  int GetDefault() const {
    m_status = 0;
    return GetPropertyDefault(m_handle, &m_status);
  }

  /**
   * Returns the string property value.
   *
   * <p>This function is string-specific.
   *
   * @return The string property value.
   */
  std::string GetString() const {
    m_status = 0;
    return GetStringProperty(m_handle, &m_status);
  }

  /**
   * Returns the string property value as a reference to the given buffer.
   *
   * This function is string-specific.
   *
   * @param buf The backing storage to which to write the property value.
   * @return The string property value as a reference to the given buffer.
   */
  std::string_view GetString(wpi::SmallVectorImpl<char>& buf) const {
    m_status = 0;
    return GetStringProperty(m_handle, buf, &m_status);
  }

  /**
   * Sets the string property value.
   *
   * This function is string-specific.
   *
   * @param value String property value.
   */
  void SetString(std::string_view value) {
    m_status = 0;
    SetStringProperty(m_handle, value, &m_status);
  }

  /**
   * Returns the possible values for the enum property value.
   *
   * This function is enum-specific.
   *
   * @return The possible values for the enum property value.
   */
  std::vector<std::string> GetChoices() const {
    m_status = 0;
    return GetEnumPropertyChoices(m_handle, &m_status);
  }

  /**
   * Returns the last status.
   *
   * @return The last status.
   */
  CS_Status GetLastStatus() const { return m_status; }

 private:
  explicit VideoProperty(CS_Property handle) : m_handle(handle) {
    m_status = 0;
    if (handle == 0) {
      m_kind = kNone;
    } else {
      m_kind = static_cast<Kind>(
          static_cast<int>(GetPropertyKind(handle, &m_status)));
    }
  }

  VideoProperty(CS_Property handle, Kind kind)
      : m_handle(handle), m_kind(kind) {}

  mutable CS_Status m_status{0};
  CS_Property m_handle{0};
  Kind m_kind{kNone};
};

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
   * This uses the same time base as wpi::Now().
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
  bool SetVideoMode(VideoMode::PixelFormat pixelFormat, int width, int height,
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
  bool SetPixelFormat(VideoMode::PixelFormat pixelFormat) {
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
  bool SetConfigJson(const wpi::json& config) {
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
  wpi::json GetConfigJsonObject() const;

  /**
   * Get the actual FPS.
   *
   * <p>SetTelemetryPeriod() must be called for this to be valid.
   *
   * @return Actual FPS averaged over the telemetry period.
   */
  double GetActualFPS() const {
    m_status = 0;
    return cs::GetTelemetryAverageValue(m_handle, CS_SOURCE_FRAMES_RECEIVED,
                                        &m_status);
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
    return cs::GetTelemetryAverageValue(m_handle, CS_SOURCE_BYTES_RECEIVED,
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

/**
 * A source that represents a video camera.
 */
class VideoCamera : public VideoSource {
 public:
  /**
   * White balance.
   */
  enum WhiteBalance {
    /// Fixed indoor white balance.
    kFixedIndoor = 3000,
    /// Fixed outdoor white balance 1.
    kFixedOutdoor1 = 4000,
    /// Fixed outdoor white balance 2.
    kFixedOutdoor2 = 5000,
    /// Fixed fluorescent white balance 1.
    kFixedFluorescent1 = 5100,
    /// Fixed fluorescent white balance 2.
    kFixedFlourescent2 = 5200
  };

  VideoCamera() = default;

  /**
   * Set the brightness, as a percentage (0-100).
   */
  void SetBrightness(int brightness) {
    m_status = 0;
    SetCameraBrightness(m_handle, brightness, &m_status);
  }

  /**
   * Get the brightness, as a percentage (0-100).
   */
  int GetBrightness() {
    m_status = 0;
    return GetCameraBrightness(m_handle, &m_status);
  }

  /**
   * Set the white balance to auto.
   */
  void SetWhiteBalanceAuto() {
    m_status = 0;
    SetCameraWhiteBalanceAuto(m_handle, &m_status);
  }

  /**
   * Set the white balance to hold current.
   */
  void SetWhiteBalanceHoldCurrent() {
    m_status = 0;
    SetCameraWhiteBalanceHoldCurrent(m_handle, &m_status);
  }

  /**
   * Set the white balance to manual, with specified color temperature.
   */
  void SetWhiteBalanceManual(int value) {
    m_status = 0;
    SetCameraWhiteBalanceManual(m_handle, value, &m_status);
  }

  /**
   * Set the exposure to auto aperture.
   */
  void SetExposureAuto() {
    m_status = 0;
    SetCameraExposureAuto(m_handle, &m_status);
  }

  /**
   * Set the exposure to hold current.
   */
  void SetExposureHoldCurrent() {
    m_status = 0;
    SetCameraExposureHoldCurrent(m_handle, &m_status);
  }

  /**
   * Set the exposure to manual, as a percentage (0-100).
   */
  void SetExposureManual(int value) {
    m_status = 0;
    SetCameraExposureManual(m_handle, value, &m_status);
  }

 protected:
  explicit VideoCamera(CS_Source handle) : VideoSource(handle) {}
};

/**
 * A source that represents a USB camera.
 */
class UsbCamera : public VideoCamera {
 public:
  UsbCamera() = default;

  /**
   * Create a source for a USB camera based on device number.
   *
   * @param name Source name (arbitrary unique identifier)
   * @param dev Device number (e.g. 0 for /dev/video0)
   */
  UsbCamera(std::string_view name, int dev) {
    m_handle = CreateUsbCameraDev(name, dev, &m_status);
  }

  /**
   * Create a source for a USB camera based on device path.
   *
   * @param name Source name (arbitrary unique identifier)
   * @param path Path to device (e.g. "/dev/video0" on Linux)
   */
  UsbCamera(std::string_view name, std::string_view path) {
    m_handle = CreateUsbCameraPath(name, path, &m_status);
  }

  /**
   * Enumerate USB cameras on the local system.
   *
   * @return Vector of USB camera information (one for each camera)
   */
  static std::vector<UsbCameraInfo> EnumerateUsbCameras() {
    CS_Status status = 0;
    return ::cs::EnumerateUsbCameras(&status);
  }

  /**
   * Change the path to the device.
   */
  void SetPath(std::string_view path) {
    m_status = 0;
    return ::cs::SetUsbCameraPath(m_handle, path, &m_status);
  }

  /**
   * Get the path to the device.
   */
  std::string GetPath() const {
    m_status = 0;
    return ::cs::GetUsbCameraPath(m_handle, &m_status);
  }

  /**
   * Get the full camera information for the device.
   */
  UsbCameraInfo GetInfo() const {
    m_status = 0;
    return ::cs::GetUsbCameraInfo(m_handle, &m_status);
  }

  /**
   * Set how verbose the camera connection messages are.
   *
   * @param level 0=don't display Connecting message, 1=do display message
   */
  void SetConnectVerbose(int level) {
    m_status = 0;
    SetProperty(GetSourceProperty(m_handle, "connect_verbose", &m_status),
                level, &m_status);
  }
};

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
    /// Axis camera.
    kAxis = CS_HTTP_AXIS
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
        static_cast<int>(::cs::GetHttpCameraKind(m_handle, &m_status)));
  }

  /**
   * Change the URLs used to connect to the camera.
   */
  void SetUrls(std::span<const std::string> urls) {
    m_status = 0;
    ::cs::SetHttpCameraUrls(m_handle, urls, &m_status);
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
    ::cs::SetHttpCameraUrls(m_handle, vec, &m_status);
  }

  /**
   * Get the URLs used to connect to the camera.
   */
  std::vector<std::string> GetUrls() const {
    m_status = 0;
    return ::cs::GetHttpCameraUrls(m_handle, &m_status);
  }
};

/**
 * A source that represents an Axis IP camera.
 *
 * @deprecated Use HttpCamera instead.
 */
class [[deprecated("Use HttpCamera instead.")]] AxisCamera : public HttpCamera {
  static std::string HostToUrl(std::string_view host);

  static std::vector<std::string> HostToUrl(
      std::span<const std::string> hosts) {
    std::vector<std::string> rv;
    rv.reserve(hosts.size());
    for (const auto& host : hosts) {
      rv.emplace_back(HostToUrl(std::string_view{host}));
    }
    return rv;
  }

  template <typename T>
  static std::vector<std::string> HostToUrl(std::initializer_list<T> hosts) {
    std::vector<std::string> rv;
    rv.reserve(hosts.size());
    for (const auto& host : hosts) {
      rv.emplace_back(HostToUrl(std::string_view{host}));
    }
    return rv;
  }

 public:
  /**
   * Create a source for an Axis IP camera.
   *
   * @param name Source name (arbitrary unique identifier)
   * @param host Camera host IP or DNS name (e.g. "10.x.y.11")
   */
  AxisCamera(std::string_view name, std::string_view host)
      : HttpCamera(name, HostToUrl(host), kAxis) {}

  /**
   * Create a source for an Axis IP camera.
   *
   * @param name Source name (arbitrary unique identifier)
   * @param host Camera host IP or DNS name (e.g. "10.x.y.11")
   */
  AxisCamera(std::string_view name, const char* host)
      : HttpCamera(name, HostToUrl(host), kAxis) {}

  /**
   * Create a source for an Axis IP camera.
   *
   * @param name Source name (arbitrary unique identifier)
   * @param host Camera host IP or DNS name (e.g. "10.x.y.11")
   */
  AxisCamera(std::string_view name, const std::string& host)
      : HttpCamera(name, HostToUrl(std::string_view{host}), kAxis) {}

  /**
   * Create a source for an Axis IP camera.
   *
   * @param name Source name (arbitrary unique identifier)
   * @param hosts Array of Camera host IPs/DNS names
   */
  AxisCamera(std::string_view name, std::span<const std::string> hosts)
      : HttpCamera(name, HostToUrl(hosts), kAxis) {}

  /**
   * Create a source for an Axis IP camera.
   *
   * @param name Source name (arbitrary unique identifier)
   * @param hosts Array of Camera host IPs/DNS names
   */
  template <typename T>
  AxisCamera(std::string_view name, std::initializer_list<T> hosts)
      : HttpCamera(name, HostToUrl(hosts), kAxis) {}
};

/**
 * A base class for single image providing sources.
 */
class ImageSource : public VideoSource {
 protected:
  ImageSource() = default;

 public:
  /**
   * Signal sinks that an error has occurred.  This should be called instead
   * of NotifyFrame when an error occurs.
   *
   * @param msg Notification message.
   */
  void NotifyError(std::string_view msg) {
    m_status = 0;
    NotifySourceError(m_handle, msg, &m_status);
  }

  /**
   * Set source connection status.  Defaults to true.
   *
   * @param connected True for connected, false for disconnected
   */
  void SetConnected(bool connected) {
    m_status = 0;
    SetSourceConnected(m_handle, connected, &m_status);
  }

  /**
   * Set source description.
   *
   * @param description Description
   */
  void SetDescription(std::string_view description) {
    m_status = 0;
    SetSourceDescription(m_handle, description, &m_status);
  }

  /**
   * Create a property.
   *
   * @param name Property name
   * @param kind Property kind
   * @param minimum Minimum value
   * @param maximum Maximum value
   * @param step Step value
   * @param defaultValue Default value
   * @param value Current value
   * @return Property
   */
  VideoProperty CreateProperty(std::string_view name, VideoProperty::Kind kind,
                               int minimum, int maximum, int step,
                               int defaultValue, int value) {
    m_status = 0;
    return VideoProperty{CreateSourceProperty(
        m_handle, name, static_cast<CS_PropertyKind>(static_cast<int>(kind)),
        minimum, maximum, step, defaultValue, value, &m_status)};
  }

  /**
   * Create an integer property.
   *
   * @param name Property name
   * @param minimum Minimum value
   * @param maximum Maximum value
   * @param step Step value
   * @param defaultValue Default value
   * @param value Current value
   * @return Property
   */
  VideoProperty CreateIntegerProperty(std::string_view name, int minimum,
                                      int maximum, int step, int defaultValue,
                                      int value) {
    m_status = 0;
    return VideoProperty{CreateSourceProperty(
        m_handle, name,
        static_cast<CS_PropertyKind>(
            static_cast<int>(VideoProperty::Kind::kInteger)),
        minimum, maximum, step, defaultValue, value, &m_status)};
  }

  /**
   * Create a boolean property.
   *
   * @param name Property name
   * @param defaultValue Default value
   * @param value Current value
   * @return Property
   */
  VideoProperty CreateBooleanProperty(std::string_view name, bool defaultValue,
                                      bool value) {
    m_status = 0;
    return VideoProperty{CreateSourceProperty(
        m_handle, name,
        static_cast<CS_PropertyKind>(
            static_cast<int>(VideoProperty::Kind::kBoolean)),
        0, 1, 1, defaultValue ? 1 : 0, value ? 1 : 0, &m_status)};
  }

  /**
   * Create a string property.
   *
   * @param name Property name
   * @param value Current value
   * @return Property
   */
  VideoProperty CreateStringProperty(std::string_view name,
                                     std::string_view value) {
    m_status = 0;
    auto prop = VideoProperty{CreateSourceProperty(
        m_handle, name,
        static_cast<CS_PropertyKind>(
            static_cast<int>(VideoProperty::Kind::kString)),
        0, 0, 0, 0, 0, &m_status)};
    prop.SetString(value);
    return prop;
  }

  /**
   * Configure enum property choices.
   *
   * @param property Property
   * @param choices Choices
   */
  void SetEnumPropertyChoices(const VideoProperty& property,
                              std::span<const std::string> choices) {
    m_status = 0;
    SetSourceEnumPropertyChoices(m_handle, property.m_handle, choices,
                                 &m_status);
  }

  /**
   * Configure enum property choices.
   *
   * @param property Property
   * @param choices Choices
   */
  template <typename T>
  void SetEnumPropertyChoices(const VideoProperty& property,
                              std::initializer_list<T> choices) {
    std::vector<std::string> vec;
    vec.reserve(choices.size());
    for (const auto& choice : choices) {
      vec.emplace_back(choice);
    }
    m_status = 0;
    SetSourceEnumPropertyChoices(m_handle, property.m_handle, vec, &m_status);
  }
};

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
  bool SetConfigJson(const wpi::json& config) {
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
  wpi::json GetConfigJsonObject() const;

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
    return cs::GetMjpegServerListenAddress(m_handle, &m_status);
  }

  /**
   * Get the port number of the server.
   */
  int GetPort() const {
    m_status = 0;
    return cs::GetMjpegServerPort(m_handle, &m_status);
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

/**
 * A base class for single image reading sinks.
 */
class ImageSink : public VideoSink {
 protected:
  ImageSink() = default;

 public:
  /**
   * Set sink description.
   *
   * @param description Description
   */
  void SetDescription(std::string_view description) {
    m_status = 0;
    SetSinkDescription(m_handle, description, &m_status);
  }

  /**
   * Get error string.  Call this if WaitForFrame() returns 0 to determine
   * what the error is.
   */
  std::string GetError() const {
    m_status = 0;
    return GetSinkError(m_handle, &m_status);
  }

  /**
   * Enable or disable getting new frames.
   *
   * <p>Disabling will cause processFrame (for callback-based CvSinks) to not
   * be called and WaitForFrame() to not return.  This can be used to save
   * processor resources when frames are not needed.
   */
  void SetEnabled(bool enabled) {
    m_status = 0;
    SetSinkEnabled(m_handle, enabled, &m_status);
  }
};

/**
 * An event generated by the library and provided to event listeners.
 */
class VideoEvent : public RawEvent {
 public:
  /**
   * Returns the source associated with the event (if any).
   *
   * @return The source associated with the event (if any).
   */
  VideoSource GetSource() const {
    CS_Status status = 0;
    return VideoSource{sourceHandle == 0 ? 0
                                         : CopySource(sourceHandle, &status)};
  }

  /**
   * Returns the sink associated with the event (if any).
   *
   * @return The sink associated with the event (if any).
   */
  VideoSink GetSink() const {
    CS_Status status = 0;
    return VideoSink{sinkHandle == 0 ? 0 : CopySink(sinkHandle, &status)};
  }

  /**
   * Returns the property associated with the event (if any).
   *
   * @return The property associated with the event (if any).
   */
  VideoProperty GetProperty() const {
    return VideoProperty{propertyHandle,
                         static_cast<VideoProperty::Kind>(propertyKind)};
  }
};

/**
 * An event listener.  This calls back to a designated callback function when
 * an event matching the specified mask is generated by the library.
 */
class VideoListener {
 public:
  VideoListener() = default;

  /**
   * Create an event listener.
   *
   * @param callback Callback function
   * @param eventMask Bitmask of VideoEvent::Kind values
   * @param immediateNotify Whether callback should be immediately called with
   *        a representative set of events for the current library state.
   */
  VideoListener(std::function<void(const VideoEvent& event)> callback,
                int eventMask, bool immediateNotify) {
    CS_Status status = 0;
    m_handle = AddListener(
        [=](const RawEvent& event) {
          callback(static_cast<const VideoEvent&>(event));
        },
        eventMask, immediateNotify, &status);
  }

  VideoListener(const VideoListener&) = delete;
  VideoListener& operator=(const VideoListener&) = delete;

  VideoListener(VideoListener&& other) noexcept : VideoListener() {
    swap(*this, other);
  }

  VideoListener& operator=(VideoListener&& other) noexcept {
    swap(*this, other);
    return *this;
  }

  ~VideoListener() {
    CS_Status status = 0;
    if (m_handle != 0) {
      RemoveListener(m_handle, &status);
    }
  }

  friend void swap(VideoListener& first, VideoListener& second) noexcept {
    using std::swap;
    swap(first.m_handle, second.m_handle);
  }

 private:
  CS_Listener m_handle{0};
};

/** @} */

}  // namespace cs

#endif  // CSCORE_CSCORE_OO_H_
