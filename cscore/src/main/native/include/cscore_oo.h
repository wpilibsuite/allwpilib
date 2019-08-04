/*----------------------------------------------------------------------------*/
/* Copyright (c) 2015-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef CSCORE_CSCORE_OO_H_
#define CSCORE_CSCORE_OO_H_

#include <initializer_list>
#include <string>
#include <utility>
#include <vector>

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
    kNone = CS_PROP_NONE,
    kBoolean = CS_PROP_BOOLEAN,
    kInteger = CS_PROP_INTEGER,
    kString = CS_PROP_STRING,
    kEnum = CS_PROP_ENUM
  };

  VideoProperty() : m_status(0), m_handle(0), m_kind(kNone) {}

  std::string GetName() const;

  Kind GetKind() const { return m_kind; }

  explicit operator bool() const { return m_kind != kNone; }

  // Kind checkers
  bool IsBoolean() const { return m_kind == kBoolean; }
  bool IsInteger() const { return m_kind == kInteger; }
  bool IsString() const { return m_kind == kString; }
  bool IsEnum() const { return m_kind == kEnum; }

  int Get() const;
  void Set(int value);
  int GetMin() const;
  int GetMax() const;
  int GetStep() const;
  int GetDefault() const;

  // String-specific functions
  std::string GetString() const;
  wpi::StringRef GetString(wpi::SmallVectorImpl<char>& buf) const;
  void SetString(const wpi::Twine& value);

  // Enum-specific functions
  std::vector<std::string> GetChoices() const;

  CS_Status GetLastStatus() const { return m_status; }

 private:
  explicit VideoProperty(CS_Property handle);
  VideoProperty(CS_Property handle, Kind kind);

  mutable CS_Status m_status;
  CS_Property m_handle;
  Kind m_kind;
};

/**
 * A source for video that provides a sequence of frames.
 */
class VideoSource {
  friend class VideoEvent;
  friend class VideoSink;

 public:
  enum Kind {
    kUnknown = CS_SOURCE_UNKNOWN,
    kUsb = CS_SOURCE_USB,
    kHttp = CS_SOURCE_HTTP,
    kCv = CS_SOURCE_CV
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

  VideoSource() noexcept : m_handle(0) {}
  VideoSource(const VideoSource& source);
  VideoSource(VideoSource&& other) noexcept;
  VideoSource& operator=(VideoSource other) noexcept;
  ~VideoSource();

  explicit operator bool() const { return m_handle != 0; }

  int GetHandle() const { return m_handle; }

  bool operator==(const VideoSource& other) const {
    return m_handle == other.m_handle;
  }

  bool operator!=(const VideoSource& other) const { return !(*this == other); }

  /**
   * Get the kind of the source.
   */
  Kind GetKind() const;

  /**
   * Get the name of the source.  The name is an arbitrary identifier
   * provided when the source is created, and should be unique.
   */
  std::string GetName() const;

  /**
   * Get the source description.  This is source-kind specific.
   */
  std::string GetDescription() const;

  /**
   * Get the last time a frame was captured.
   * This uses the same time base as wpi::Now().
   *
   * @return Time in 1 us increments.
   */
  uint64_t GetLastFrameTime() const;

  /**
   * Sets the connection strategy.  By default, the source will automatically
   * connect or disconnect based on whether any sinks are connected.
   *
   * <p>This function is non-blocking; look for either a connection open or
   * close event or call IsConnected() to determine the connection state.
   *
   * @param strategy connection strategy (auto, keep open, or force close)
   */
  void SetConnectionStrategy(ConnectionStrategy strategy);

  /**
   * Is the source currently connected to whatever is providing the images?
   */
  bool IsConnected() const;

  /**
   * Gets source enable status.  This is determined with a combination of
   * connection strategy and the number of sinks connected.
   *
   * @return True if enabled, false otherwise.
   */
  bool IsEnabled() const;

  /** Get a property.
   *
   * @param name Property name
   * @return Property contents (of kind Property::kNone if no property with
   *         the given name exists)
   */
  VideoProperty GetProperty(const wpi::Twine& name);

  /**
   * Enumerate all properties of this source.
   */
  std::vector<VideoProperty> EnumerateProperties() const;

  /**
   * Get the current video mode.
   */
  VideoMode GetVideoMode() const;

  /**
   * Set the video mode.
   *
   * @param mode Video mode
   */
  bool SetVideoMode(const VideoMode& mode);

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
                    int fps);

  /**
   * Set the pixel format.
   *
   * @param pixelFormat desired pixel format
   * @return True if set successfully
   */
  bool SetPixelFormat(VideoMode::PixelFormat pixelFormat);

  /**
   * Set the resolution.
   *
   * @param width desired width
   * @param height desired height
   * @return True if set successfully
   */
  bool SetResolution(int width, int height);

  /**
   * Set the frames per second (FPS).
   *
   * @param fps desired FPS
   * @return True if set successfully
   */
  bool SetFPS(int fps);

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
  bool SetConfigJson(wpi::StringRef config);

  /**
   * Set video mode and properties from a JSON configuration object.
   *
   * @param config configuration
   * @return True if set successfully
   */
  bool SetConfigJson(const wpi::json& config);

  /**
   * Get a JSON configuration string.
   *
   * @return JSON configuration string
   */
  std::string GetConfigJson() const;

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
  double GetActualFPS() const;

  /**
   * Get the data rate (in bytes per second).
   *
   * <p>SetTelemetryPeriod() must be called for this to be valid.
   *
   * @return Data rate averaged over the telemetry period.
   */
  double GetActualDataRate() const;

  /**
   * Enumerate all known video modes for this source.
   */
  std::vector<VideoMode> EnumerateVideoModes() const;

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
  CS_Source m_handle;
};

/**
 * A source that represents a video camera.
 */
class VideoCamera : public VideoSource {
 public:
  enum WhiteBalance {
    kFixedIndoor = 3000,
    kFixedOutdoor1 = 4000,
    kFixedOutdoor2 = 5000,
    kFixedFluorescent1 = 5100,
    kFixedFlourescent2 = 5200
  };

  VideoCamera() = default;

  /**
   * Set the brightness, as a percentage (0-100).
   */
  void SetBrightness(int brightness);

  /**
   * Get the brightness, as a percentage (0-100).
   */
  int GetBrightness();

  /**
   * Set the white balance to auto.
   */
  void SetWhiteBalanceAuto();

  /**
   * Set the white balance to hold current.
   */
  void SetWhiteBalanceHoldCurrent();

  /**
   * Set the white balance to manual, with specified color temperature.
   */
  void SetWhiteBalanceManual(int value);

  /**
   * Set the exposure to auto aperature.
   */
  void SetExposureAuto();

  /**
   * Set the exposure to hold current.
   */
  void SetExposureHoldCurrent();

  /**
   * Set the exposure to manual, as a percentage (0-100).
   */
  void SetExposureManual(int value);

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
  UsbCamera(const wpi::Twine& name, int dev);

  /**
   * Create a source for a USB camera based on device path.
   *
   * @param name Source name (arbitrary unique identifier)
   * @param path Path to device (e.g. "/dev/video0" on Linux)
   */
  UsbCamera(const wpi::Twine& name, const wpi::Twine& path);

  /**
   * Enumerate USB cameras on the local system.
   *
   * @return Vector of USB camera information (one for each camera)
   */
  static std::vector<UsbCameraInfo> EnumerateUsbCameras();

  /**
   * Get the path to the device.
   */
  std::string GetPath() const;

  /**
   * Get the full camera information for the device.
   */
  UsbCameraInfo GetInfo() const;

  /**
   * Set how verbose the camera connection messages are.
   *
   * @param level 0=don't display Connecting message, 1=do display message
   */
  void SetConnectVerbose(int level);
};

/**
 * A source that represents a MJPEG-over-HTTP (IP) camera.
 */
class HttpCamera : public VideoCamera {
 public:
  enum HttpCameraKind {
    kUnknown = CS_HTTP_UNKNOWN,
    kMJPGStreamer = CS_HTTP_MJPGSTREAMER,
    kCSCore = CS_HTTP_CSCORE,
    kAxis = CS_HTTP_AXIS
  };

  /**
   * Create a source for a MJPEG-over-HTTP (IP) camera.
   *
   * @param name Source name (arbitrary unique identifier)
   * @param url Camera URL (e.g. "http://10.x.y.11/video/stream.mjpg")
   * @param kind Camera kind (e.g. kAxis)
   */
  HttpCamera(const wpi::Twine& name, const wpi::Twine& url,
             HttpCameraKind kind = kUnknown);

  /**
   * Create a source for a MJPEG-over-HTTP (IP) camera.
   *
   * @param name Source name (arbitrary unique identifier)
   * @param url Camera URL (e.g. "http://10.x.y.11/video/stream.mjpg")
   * @param kind Camera kind (e.g. kAxis)
   */
  HttpCamera(const wpi::Twine& name, const char* url,
             HttpCameraKind kind = kUnknown);

  /**
   * Create a source for a MJPEG-over-HTTP (IP) camera.
   *
   * @param name Source name (arbitrary unique identifier)
   * @param url Camera URL (e.g. "http://10.x.y.11/video/stream.mjpg")
   * @param kind Camera kind (e.g. kAxis)
   */
  HttpCamera(const wpi::Twine& name, const std::string& url,
             HttpCameraKind kind = kUnknown);

  /**
   * Create a source for a MJPEG-over-HTTP (IP) camera.
   *
   * @param name Source name (arbitrary unique identifier)
   * @param urls Array of Camera URLs
   * @param kind Camera kind (e.g. kAxis)
   */
  HttpCamera(const wpi::Twine& name, wpi::ArrayRef<std::string> urls,
             HttpCameraKind kind = kUnknown);

  /**
   * Create a source for a MJPEG-over-HTTP (IP) camera.
   *
   * @param name Source name (arbitrary unique identifier)
   * @param urls Array of Camera URLs
   * @param kind Camera kind (e.g. kAxis)
   */
  template <typename T>
  HttpCamera(const wpi::Twine& name, std::initializer_list<T> urls,
             HttpCameraKind kind = kUnknown);

  /**
   * Get the kind of HTTP camera.
   *
   * <p>Autodetection can result in returning a different value than the camera
   * was created with.
   */
  HttpCameraKind GetHttpCameraKind() const;

  /**
   * Change the URLs used to connect to the camera.
   */
  void SetUrls(wpi::ArrayRef<std::string> urls);

  /**
   * Change the URLs used to connect to the camera.
   */
  template <typename T>
  void SetUrls(std::initializer_list<T> urls);

  /**
   * Get the URLs used to connect to the camera.
   */
  std::vector<std::string> GetUrls() const;
};

/**
 * A source that represents an Axis IP camera.
 */
class AxisCamera : public HttpCamera {
  static std::string HostToUrl(const wpi::Twine& host);
  static std::vector<std::string> HostToUrl(wpi::ArrayRef<std::string> hosts);
  template <typename T>
  static std::vector<std::string> HostToUrl(std::initializer_list<T> hosts);

 public:
  /**
   * Create a source for an Axis IP camera.
   *
   * @param name Source name (arbitrary unique identifier)
   * @param host Camera host IP or DNS name (e.g. "10.x.y.11")
   * @param kind Camera kind (e.g. kAxis)
   */
  AxisCamera(const wpi::Twine& name, const wpi::Twine& host);

  /**
   * Create a source for an Axis IP camera.
   *
   * @param name Source name (arbitrary unique identifier)
   * @param host Camera host IP or DNS name (e.g. "10.x.y.11")
   * @param kind Camera kind (e.g. kAxis)
   */
  AxisCamera(const wpi::Twine& name, const char* host);

  /**
   * Create a source for an Axis IP camera.
   *
   * @param name Source name (arbitrary unique identifier)
   * @param host Camera host IP or DNS name (e.g. "10.x.y.11")
   * @param kind Camera kind (e.g. kAxis)
   */
  AxisCamera(const wpi::Twine& name, const std::string& host);

  /**
   * Create a source for an Axis IP camera.
   *
   * @param name Source name (arbitrary unique identifier)
   * @param host Camera host IP or DNS name (e.g. "10.x.y.11")
   * @param kind Camera kind (e.g. kAxis)
   */
  AxisCamera(const wpi::Twine& name, wpi::StringRef host);

  /**
   * Create a source for an Axis IP camera.
   *
   * @param name Source name (arbitrary unique identifier)
   * @param hosts Array of Camera host IPs/DNS names
   * @param kind Camera kind (e.g. kAxis)
   */
  AxisCamera(const wpi::Twine& name, wpi::ArrayRef<std::string> hosts);

  /**
   * Create a source for an Axis IP camera.
   *
   * @param name Source name (arbitrary unique identifier)
   * @param hosts Array of Camera host IPs/DNS names
   * @param kind Camera kind (e.g. kAxis)
   */
  template <typename T>
  AxisCamera(const wpi::Twine& name, std::initializer_list<T> hosts);
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
   */
  void NotifyError(const wpi::Twine& msg);

  /**
   * Set source connection status.  Defaults to true.
   *
   * @param connected True for connected, false for disconnected
   */
  void SetConnected(bool connected);

  /**
   * Set source description.
   *
   * @param description Description
   */
  void SetDescription(const wpi::Twine& description);

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
  VideoProperty CreateProperty(const wpi::Twine& name, VideoProperty::Kind kind,
                               int minimum, int maximum, int step,
                               int defaultValue, int value);

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
  VideoProperty CreateIntegerProperty(const wpi::Twine& name, int minimum,
                                      int maximum, int step, int defaultValue,
                                      int value);

  /**
   * Create a boolean property.
   *
   * @param name Property name
   * @param defaultValue Default value
   * @param value Current value
   * @return Property
   */
  VideoProperty CreateBooleanProperty(const wpi::Twine& name, bool defaultValue,
                                      bool value);

  /**
   * Create a string property.
   *
   * @param name Property name
   * @param defaultValue Default value
   * @param value Current value
   * @return Property
   */
  VideoProperty CreateStringProperty(const wpi::Twine& name,
                                     const wpi::Twine& value);

  /**
   * Configure enum property choices.
   *
   * @param property Property
   * @param choices Choices
   */
  void SetEnumPropertyChoices(const VideoProperty& property,
                              wpi::ArrayRef<std::string> choices);

  /**
   * Configure enum property choices.
   *
   * @param property Property
   * @param choices Choices
   */
  template <typename T>
  void SetEnumPropertyChoices(const VideoProperty& property,
                              std::initializer_list<T> choices);
};

/**
 * A sink for video that accepts a sequence of frames.
 */
class VideoSink {
  friend class VideoEvent;
  friend class VideoSource;

 public:
  enum Kind {
    kUnknown = CS_SINK_UNKNOWN,
    kMjpeg = CS_SINK_MJPEG,
    kCv = CS_SINK_CV
  };

  VideoSink() noexcept : m_handle(0) {}
  VideoSink(const VideoSink& sink);
  VideoSink(VideoSink&& sink) noexcept;
  VideoSink& operator=(VideoSink other) noexcept;
  ~VideoSink();

  explicit operator bool() const { return m_handle != 0; }

  int GetHandle() const { return m_handle; }

  bool operator==(const VideoSink& other) const {
    return m_handle == other.m_handle;
  }

  bool operator!=(const VideoSink& other) const { return !(*this == other); }

  /**
   * Get the kind of the sink.
   */
  Kind GetKind() const;

  /**
   * Get the name of the sink.  The name is an arbitrary identifier
   * provided when the sink is created, and should be unique.
   */
  std::string GetName() const;

  /**
   * Get the sink description.  This is sink-kind specific.
   */
  std::string GetDescription() const;

  /**
   * Get a property of the sink.
   *
   * @param name Property name
   * @return Property (kind Property::kNone if no property with
   *         the given name exists)
   */
  VideoProperty GetProperty(const wpi::Twine& name);

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
  bool SetConfigJson(wpi::StringRef config);

  /**
   * Set properties from a JSON configuration object.
   *
   * @param config configuration
   * @return True if set successfully
   */
  bool SetConfigJson(const wpi::json& config);

  /**
   * Get a JSON configuration string.
   *
   * @return JSON configuration string
   */
  std::string GetConfigJson() const;

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
  void SetSource(VideoSource source);

  /**
   * Get the connected source.
   *
   * @return Connected source (empty if none connected).
   */
  VideoSource GetSource() const;

  /**
   * Get a property of the associated source.
   *
   * @param name Property name
   * @return Property (kind Property::kNone if no property with
   *         the given name exists or no source connected)
   */
  VideoProperty GetSourceProperty(const wpi::Twine& name);

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
  CS_Sink m_handle;
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
  MjpegServer(const wpi::Twine& name, const wpi::Twine& listenAddress,
              int port);

  /**
   * Create a MJPEG-over-HTTP server sink.
   *
   * @param name Sink name (arbitrary unique identifier)
   * @param port TCP port number
   */
  MjpegServer(const wpi::Twine& name, int port) : MjpegServer(name, "", port) {}

  /**
   * Get the listen address of the server.
   */
  std::string GetListenAddress() const;

  /**
   * Get the port number of the server.
   */
  int GetPort() const;

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
  void SetResolution(int width, int height);

  /**
   * Set the stream frames per second (FPS) for clients that don't specify it.
   *
   * <p>It is not necessary to set this if it is the same as the source FPS.
   *
   * @param fps FPS, 0 for unspecified
   */
  void SetFPS(int fps);

  /**
   * Set the compression for clients that don't specify it.
   *
   * <p>Setting this will result in increased CPU usage for MJPEG source cameras
   * as it will decompress and recompress the image instead of using the
   * camera's MJPEG image directly.
   *
   * @param quality JPEG compression quality (0-100), -1 for unspecified
   */
  void SetCompression(int quality);

  /**
   * Set the default compression used for non-MJPEG sources.  If not set,
   * 80 is used.  This function has no effect on MJPEG source cameras; use
   * SetCompression() instead to force recompression of MJPEG source images.
   *
   * @param quality JPEG compression quality (0-100)
   */
  void SetDefaultCompression(int quality);
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
  void SetDescription(const wpi::Twine& description);

  /**
   * Get error string.  Call this if WaitForFrame() returns 0 to determine
   * what the error is.
   */
  std::string GetError() const;

  /**
   * Enable or disable getting new frames.
   *
   * <p>Disabling will cause processFrame (for callback-based CvSinks) to not
   * be called and WaitForFrame() to not return.  This can be used to save
   * processor resources when frames are not needed.
   */
  void SetEnabled(bool enabled);
};

/**
 * An event generated by the library and provided to event listeners.
 */
class VideoEvent : public RawEvent {
 public:
  /**
   * Get the source associated with the event (if any).
   */
  VideoSource GetSource() const;

  /**
   * Get the sink associated with the event (if any).
   */
  VideoSink GetSink() const;

  /**
   * Get the property associated with the event (if any).
   */
  VideoProperty GetProperty() const;
};

/**
 * An event listener.  This calls back to a desigated callback function when
 * an event matching the specified mask is generated by the library.
 */
class VideoListener {
 public:
  VideoListener() : m_handle(0) {}

  /**
   * Create an event listener.
   *
   * @param callback Callback function
   * @param eventMask Bitmask of VideoEvent::Kind values
   * @param immediateNotify Whether callback should be immediately called with
   *        a representative set of events for the current library state.
   */
  VideoListener(std::function<void(const VideoEvent& event)> callback,
                int eventMask, bool immediateNotify);

  VideoListener(const VideoListener&) = delete;
  VideoListener& operator=(const VideoListener&) = delete;
  VideoListener(VideoListener&& other) noexcept;
  VideoListener& operator=(VideoListener&& other) noexcept;
  ~VideoListener();

  friend void swap(VideoListener& first, VideoListener& second) noexcept {
    using std::swap;
    swap(first.m_handle, second.m_handle);
  }

 private:
  CS_Listener m_handle;
};

/** @} */

}  // namespace cs

#include "cscore_oo.inl"

#endif  // CSCORE_CSCORE_OO_H_
