/*----------------------------------------------------------------------------*/
/* Copyright (c) 2015-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef CSCORE_CSCORE_CPP_H_
#define CSCORE_CSCORE_CPP_H_

#include <stdint.h>

#include <functional>
#include <string>
#include <vector>

#include <llvm/ArrayRef.h>
#include <llvm/SmallVector.h>
#include <llvm/StringRef.h>

#include "cscore_c.h"

namespace cv {
class Mat;
}  // namespace cv

namespace cs {

//
// Handle-based interface for C++.  Users are encouraged to use the
// object oriented interface instead; this interface is intended for use
// in applications such as JNI which require handle-based access.
//

/// USB camera information
struct UsbCameraInfo {
  /// Device number (e.g. N in '/dev/videoN' on Linux)
  int dev;
  /// Path to device if available (e.g. '/dev/video0' on Linux)
  std::string path;
  /// Vendor/model name of the camera as provided by the USB driver
  std::string name;
};

/// Video mode
struct VideoMode : public CS_VideoMode {
  enum PixelFormat {
    kUnknown = CS_PIXFMT_UNKNOWN,
    kMJPEG = CS_PIXFMT_MJPEG,
    kYUYV = CS_PIXFMT_YUYV,
    kRGB565 = CS_PIXFMT_RGB565,
    kBGR = CS_PIXFMT_BGR,
    kGray = CS_PIXFMT_GRAY
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
};

/// Listener event
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
    kNetworkInterfacesChanged = CS_NETWORK_INTERFACES_CHANGED
  };

  RawEvent() = default;
  explicit RawEvent(RawEvent::Kind kind_) : kind{kind_} {}
  RawEvent(llvm::StringRef name_, CS_Handle handle_, RawEvent::Kind kind_)
      : kind{kind_}, name{name_} {
    if (kind_ == kSinkCreated || kind_ == kSinkDestroyed ||
        kind_ == kSinkEnabled || kind_ == kSinkDisabled)
      sinkHandle = handle_;
    else
      sourceHandle = handle_;
  }
  RawEvent(llvm::StringRef name_, CS_Source source_, const VideoMode& mode_)
      : kind{kSourceVideoModeChanged},
        sourceHandle{source_},
        name{name_},
        mode{mode_} {}
  RawEvent(llvm::StringRef name_, CS_Source source_, RawEvent::Kind kind_,
           CS_Property property_, CS_PropertyKind propertyKind_, int value_,
           llvm::StringRef valueStr_)
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
};

//
// Property Functions
//
CS_PropertyKind GetPropertyKind(CS_Property property, CS_Status* status);
std::string GetPropertyName(CS_Property property, CS_Status* status);
llvm::StringRef GetPropertyName(CS_Property property,
                                llvm::SmallVectorImpl<char>& buf,
                                CS_Status* status);
int GetProperty(CS_Property property, CS_Status* status);
void SetProperty(CS_Property property, int value, CS_Status* status);
int GetPropertyMin(CS_Property property, CS_Status* status);
int GetPropertyMax(CS_Property property, CS_Status* status);
int GetPropertyStep(CS_Property property, CS_Status* status);
int GetPropertyDefault(CS_Property property, CS_Status* status);
std::string GetStringProperty(CS_Property property, CS_Status* status);
llvm::StringRef GetStringProperty(CS_Property property,
                                  llvm::SmallVectorImpl<char>& buf,
                                  CS_Status* status);
void SetStringProperty(CS_Property property, llvm::StringRef value,
                       CS_Status* status);
std::vector<std::string> GetEnumPropertyChoices(CS_Property property,
                                                CS_Status* status);

//
// Source Creation Functions
//
CS_Source CreateUsbCameraDev(llvm::StringRef name, int dev, CS_Status* status);
CS_Source CreateUsbCameraPath(llvm::StringRef name, llvm::StringRef path,
                              CS_Status* status);
CS_Source CreateHttpCamera(llvm::StringRef name, llvm::StringRef url,
                           CS_HttpCameraKind kind, CS_Status* status);
CS_Source CreateHttpCamera(llvm::StringRef name,
                           llvm::ArrayRef<std::string> urls,
                           CS_HttpCameraKind kind, CS_Status* status);
CS_Source CreateCvSource(llvm::StringRef name, const VideoMode& mode,
                         CS_Status* status);

//
// Source Functions
//
CS_SourceKind GetSourceKind(CS_Source source, CS_Status* status);
std::string GetSourceName(CS_Source source, CS_Status* status);
llvm::StringRef GetSourceName(CS_Source source,
                              llvm::SmallVectorImpl<char>& buf,
                              CS_Status* status);
std::string GetSourceDescription(CS_Source source, CS_Status* status);
llvm::StringRef GetSourceDescription(CS_Source source,
                                     llvm::SmallVectorImpl<char>& buf,
                                     CS_Status* status);
uint64_t GetSourceLastFrameTime(CS_Source source, CS_Status* status);
bool IsSourceConnected(CS_Source source, CS_Status* status);
CS_Property GetSourceProperty(CS_Source source, llvm::StringRef name,
                              CS_Status* status);
llvm::ArrayRef<CS_Property> EnumerateSourceProperties(
    CS_Source source, llvm::SmallVectorImpl<CS_Property>& vec,
    CS_Status* status);
VideoMode GetSourceVideoMode(CS_Source source, CS_Status* status);
bool SetSourceVideoMode(CS_Source source, const VideoMode& mode,
                        CS_Status* status);
bool SetSourcePixelFormat(CS_Source source, VideoMode::PixelFormat pixelFormat,
                          CS_Status* status);
bool SetSourceResolution(CS_Source source, int width, int height,
                         CS_Status* status);
bool SetSourceFPS(CS_Source source, int fps, CS_Status* status);
std::vector<VideoMode> EnumerateSourceVideoModes(CS_Source source,
                                                 CS_Status* status);
llvm::ArrayRef<CS_Sink> EnumerateSourceSinks(
    CS_Source source, llvm::SmallVectorImpl<CS_Sink>& vec, CS_Status* status);
CS_Source CopySource(CS_Source source, CS_Status* status);
void ReleaseSource(CS_Source source, CS_Status* status);

//
// Camera Source Common Property Fuctions
//
void SetCameraBrightness(CS_Source source, int brightness, CS_Status* status);
int GetCameraBrightness(CS_Source source, CS_Status* status);
void SetCameraWhiteBalanceAuto(CS_Source source, CS_Status* status);
void SetCameraWhiteBalanceHoldCurrent(CS_Source source, CS_Status* status);
void SetCameraWhiteBalanceManual(CS_Source source, int value,
                                 CS_Status* status);
void SetCameraExposureAuto(CS_Source source, CS_Status* status);
void SetCameraExposureHoldCurrent(CS_Source source, CS_Status* status);
void SetCameraExposureManual(CS_Source source, int value, CS_Status* status);

//
// UsbCamera Source Functions
//
std::string GetUsbCameraPath(CS_Source source, CS_Status* status);

//
// HttpCamera Source Functions
//
CS_HttpCameraKind GetHttpCameraKind(CS_Source source, CS_Status* status);
void SetHttpCameraUrls(CS_Source source, llvm::ArrayRef<std::string> urls,
                       CS_Status* status);
std::vector<std::string> GetHttpCameraUrls(CS_Source source, CS_Status* status);

//
// OpenCV Source Functions
//
void PutSourceFrame(CS_Source source, cv::Mat& image, CS_Status* status);
void NotifySourceError(CS_Source source, llvm::StringRef msg,
                       CS_Status* status);
void SetSourceConnected(CS_Source source, bool connected, CS_Status* status);
void SetSourceDescription(CS_Source source, llvm::StringRef description,
                          CS_Status* status);
CS_Property CreateSourceProperty(CS_Source source, llvm::StringRef name,
                                 CS_PropertyKind kind, int minimum, int maximum,
                                 int step, int defaultValue, int value,
                                 CS_Status* status);
void SetSourceEnumPropertyChoices(CS_Source source, CS_Property property,
                                  llvm::ArrayRef<std::string> choices,
                                  CS_Status* status);

//
// Sink Creation Functions
//
CS_Sink CreateMjpegServer(llvm::StringRef name, llvm::StringRef listenAddress,
                          int port, CS_Status* status);
CS_Sink CreateCvSink(llvm::StringRef name, CS_Status* status);
CS_Sink CreateCvSinkCallback(llvm::StringRef name,
                             std::function<void(uint64_t time)> processFrame,
                             CS_Status* status);

//
// Sink Functions
//
CS_SinkKind GetSinkKind(CS_Sink sink, CS_Status* status);
std::string GetSinkName(CS_Sink sink, CS_Status* status);
llvm::StringRef GetSinkName(CS_Sink sink, llvm::SmallVectorImpl<char>& buf,
                            CS_Status* status);
std::string GetSinkDescription(CS_Sink sink, CS_Status* status);
llvm::StringRef GetSinkDescription(CS_Sink sink,
                                   llvm::SmallVectorImpl<char>& buf,
                                   CS_Status* status);
void SetSinkSource(CS_Sink sink, CS_Source source, CS_Status* status);
CS_Property GetSinkSourceProperty(CS_Sink sink, llvm::StringRef name,
                                  CS_Status* status);
CS_Source GetSinkSource(CS_Sink sink, CS_Status* status);
CS_Sink CopySink(CS_Sink sink, CS_Status* status);
void ReleaseSink(CS_Sink sink, CS_Status* status);

//
// MjpegServer Sink Functions
//
std::string GetMjpegServerListenAddress(CS_Sink sink, CS_Status* status);
int GetMjpegServerPort(CS_Sink sink, CS_Status* status);

//
// OpenCV Sink Functions
//
void SetSinkDescription(CS_Sink sink, llvm::StringRef description,
                        CS_Status* status);
uint64_t GrabSinkFrame(CS_Sink sink, cv::Mat& image, CS_Status* status);
uint64_t GrabSinkFrameTimeout(CS_Sink sink, cv::Mat& image, double timeout,
                              CS_Status* status);
std::string GetSinkError(CS_Sink sink, CS_Status* status);
llvm::StringRef GetSinkError(CS_Sink sink, llvm::SmallVectorImpl<char>& buf,
                             CS_Status* status);
void SetSinkEnabled(CS_Sink sink, bool enabled, CS_Status* status);

//
// Listener Functions
//
void SetListenerOnStart(std::function<void()> onStart);
void SetListenerOnExit(std::function<void()> onExit);

CS_Listener AddListener(std::function<void(const RawEvent& event)> callback,
                        int eventMask, bool immediateNotify, CS_Status* status);

void RemoveListener(CS_Listener handle, CS_Status* status);

bool NotifierDestroyed();

//
// Logging Functions
//
typedef std::function<void(unsigned int level, const char* file,
                           unsigned int line, const char* msg)>
    LogFunc;
void SetLogger(LogFunc func, unsigned int min_level);
void SetDefaultLogger(unsigned int min_level);

//
// Utility Functions
//
std::vector<UsbCameraInfo> EnumerateUsbCameras(CS_Status* status);

llvm::ArrayRef<CS_Source> EnumerateSourceHandles(
    llvm::SmallVectorImpl<CS_Source>& vec, CS_Status* status);
llvm::ArrayRef<CS_Sink> EnumerateSinkHandles(
    llvm::SmallVectorImpl<CS_Sink>& vec, CS_Status* status);

std::string GetHostname();

std::vector<std::string> GetNetworkInterfaces();

}  // namespace cs

// C functions taking a cv::Mat* for specific interop implementations
extern "C" {
uint64_t CS_GrabSinkFrameCpp(CS_Sink sink, cv::Mat* image, CS_Status* status);
uint64_t CS_GrabSinkFrameTimeoutCpp(CS_Sink sink, cv::Mat* image,
                                    double timeout, CS_Status* status);
void CS_PutSourceFrameCpp(CS_Source source, cv::Mat* image, CS_Status* status);
}  // extern "C"

#endif  // CSCORE_CSCORE_CPP_H_
