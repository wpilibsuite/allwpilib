// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef CSCORE_CSCORE_CPP_H_
#define CSCORE_CSCORE_CPP_H_

#include <stdint.h>

#include <functional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include <wpi/RawFrame.h>
#include <wpi/SmallVector.h>
#include <wpi/json_fwd.h>

#include "cscore_c.h"

#ifdef _WIN32
// Disable uninitialized variable warnings
#pragma warning(push)
#pragma warning(disable : 26495)
#endif

/** CameraServer (cscore) namespace */
namespace cs {

/**
 * @defgroup cscore_cpp_api cscore C++ function API
 *
 * Handle-based interface for C++.  Users are encouraged to use the
 * object oriented interface instead; this interface is intended for use
 * in applications such as JNI which require handle-based access.
 *
 * @{
 */

/**
 * USB camera information
 */
struct UsbCameraInfo {
  /** Device number (e.g. N in '/dev/videoN' on Linux) */
  int dev = -1;
  /** Path to device if available (e.g. '/dev/video0' on Linux) */
  std::string path;
  /** Vendor/model name of the camera as provided by the USB driver */
  std::string name;
  /** Other path aliases to device (e.g. '/dev/v4l/by-id/...' etc on Linux) */
  std::vector<std::string> otherPaths;
  /** USB Vendor Id */
  int vendorId = -1;
  /** USB Product Id */
  int productId = -1;
};

/**
 * Video mode
 */
struct VideoMode : public CS_VideoMode {
  enum PixelFormat {
    kUnknown = WPI_PIXFMT_UNKNOWN,
    kMJPEG = WPI_PIXFMT_MJPEG,
    kYUYV = WPI_PIXFMT_YUYV,
    kRGB565 = WPI_PIXFMT_RGB565,
    kBGR = WPI_PIXFMT_BGR,
    kGray = WPI_PIXFMT_GRAY,
    kY16 = WPI_PIXFMT_Y16,
    kUYVY = WPI_PIXFMT_UYVY,
    kBGRA = WPI_PIXFMT_BGRA,
  };
  VideoMode() {
    pixelFormat = 0;
    width = 0;
    height = 0;
    fps = 0;
  }
  VideoMode(PixelFormat pixelFormat_, int width_, int height_, int fps_) {
    pixelFormat = pixelFormat_;
    width = width_;
    height = height_;
    fps = fps_;
  }
  explicit operator bool() const { return pixelFormat == kUnknown; }

  bool operator==(const VideoMode& other) const {
    return pixelFormat == other.pixelFormat && width == other.width &&
           height == other.height && fps == other.fps;
  }

  bool CompareWithoutFps(const VideoMode& other) const {
    return pixelFormat == other.pixelFormat && width == other.width &&
           height == other.height;
  }
};

/**
 * Listener event
 */
struct RawEvent {
  enum Kind {
    kSourceCreated = CS_SOURCE_CREATED,
    kSourceDestroyed = CS_SOURCE_DESTROYED,
    kSourceConnected = CS_SOURCE_CONNECTED,
    kSourceDisconnected = CS_SOURCE_DISCONNECTED,
    kSourceVideoModesUpdated = CS_SOURCE_VIDEOMODES_UPDATED,
    kSourceVideoModeChanged = CS_SOURCE_VIDEOMODE_CHANGED,
    kSourcePropertyCreated = CS_SOURCE_PROPERTY_CREATED,
    kSourcePropertyValueUpdated = CS_SOURCE_PROPERTY_VALUE_UPDATED,
    kSourcePropertyChoicesUpdated = CS_SOURCE_PROPERTY_CHOICES_UPDATED,
    kSinkSourceChanged = CS_SINK_SOURCE_CHANGED,
    kSinkCreated = CS_SINK_CREATED,
    kSinkDestroyed = CS_SINK_DESTROYED,
    kSinkEnabled = CS_SINK_ENABLED,
    kSinkDisabled = CS_SINK_DISABLED,
    kNetworkInterfacesChanged = CS_NETWORK_INTERFACES_CHANGED,
    kTelemetryUpdated = CS_TELEMETRY_UPDATED,
    kSinkPropertyCreated = CS_SINK_PROPERTY_CREATED,
    kSinkPropertyValueUpdated = CS_SINK_PROPERTY_VALUE_UPDATED,
    kSinkPropertyChoicesUpdated = CS_SINK_PROPERTY_CHOICES_UPDATED,
    kUsbCamerasChanged = CS_USB_CAMERAS_CHANGED
  };

  RawEvent() = default;
  explicit RawEvent(RawEvent::Kind kind_) : kind{kind_} {}
  RawEvent(std::string_view name_, CS_Handle handle_, RawEvent::Kind kind_)
      : kind{kind_}, name{name_} {
    if (kind_ == kSinkCreated || kind_ == kSinkDestroyed ||
        kind_ == kSinkEnabled || kind_ == kSinkDisabled) {
      sinkHandle = handle_;
    } else {
      sourceHandle = handle_;
    }
  }
  RawEvent(std::string_view name_, CS_Source source_, const VideoMode& mode_)
      : kind{kSourceVideoModeChanged},
        sourceHandle{source_},
        name{name_},
        mode{mode_} {}
  RawEvent(std::string_view name_, CS_Source source_, RawEvent::Kind kind_,
           CS_Property property_, CS_PropertyKind propertyKind_, int value_,
           std::string_view valueStr_)
      : kind{kind_},
        sourceHandle{source_},
        name{name_},
        propertyHandle{property_},
        propertyKind{propertyKind_},
        value{value_},
        valueStr{valueStr_} {}

  Kind kind;

  // Valid for kSource* and kSink* respectively
  CS_Source sourceHandle = CS_INVALID_HANDLE;
  CS_Sink sinkHandle = CS_INVALID_HANDLE;

  // Source/sink/property name
  std::string name;

  // Fields for kSourceVideoModeChanged event
  VideoMode mode;

  // Fields for kSourceProperty* events
  CS_Property propertyHandle;
  CS_PropertyKind propertyKind;
  int value;
  std::string valueStr;

  // Listener that was triggered
  CS_Listener listener{0};
};

/**
 * @defgroup cscore_property_func Property Functions
 * @{
 */
CS_PropertyKind GetPropertyKind(CS_Property property, CS_Status* status);
std::string GetPropertyName(CS_Property property, CS_Status* status);
std::string_view GetPropertyName(CS_Property property,
                                 wpi::SmallVectorImpl<char>& buf,
                                 CS_Status* status);
int GetProperty(CS_Property property, CS_Status* status);
void SetProperty(CS_Property property, int value, CS_Status* status);
int GetPropertyMin(CS_Property property, CS_Status* status);
int GetPropertyMax(CS_Property property, CS_Status* status);
int GetPropertyStep(CS_Property property, CS_Status* status);
int GetPropertyDefault(CS_Property property, CS_Status* status);
std::string GetStringProperty(CS_Property property, CS_Status* status);
std::string_view GetStringProperty(CS_Property property,
                                   wpi::SmallVectorImpl<char>& buf,
                                   CS_Status* status);
void SetStringProperty(CS_Property property, std::string_view value,
                       CS_Status* status);
std::vector<std::string> GetEnumPropertyChoices(CS_Property property,
                                                CS_Status* status);
/** @} */

/**
 * @defgroup cscore_source_create_func Source Creation Functions
 * @{
 */
CS_Source CreateUsbCameraDev(std::string_view name, int dev, CS_Status* status);
CS_Source CreateUsbCameraPath(std::string_view name, std::string_view path,
                              CS_Status* status);
CS_Source CreateHttpCamera(std::string_view name, std::string_view url,
                           CS_HttpCameraKind kind, CS_Status* status);
CS_Source CreateHttpCamera(std::string_view name,
                           std::span<const std::string> urls,
                           CS_HttpCameraKind kind, CS_Status* status);
CS_Source CreateCvSource(std::string_view name, const VideoMode& mode,
                         CS_Status* status);
/** @} */

/**
 * @defgroup cscore_source_func Source Functions
 * @{
 */
CS_SourceKind GetSourceKind(CS_Source source, CS_Status* status);
std::string GetSourceName(CS_Source source, CS_Status* status);
std::string_view GetSourceName(CS_Source source,
                               wpi::SmallVectorImpl<char>& buf,
                               CS_Status* status);
std::string GetSourceDescription(CS_Source source, CS_Status* status);
std::string_view GetSourceDescription(CS_Source source,
                                      wpi::SmallVectorImpl<char>& buf,
                                      CS_Status* status);
uint64_t GetSourceLastFrameTime(CS_Source source, CS_Status* status);
void SetSourceConnectionStrategy(CS_Source source,
                                 CS_ConnectionStrategy strategy,
                                 CS_Status* status);
bool IsSourceConnected(CS_Source source, CS_Status* status);
bool IsSourceEnabled(CS_Source source, CS_Status* status);
CS_Property GetSourceProperty(CS_Source source, std::string_view name,
                              CS_Status* status);
std::span<CS_Property> EnumerateSourceProperties(
    CS_Source source, wpi::SmallVectorImpl<CS_Property>& vec,
    CS_Status* status);
VideoMode GetSourceVideoMode(CS_Source source, CS_Status* status);
bool SetSourceVideoMode(CS_Source source, const VideoMode& mode,
                        CS_Status* status);
bool SetSourcePixelFormat(CS_Source source, VideoMode::PixelFormat pixelFormat,
                          CS_Status* status);
bool SetSourceResolution(CS_Source source, int width, int height,
                         CS_Status* status);
bool SetSourceFPS(CS_Source source, int fps, CS_Status* status);
bool SetSourceConfigJson(CS_Source source, std::string_view config,
                         CS_Status* status);
bool SetSourceConfigJson(CS_Source source, const wpi::json& config,
                         CS_Status* status);
std::string GetSourceConfigJson(CS_Source source, CS_Status* status);
wpi::json GetSourceConfigJsonObject(CS_Source source, CS_Status* status);
std::vector<VideoMode> EnumerateSourceVideoModes(CS_Source source,
                                                 CS_Status* status);
std::span<CS_Sink> EnumerateSourceSinks(CS_Source source,
                                        wpi::SmallVectorImpl<CS_Sink>& vec,
                                        CS_Status* status);
CS_Source CopySource(CS_Source source, CS_Status* status);
void ReleaseSource(CS_Source source, CS_Status* status);
/** @} */

/**
 * @defgroup cscore_camera_property_func Camera Source Common Property Functions
 * @{
 */
void SetCameraBrightness(CS_Source source, int brightness, CS_Status* status);
int GetCameraBrightness(CS_Source source, CS_Status* status);
void SetCameraWhiteBalanceAuto(CS_Source source, CS_Status* status);
void SetCameraWhiteBalanceHoldCurrent(CS_Source source, CS_Status* status);
void SetCameraWhiteBalanceManual(CS_Source source, int value,
                                 CS_Status* status);
void SetCameraExposureAuto(CS_Source source, CS_Status* status);
void SetCameraExposureHoldCurrent(CS_Source source, CS_Status* status);
void SetCameraExposureManual(CS_Source source, int value, CS_Status* status);
/** @} */

/**
 * @defgroup cscore_usbcamera_func UsbCamera Source Functions
 * @{
 */
void SetUsbCameraPath(CS_Source, std::string_view path, CS_Status* status);
std::string GetUsbCameraPath(CS_Source source, CS_Status* status);
UsbCameraInfo GetUsbCameraInfo(CS_Source source, CS_Status* status);
/** @} */

/**
 * @defgroup cscore_httpcamera_func HttpCamera Source Functions
 * @{
 */
CS_HttpCameraKind GetHttpCameraKind(CS_Source source, CS_Status* status);
void SetHttpCameraUrls(CS_Source source, std::span<const std::string> urls,
                       CS_Status* status);
std::vector<std::string> GetHttpCameraUrls(CS_Source source, CS_Status* status);
/** @} */

/**
 * @defgroup cscore_frame_source_func Frame Source Functions
 * @{
 */
void NotifySourceError(CS_Source source, std::string_view msg,
                       CS_Status* status);
void SetSourceConnected(CS_Source source, bool connected, CS_Status* status);
void SetSourceDescription(CS_Source source, std::string_view description,
                          CS_Status* status);
CS_Property CreateSourceProperty(CS_Source source, std::string_view name,
                                 CS_PropertyKind kind, int minimum, int maximum,
                                 int step, int defaultValue, int value,
                                 CS_Status* status);
void SetSourceEnumPropertyChoices(CS_Source source, CS_Property property,
                                  std::span<const std::string> choices,
                                  CS_Status* status);
/** @} */

/**
 * @defgroup cscore_sink_create_func Sink Creation Functions
 * @{
 */
CS_Sink CreateMjpegServer(std::string_view name, std::string_view listenAddress,
                          int port, CS_Status* status);
CS_Sink CreateCvSink(std::string_view name, VideoMode::PixelFormat pixelFormat,
                     CS_Status* status);
CS_Sink CreateCvSinkCallback(std::string_view name,
                             VideoMode::PixelFormat pixelFormat,
                             std::function<void(uint64_t time)> processFrame,
                             CS_Status* status);

/** @} */

/**
 * @defgroup cscore_sink_func Sink Functions
 * @{
 */
CS_SinkKind GetSinkKind(CS_Sink sink, CS_Status* status);
std::string GetSinkName(CS_Sink sink, CS_Status* status);
std::string_view GetSinkName(CS_Sink sink, wpi::SmallVectorImpl<char>& buf,
                             CS_Status* status);
std::string GetSinkDescription(CS_Sink sink, CS_Status* status);
std::string_view GetSinkDescription(CS_Sink sink,
                                    wpi::SmallVectorImpl<char>& buf,
                                    CS_Status* status);
CS_Property GetSinkProperty(CS_Sink sink, std::string_view name,
                            CS_Status* status);
std::span<CS_Property> EnumerateSinkProperties(
    CS_Sink sink, wpi::SmallVectorImpl<CS_Property>& vec, CS_Status* status);
void SetSinkSource(CS_Sink sink, CS_Source source, CS_Status* status);
CS_Property GetSinkSourceProperty(CS_Sink sink, std::string_view name,
                                  CS_Status* status);
bool SetSinkConfigJson(CS_Sink sink, std::string_view config,
                       CS_Status* status);
bool SetSinkConfigJson(CS_Sink sink, const wpi::json& config,
                       CS_Status* status);
std::string GetSinkConfigJson(CS_Sink sink, CS_Status* status);
wpi::json GetSinkConfigJsonObject(CS_Sink sink, CS_Status* status);
CS_Source GetSinkSource(CS_Sink sink, CS_Status* status);
CS_Sink CopySink(CS_Sink sink, CS_Status* status);
void ReleaseSink(CS_Sink sink, CS_Status* status);
/** @} */

/**
 * @defgroup cscore_mjpegserver_func MjpegServer Sink Functions
 * @{
 */
std::string GetMjpegServerListenAddress(CS_Sink sink, CS_Status* status);
int GetMjpegServerPort(CS_Sink sink, CS_Status* status);
/** @} */

/**
 * @defgroup cscore_frame_sink_func Frame Sink Functions
 * @{
 */
void SetSinkDescription(CS_Sink sink, std::string_view description,
                        CS_Status* status);
std::string GetSinkError(CS_Sink sink, CS_Status* status);
std::string_view GetSinkError(CS_Sink sink, wpi::SmallVectorImpl<char>& buf,
                              CS_Status* status);
void SetSinkEnabled(CS_Sink sink, bool enabled, CS_Status* status);
/** @} */

/**
 * @defgroup cscore_listener_func Listener Functions
 * @{
 */
void SetListenerOnStart(std::function<void()> onStart);
void SetListenerOnExit(std::function<void()> onExit);

CS_Listener AddListener(std::function<void(const RawEvent& event)> callback,
                        int eventMask, bool immediateNotify, CS_Status* status);

void RemoveListener(CS_Listener handle, CS_Status* status);

CS_ListenerPoller CreateListenerPoller();
void DestroyListenerPoller(CS_ListenerPoller poller);
CS_Listener AddPolledListener(CS_ListenerPoller poller, int eventMask,
                              bool immediateNotify, CS_Status* status);
std::vector<RawEvent> PollListener(CS_ListenerPoller poller);
std::vector<RawEvent> PollListener(CS_ListenerPoller poller, double timeout,
                                   bool* timedOut);
void CancelPollListener(CS_ListenerPoller poller);
/** @} */

bool NotifierDestroyed();

/**
 * @defgroup cscore_telemetry_func Telemetry Functions
 * @{
 */
void SetTelemetryPeriod(double seconds);
double GetTelemetryElapsedTime();
int64_t GetTelemetryValue(CS_Handle handle, CS_TelemetryKind kind,
                          CS_Status* status);
double GetTelemetryAverageValue(CS_Handle handle, CS_TelemetryKind kind,
                                CS_Status* status);
/** @} */

/**
 * @defgroup cscore_logging_func Logging Functions
 * @{
 */
using LogFunc = std::function<void(unsigned int level, const char* file,
                                   unsigned int line, const char* msg)>;
void SetLogger(LogFunc func, unsigned int min_level);
void SetDefaultLogger(unsigned int min_level);
/** @} */

/**
 * @defgroup cscore_shutdown_func Library Shutdown Function
 * @{
 */
void Shutdown();
/** @} */

/**
 * @defgroup cscore_utility_func Utility Functions
 * @{
 */
std::vector<UsbCameraInfo> EnumerateUsbCameras(CS_Status* status);

std::span<CS_Source> EnumerateSourceHandles(
    wpi::SmallVectorImpl<CS_Source>& vec, CS_Status* status);
std::span<CS_Sink> EnumerateSinkHandles(wpi::SmallVectorImpl<CS_Sink>& vec,
                                        CS_Status* status);

std::string GetHostname();

std::vector<std::string> GetNetworkInterfaces();
/** @} */

/** @} */

}  // namespace cs

#ifdef _WIN32
// Disable uninitialized variable warnings
#pragma warning(pop)
#endif

#endif  // CSCORE_CSCORE_CPP_H_
