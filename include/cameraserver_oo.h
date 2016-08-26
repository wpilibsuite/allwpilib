/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef CAMERASERVER_OO_H_
#define CAMERASERVER_OO_H_

#include "cameraserver_cpp.h"

namespace cs {

//
// Object-oriented interface
//

// Forward declarations so friend declarations work correctly
class CvSource;
class SinkListener;
class SourceListener;
class VideoSink;
class VideoSource;

class VideoProperty {
  friend class CvSource;
  friend class VideoSink;
  friend class VideoSource;

 public:
  enum Type {
    kNone = CS_PROP_NONE,
    kBoolean = CS_PROP_BOOLEAN,
    kDouble = CS_PROP_DOUBLE,
    kString = CS_PROP_STRING,
    kEnum = CS_PROP_ENUM
  };

  VideoProperty() : m_handle(0), m_type(kNone) {}

  Type type() const { return m_type; }

  explicit operator bool() const { return m_type != kNone; }

  // Type checkers
  bool IsBoolean() const { return m_type == kBoolean; }
  bool IsDouble() const { return m_type == kDouble; }
  bool IsString() const { return m_type == kString; }
  bool IsEnum() const { return m_type == kEnum; }

  // Boolean-specific functions
  bool GetBoolean() const {
    m_status = 0;
    return GetBooleanProperty(m_handle, &m_status);
  }
  void SetBoolean(bool value) {
    m_status = 0;
    SetBooleanProperty(m_handle, value, &m_status);
  }

  // Double-specific functions
  double GetDouble() const {
    m_status = 0;
    return GetDoubleProperty(m_handle, &m_status);
  }
  void SetDouble(double value) {
    m_status = 0;
    SetDoubleProperty(m_handle, value, &m_status);
  }
  double GetMin() const {
    m_status = 0;
    return GetDoublePropertyMin(m_handle, &m_status);
  }
  double GetMax() const {
    m_status = 0;
    return GetDoublePropertyMax(m_handle, &m_status);
  }

  // String-specific functions
  std::string GetString() const {
    m_status = 0;
    return GetStringProperty(m_handle, &m_status);
  }
  void GetString(llvm::SmallVectorImpl<char>& value) const {
    m_status = 0;
    GetStringProperty(m_handle, value, &m_status);
  }
  void SetString(llvm::StringRef value) {
    m_status = 0;
    SetStringProperty(m_handle, value, &m_status);
  }

  // Enum-specific functions
  int GetEnum() const {
    m_status = 0;
    return GetEnumProperty(m_handle, &m_status);
  }
  void SetEnum(int value) {
    m_status = 0;
    SetEnumProperty(m_handle, value, &m_status);
  }
  std::vector<std::string> GetChoices() const {
    m_status = 0;
    return GetEnumPropertyChoices(m_handle, &m_status);
  }

  CS_Status GetLastStatus() const { return m_status; }

 private:
  explicit VideoProperty(CS_Property handle) : m_handle(handle) {
    m_status = 0;
    if (handle == 0)
      m_type = kNone;
    else
      m_type = static_cast<Type>(
          static_cast<int>(GetPropertyType(handle, &m_status)));
  }

  mutable CS_Status m_status;
  CS_Property m_handle;
  Type m_type;
};

/// A source for video that provides a sequence of frames.  Each frame may
/// consist of multiple images (e.g. from a stereo or depth camera); these
/// are called channels.
class VideoSource {
  friend class SourceListener;
  friend class VideoSink;

 public:
  VideoSource() : m_handle(0) {}

  VideoSource(const VideoSource& source)
      : m_handle(source.m_handle == 0 ? 0 : CopySource(source.m_handle,
                                                       &m_status)) {}

  VideoSource(VideoSource&& source) noexcept : m_status(source.m_status),
                                               m_handle(source.m_handle) {
    source.m_handle = 0;
  }

  VideoSource& operator=(const VideoSource& rhs) {
    m_status = 0;
    if (m_handle != 0) ReleaseSource(m_handle, &m_status);
    m_handle = rhs.m_handle == 0 ? 0 : CopySource(rhs.m_handle, &m_status);
    return *this;
  }

  ~VideoSource() {
    m_status = 0;
    if (m_handle != 0) ReleaseSource(m_handle, &m_status);
  }

  explicit operator bool() const { return m_handle != 0; }

  /// Get the name of the source.  The name is an arbitrary identifier
  /// provided when the source is created, and should be unique.
  llvm::StringRef GetName() const {
    m_status = 0;
    return GetSourceName(m_handle, &m_status);
  }

  /// Get the source description.  This is source-type specific.
  std::string GetDescription() const {
    m_status = 0;
    return GetSourceDescription(m_handle, &m_status);
  }

  /// Get the last time a frame was captured.
  uint64_t GetLastFrameTime() const {
    m_status = 0;
    return GetSourceLastFrameTime(m_handle, &m_status);
  }

  /// Get the number of channels this source provides.
  int GetNumChannels() const {
    m_status = 0;
    return GetSourceNumChannels(m_handle, &m_status);
  }

  /// Is the source currently connected to whatever is providing the images?
  bool IsConnected() const {
    m_status = 0;
    return IsSourceConnected(m_handle, &m_status);
  }

  /// Get a property.
  /// @param name Property name
  /// @return Property contents (of type Property::kNone if no property with
  ///         the given name exists)
  VideoProperty GetProperty(llvm::StringRef name) {
    m_status = 0;
    return VideoProperty{GetSourceProperty(m_handle, name, &m_status)};
  }

  CS_Status GetLastStatus() const { return m_status; }

  /// Enumerate all existing sources.
  /// @return Vector of sources.
  static std::vector<VideoSource> EnumerateSources();

 protected:
  explicit VideoSource(CS_Source handle) : m_handle(handle) {}

  mutable CS_Status m_status = 0;
  CS_Source m_handle;
};

/// USB camera information
struct USBCameraInfo {
  /// Device number (e.g. N in '/dev/videoN' on Linux)
  int dev;
  /// Path to device if available (e.g. '/dev/video0' on Linux)
  std::string path;
  /// Vendor/model name of the camera as provided by the USB driver
  std::string name;
  /// Number of channels the camera provides (usually 1, but some cameras such
  /// as stereo or depth cameras may provide multiple channels).
  int channels;
};

/// A source that represents a USB camera.
class USBCamera : public VideoSource {
 public:
  /// Create a source for a USB camera based on device number.
  /// @param name Source name (arbitrary unique identifier)
  /// @param dev Device number (e.g. 0 for /dev/video0)
  USBCamera(llvm::StringRef name, int dev) {
    m_handle = CreateUSBSourceDev(name, dev, &m_status);
  }

  /// Create a source for a USB camera based on device path.
  /// @param name Source name (arbitrary unique identifier)
  /// @param path Path to device (e.g. "/dev/video0" on Linux)
  USBCamera(llvm::StringRef name, llvm::StringRef path) {
    m_handle = CreateUSBSourcePath(name, path, &m_status);
  }

  /// Enumerate USB cameras on the local system.
  /// @return Vector of USB camera information (one for each camera)
  static std::vector<USBCameraInfo> EnumerateUSBCameras();
};

/// A source that represents a MJPEG-over-HTTP (IP) camera.
class HTTPCamera : public VideoSource {
  /// Create a source for a MJPEG-over-HTTP (IP) camera.
  /// @param name Source name (arbitrary unique identifier)
  /// @param url Camera URL (e.g. "http://10.x.y.11/video/stream.mjpg")
  HTTPCamera(llvm::StringRef name, llvm::StringRef url) {
    m_handle = CreateHTTPSource(name, url, &m_status);
  }
};

/// A source for user code to provide OpenCV images as video frames.
class CvSource : public VideoSource {
 public:
  /// Create an OpenCV source.
  /// @param name Source name (arbitrary unique identifier)
  /// @param numChannels Number of channels
  CvSource(llvm::StringRef name, int numChannels = 1) {
    m_handle = CreateCvSource(name, numChannels, &m_status);
  }

  /// Put an OpenCV image onto the specified channel.
  /// @param channel Channel number (range 0 to numChannels-1)
  /// @param image OpenCV image
  void PutImage(int channel, cv::Mat* image) {
    m_status = 0;
    PutSourceImage(m_handle, channel, image, &m_status);
  }

  /// Signal sinks connected to this source that all new channel images have
  /// been put to the stream and are ready to be read.
  void NotifyFrame() {
    m_status = 0;
    NotifySourceFrame(m_handle, &m_status);
  }

  /// Put an OpenCV image onto channel 0 and notify sinks.
  /// This is identical in behavior to calling PutImage(0, image) followed by
  /// NotifyFrame().
  /// @param image OpenCV image
  void PutFrame(cv::Mat* image) {
    m_status = 0;
    PutSourceFrame(m_handle, image, &m_status);
  }

  /// Signal sinks that an error has occurred.  This should be called instead
  /// of NotifyFrame when an error occurs.
  void NotifyError(llvm::StringRef msg) {
    m_status = 0;
    NotifySourceError(m_handle, msg, &m_status);
  }

  /// Set source connection status.  Defaults to true.
  /// @param connected True for connected, false for disconnected
  void SetConnected(bool connected) {
    m_status = 0;
    SetSourceConnected(m_handle, connected, &m_status);
  }

  /// Create a property.
  /// @param name Property name
  /// @param type Property type
  /// @return Property
  VideoProperty CreateProperty(llvm::StringRef name, VideoProperty::Type type) {
    m_status = 0;
    return VideoProperty{CreateSourceProperty(
        m_handle, name, static_cast<CS_PropertyType>(static_cast<int>(type)),
        &m_status)};
  }

  /// Create a property with a change callback.
  /// @param name Property name
  /// @param type Property type
  /// @param onChange Callback to call when the property value changes
  /// @return Property
  VideoProperty CreateProperty(
      llvm::StringRef name, VideoProperty::Type type,
      std::function<void(llvm::StringRef name, VideoProperty property)>
          onChange) {
    m_status = 0;
    return VideoProperty{CreateSourcePropertyCallback(
        m_handle, name, static_cast<CS_PropertyType>(static_cast<int>(type)),
        [=](llvm::StringRef name, CS_Property property) {
          onChange(name, VideoProperty{property});
        },
        &m_status)};
  }

  /// Remove a property.
  /// @param name Property name
  void RemoveProperty(llvm::StringRef name) {
    m_status = 0;
    RemoveSourceProperty(m_handle, name, &m_status);
  }
};

/// A sink for video that accepts a sequence of frames.  Each frame may
/// consist of multiple images (e.g. from a stereo or depth camera); these are
/// called channels.
class VideoSink {
  friend class SinkListener;

 public:
  VideoSink() : m_handle(0) {}

  VideoSink(const VideoSink& sink)
      : m_handle(sink.m_handle == 0 ? 0 : CopySink(sink.m_handle, &m_status)) {}

  VideoSink(VideoSink&& sink) noexcept : m_status(sink.m_status),
                                         m_handle(sink.m_handle) {
    sink.m_handle = 0;
  }

  VideoSink& operator=(const VideoSink& rhs) {
    m_status = 0;
    if (m_handle != 0) ReleaseSink(m_handle, &m_status);
    m_handle = rhs.m_handle == 0 ? 0 : CopySink(rhs.m_handle, &m_status);
    return *this;
  }

  ~VideoSink() {
    m_status = 0;
    if (m_handle != 0) ReleaseSink(m_handle, &m_status);
  }

  explicit operator bool() const { return m_handle != 0; }

  /// Get the name of the sink.  The name is an arbitrary identifier
  /// provided when the sink is created, and should be unique.
  llvm::StringRef GetName() const {
    m_status = 0;
    return GetSinkName(m_handle, &m_status);
  }

  /// Get the sink description.  This is sink-type specific.
  std::string GetDescription() const {
    m_status = 0;
    return GetSinkDescription(m_handle, &m_status);
  }

  /// Configure which source should provide frames to this sink.  Each sink
  /// can accept frames from only a single source, but a single source can
  /// provide frames to multiple clients.
  /// @param source Source
  void SetSource(VideoSource source) {
    m_status = 0;
    if (!source)
      SetSinkSource(m_handle, 0, &m_status);
    else
      SetSinkSource(m_handle, source.m_handle, &m_status);
  }

  /// Get the connected source.
  /// @return Connected source (empty if none connected).
  VideoSource GetSource() const {
    m_status = 0;
    return VideoSource{GetSinkSource(m_handle, &m_status)};
  }

  /// Get a property of the associated source.
  /// @param name Property name
  /// @return Property (type Property::kNone if no property with
  ///         the given name exists or no source connected)
  VideoProperty GetSourceProperty(llvm::StringRef name) {
    m_status = 0;
    return VideoProperty{::cs::GetSourceProperty(m_handle, name, &m_status)};
  }

  CS_Status GetLastStatus() const { return m_status; }

  /// Enumerate all existing sinks.
  /// @return Vector of sinks.
  static std::vector<VideoSink> EnumerateSinks();

 protected:
  explicit VideoSink(CS_Sink handle) : m_handle(handle) {}

  mutable CS_Status m_status = 0;
  CS_Sink m_handle;
};

/// A sink that acts as a MJPEG-over-HTTP network server.
class HTTPSink : public VideoSink {
 public:
  /// Create a MJPEG-over-HTTP server sink.
  /// @param name Sink name (arbitrary unique identifier)
  /// @param listenAddress TCP listen address (empty string for all addresses)
  /// @param port TCP port number
  HTTPSink(llvm::StringRef name, llvm::StringRef listenAddress, int port) {
    m_handle = CreateHTTPSink(name, listenAddress, port, &m_status);
  }

  /// Create a MJPEG-over-HTTP server sink.
  /// @param name Sink name (arbitrary unique identifier)
  /// @param port TCP port number
  HTTPSink(llvm::StringRef name, int port) : HTTPSink(name, "", port) {}

  /// Set what video channel should be served.
  /// MJPEG-HTTP can only serve a single channel of video.
  /// By default, channel 0 is served.
  /// @param channel video channel to serve to clients
  void SetSourceChannel(int channel) {
    m_status = 0;
    SetSinkSourceChannel(m_handle, channel, &m_status);
  }
};

/// A sink for user code to accept video frames as OpenCV images.
class CvSink : public VideoSink {
 public:
  /// Create a sink for accepting OpenCV images.
  /// WaitForFrame() must be called on the created sink to get each new
  /// image.
  /// @param name Source name (arbitrary unique identifier)
  CvSink(llvm::StringRef name) { m_handle = CreateCvSink(name, &m_status); }

  /// Create a sink for accepting OpenCV images in a separate thread.
  /// A thread will be created that calls WaitForFrame() and calls the
  /// processFrame() callback each time a new frame arrives.
  /// @param name Source name (arbitrary unique identifier)
  /// @param processFrame Frame processing function; will be called with a
  ///        time=0 if an error occurred.  processFrame should call GetImage()
  ///        or GetError() as needed, but should not call (except in very
  ///        unusual circumstances) WaitForImage().
  CvSink(llvm::StringRef name,
         std::function<void(uint64_t time)> processFrame) {
    m_handle = CreateCvSinkCallback(name, processFrame, &m_status);
  }

  /// Wait for the next frame.  This is a blocking call.
  /// @return Frame time, or 0 on error (call GetError() to obtain the error
  ///         message).
  uint64_t WaitForFrame() const {
    m_status = 0;
    return SinkWaitForFrame(m_handle, &m_status);
  }

  /// Get an OpenCV image from the specified channel.
  /// @return False if image could not be obtained for some reason (e.g.
  ///         channel out of range)
  bool GetImage(int channel, cv::Mat* image) const {
    m_status = 0;
    return GetSinkImage(m_handle, channel, image, &m_status);
  }

  /// Wait for the next frame and get the image from channel 0.  Equivalent
  /// to calling WaitForFrame() followed by GetImage(0, image).
  /// @return Frame time, or 0 on error (call GetError() to obtain the error
  ///         message);
  uint64_t FrameGrab(cv::Mat* image) const {
    m_status = 0;
    return GrabSinkFrame(m_handle, image, &m_status);
  }

  /// Get error string.  Call this if WaitForFrame() returns 0 to determine
  /// what the error is.
  std::string GetError() const {
    m_status = 0;
    return GetSinkError(m_handle, &m_status);
  }

  /// Enable or disable getting new frames.
  /// Disabling will cause processFrame (for callback-based CvSinks) to not
  /// be called and WaitForFrame() to not return.  This can be used to save
  /// processor resources when frames are not needed.
  void SetEnabled(bool enabled) {
    m_status = 0;
    SetSinkEnabled(m_handle, enabled, &m_status);
  }
};

class SourceListener {
 public:
  enum Event {
    kCreated = CS_SOURCE_CREATED,
    kDestroyed = CS_SOURCE_DESTROYED,
    kConnected = CS_SOURCE_CONNECTED,
    kDisconnected = CS_SOURCE_DISCONNECTED
  };

  SourceListener(
      std::function<void(llvm::StringRef name, VideoSource source, int event)>
          callback,
      int eventMask) {
    CS_Status status = 0;
    m_handle = AddSourceListener(
        [=](llvm::StringRef name, CS_Source sourceHandle, int event) {
          callback(name, VideoSource{sourceHandle}, event);
        },
        eventMask, &status);
  }

  SourceListener(const SourceListener&) = delete;
  SourceListener& operator=(const SourceListener&) = delete;

  SourceListener(SourceListener&& rhs) noexcept : m_handle(rhs.m_handle) {
    rhs.m_handle = 0;
  }

  ~SourceListener() {
    CS_Status status = 0;
    if (m_handle != 0) RemoveSourceListener(m_handle, &status);
  }

 private:
  CS_Listener m_handle;
};

class SinkListener {
 public:
  enum Event {
    kCreated = CS_SINK_CREATED,
    kDestroyed = CS_SINK_DESTROYED,
    kEnabled = CS_SINK_ENABLED,
    kDisabled = CS_SINK_DISABLED
  };

  SinkListener(
      std::function<void(llvm::StringRef name, VideoSink sink, int event)>
          callback,
      int eventMask) {
    CS_Status status = 0;
    m_handle = AddSinkListener(
        [=](llvm::StringRef name, CS_Sink sinkHandle, int event) {
          callback(name, VideoSink{sinkHandle}, event);
        },
        eventMask, &status);
  }

  SinkListener(const SinkListener&) = delete;
  SinkListener& operator=(const SinkListener&) = delete;

  SinkListener(SinkListener&& rhs) noexcept : m_handle(rhs.m_handle) {
    rhs.m_handle = 0;
  }

  ~SinkListener() {
    CS_Status status = 0;
    if (m_handle != 0) RemoveSinkListener(m_handle, &status);
  }

 private:
  CS_Listener m_handle;
};

}  // namespace cs

#endif  /* CAMERASERVER_OO_H_ */
