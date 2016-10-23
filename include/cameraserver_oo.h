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
    kInteger = CS_PROP_INTEGER,
    kString = CS_PROP_STRING,
    kEnum = CS_PROP_ENUM
  };

  VideoProperty() : m_handle(0), m_type(kNone) {}

  std::string GetName() const;

  Type type() const { return m_type; }

  explicit operator bool() const { return m_type != kNone; }

  // Type checkers
  bool IsBoolean() const { return m_type == kBoolean; }
  bool IsInteger() const { return m_type == kInteger; }
  bool IsString() const { return m_type == kString; }
  bool IsEnum() const { return m_type == kEnum; }

  int Get() const;
  void Set(int value);
  int GetMin() const;
  int GetMax() const;
  int GetStep() const;
  int GetDefault() const;

  // String-specific functions
  std::string GetString() const;
  llvm::StringRef GetString(llvm::SmallVectorImpl<char>& buf) const;
  void SetString(llvm::StringRef value);

  // Enum-specific functions
  std::vector<std::string> GetChoices() const;

  CS_Status GetLastStatus() const { return m_status; }

 private:
  explicit VideoProperty(CS_Property handle);

  mutable CS_Status m_status;
  CS_Property m_handle;
  Type m_type;
};

/// A source for video that provides a sequence of frames.
class VideoSource {
  friend class SourceListener;
  friend class VideoSink;

 public:
  VideoSource() noexcept : m_handle(0) {}
  VideoSource(const VideoSource& source);
  VideoSource(VideoSource&& other) noexcept;
  VideoSource& operator=(VideoSource other) noexcept;
  ~VideoSource();

  explicit operator bool() const { return m_handle != 0; }

  /// Get the name of the source.  The name is an arbitrary identifier
  /// provided when the source is created, and should be unique.
  std::string GetName() const;

  /// Get the source description.  This is source-type specific.
  std::string GetDescription() const;

  /// Get the last time a frame was captured.
  uint64_t GetLastFrameTime() const;

  /// Is the source currently connected to whatever is providing the images?
  bool IsConnected() const;

  /// Get a property.
  /// @param name Property name
  /// @return Property contents (of type Property::kNone if no property with
  ///         the given name exists)
  VideoProperty GetProperty(llvm::StringRef name);

  /// Enumerate all properties of this source.
  std::vector<VideoProperty> EnumerateProperties() const;

  /// Get the current video mode.
  VideoMode GetVideoMode() const;

  /// Set the video mode.
  /// @param mode Video mode
  bool SetVideoMode(const VideoMode& mode);

  /// Set the video mode.
  /// @param pixelFormat desired pixel format
  /// @param width desired width
  /// @param height desired height
  /// @param fps desired FPS
  /// @return True if set successfully
  bool SetVideoMode(VideoMode::PixelFormat pixelFormat, int width, int height,
                    int fps);

  /// Set the pixel format.
  /// @param pixelFormat desired pixel format
  /// @return True if set successfully
  bool SetPixelFormat(VideoMode::PixelFormat pixelFormat);

  /// Set the resolution.
  /// @param width desired width
  /// @param height desired height
  /// @return True if set successfully
  bool SetResolution(int width, int height);

  /// Set the frames per second (FPS).
  /// @param fps desired FPS
  /// @return True if set successfully
  bool SetFPS(int fps);

  /// Enumerate all known video modes for this source.
  std::vector<VideoMode> EnumerateVideoModes() const;

  CS_Status GetLastStatus() const { return m_status; }

  /// Enumerate all existing sources.
  /// @return Vector of sources.
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

/// A source that represents a USB camera.
class USBCamera : public VideoSource {
 public:
  /// Create a source for a USB camera based on device number.
  /// @param name Source name (arbitrary unique identifier)
  /// @param dev Device number (e.g. 0 for /dev/video0)
  USBCamera(llvm::StringRef name, int dev);

  /// Create a source for a USB camera based on device path.
  /// @param name Source name (arbitrary unique identifier)
  /// @param path Path to device (e.g. "/dev/video0" on Linux)
  USBCamera(llvm::StringRef name, llvm::StringRef path);

  /// Enumerate USB cameras on the local system.
  /// @return Vector of USB camera information (one for each camera)
  static std::vector<USBCameraInfo> EnumerateUSBCameras();
};

/// A source that represents a MJPEG-over-HTTP (IP) camera.
class HTTPCamera : public VideoSource {
 public:
  /// Create a source for a MJPEG-over-HTTP (IP) camera.
  /// @param name Source name (arbitrary unique identifier)
  /// @param url Camera URL (e.g. "http://10.x.y.11/video/stream.mjpg")
  HTTPCamera(llvm::StringRef name, llvm::StringRef url);
};

/// A source for user code to provide OpenCV images as video frames.
class CvSource : public VideoSource {
 public:
  /// Create an OpenCV source.
  /// @param name Source name (arbitrary unique identifier)
  /// @param mode Video mode being generated
  CvSource(llvm::StringRef name, const VideoMode& mode);

  /// Create an OpenCV source.
  /// @param name Source name (arbitrary unique identifier)
  /// @param pixelFormat Pixel format
  /// @param width width
  /// @param height height
  /// @param fps fps
  CvSource(llvm::StringRef name, VideoMode::PixelFormat pixelFormat, int width,
           int height, int fps);

  /// Put an OpenCV image and notify sinks.
  /// This is identical in behavior to calling PutImage(0, image) followed by
  /// NotifyFrame().
  /// @param image OpenCV image
  void PutFrame(cv::Mat& image);

  /// Signal sinks that an error has occurred.  This should be called instead
  /// of NotifyFrame when an error occurs.
  void NotifyError(llvm::StringRef msg);

  /// Set source connection status.  Defaults to true.
  /// @param connected True for connected, false for disconnected
  void SetConnected(bool connected);

  /// Set source description.
  /// @param description Description
  void SetDescription(llvm::StringRef description);

  /// Create a property.
  /// @param name Property name
  /// @param type Property type
  /// @param minimum Minimum value
  /// @param maximum Maximum value
  /// @param step Step value
  /// @param defaultValue Default value
  /// @param value Current value
  /// @return Property
  VideoProperty CreateProperty(llvm::StringRef name, VideoProperty::Type type,
                               int minimum, int maximum, int step,
                               int defaultValue, int value);

  /// Create a property with a change callback.
  /// @param name Property name
  /// @param type Property type
  /// @param minimum Minimum value
  /// @param maximum Maximum value
  /// @param step Step value
  /// @param defaultValue Default value
  /// @param value Current value
  /// @param onChange Callback to call when the property value changes
  /// @return Property
  VideoProperty CreateProperty(
      llvm::StringRef name, VideoProperty::Type type, int minimum, int maximum,
      int step, int defaultValue, int value,
      std::function<void(VideoProperty property)> onChange);

  /// Configure enum property choices.
  /// @param property Property
  /// @param choices Choices
  void SetEnumPropertyChoices(const VideoProperty& property,
                              llvm::ArrayRef<std::string> choices);

  /// Remove a property.
  /// @param property Property
  void RemoveProperty(const VideoProperty& property);

  /// Remove a property.
  /// @param name Property name
  void RemoveProperty(llvm::StringRef name);
};

/// A sink for video that accepts a sequence of frames.
class VideoSink {
  friend class SinkListener;

 public:
  VideoSink() noexcept : m_handle(0) {}
  VideoSink(const VideoSink& sink);
  VideoSink(VideoSink&& sink) noexcept;
  VideoSink& operator=(VideoSink other) noexcept;
  ~VideoSink();

  explicit operator bool() const { return m_handle != 0; }

  /// Get the name of the sink.  The name is an arbitrary identifier
  /// provided when the sink is created, and should be unique.
  std::string GetName() const;

  /// Get the sink description.  This is sink-type specific.
  std::string GetDescription() const;

  /// Configure which source should provide frames to this sink.  Each sink
  /// can accept frames from only a single source, but a single source can
  /// provide frames to multiple clients.
  /// @param source Source
  void SetSource(VideoSource source);

  /// Get the connected source.
  /// @return Connected source (empty if none connected).
  VideoSource GetSource() const;

  /// Get a property of the associated source.
  /// @param name Property name
  /// @return Property (type Property::kNone if no property with
  ///         the given name exists or no source connected)
  VideoProperty GetSourceProperty(llvm::StringRef name);

  CS_Status GetLastStatus() const { return m_status; }

  /// Enumerate all existing sinks.
  /// @return Vector of sinks.
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

/// A sink that acts as a MJPEG-over-HTTP network server.
class MJPEGServer : public VideoSink {
 public:
  /// Create a MJPEG-over-HTTP server sink.
  /// @param name Sink name (arbitrary unique identifier)
  /// @param listenAddress TCP listen address (empty string for all addresses)
  /// @param port TCP port number
  MJPEGServer(llvm::StringRef name, llvm::StringRef listenAddress, int port);

  /// Create a MJPEG-over-HTTP server sink.
  /// @param name Sink name (arbitrary unique identifier)
  /// @param port TCP port number
  MJPEGServer(llvm::StringRef name, int port) : MJPEGServer(name, "", port) {}
};

/// A sink for user code to accept video frames as OpenCV images.
class CvSink : public VideoSink {
 public:
  /// Create a sink for accepting OpenCV images.
  /// WaitForFrame() must be called on the created sink to get each new
  /// image.
  /// @param name Source name (arbitrary unique identifier)
  CvSink(llvm::StringRef name);

  /// Create a sink for accepting OpenCV images in a separate thread.
  /// A thread will be created that calls WaitForFrame() and calls the
  /// processFrame() callback each time a new frame arrives.
  /// @param name Source name (arbitrary unique identifier)
  /// @param processFrame Frame processing function; will be called with a
  ///        time=0 if an error occurred.  processFrame should call GetImage()
  ///        or GetError() as needed, but should not call (except in very
  ///        unusual circumstances) WaitForImage().
  CvSink(llvm::StringRef name, std::function<void(uint64_t time)> processFrame);

  /// Wait for the next frame and get the image.
  /// @return Frame time, or 0 on error (call GetError() to obtain the error
  ///         message);
  uint64_t GrabFrame(cv::Mat& image) const;

  /// Get error string.  Call this if WaitForFrame() returns 0 to determine
  /// what the error is.
  std::string GetError() const;

  /// Enable or disable getting new frames.
  /// Disabling will cause processFrame (for callback-based CvSinks) to not
  /// be called and WaitForFrame() to not return.  This can be used to save
  /// processor resources when frames are not needed.
  void SetEnabled(bool enabled);
};

class SourceListener {
 public:
  enum Event {
    kCreated = CS_SOURCE_CREATED,
    kDestroyed = CS_SOURCE_DESTROYED,
    kConnected = CS_SOURCE_CONNECTED,
    kDisconnected = CS_SOURCE_DISCONNECTED
  };

  SourceListener() : m_handle(0) {}
  SourceListener(
      std::function<void(llvm::StringRef name, VideoSource source, int event)>
          callback,
      int eventMask);

  SourceListener(const SourceListener&) = delete;
  SourceListener& operator=(const SourceListener&) = delete;
  SourceListener(SourceListener&& other) noexcept;
  ~SourceListener();

  friend void swap(SourceListener& first, SourceListener& second) noexcept {
    using std::swap;
    swap(first.m_handle, second.m_handle);
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

  SinkListener() : m_handle(0) {}
  SinkListener(
      std::function<void(llvm::StringRef name, VideoSink sink, int event)>
          callback,
      int eventMask);

  SinkListener(const SinkListener&) = delete;
  SinkListener& operator=(const SinkListener&) = delete;
  SinkListener(SinkListener&& other) noexcept;
  ~SinkListener();

  friend void swap(SinkListener& first, SinkListener& second) noexcept {
    using std::swap;
    swap(first.m_handle, second.m_handle);
  }

 private:
  CS_Listener m_handle;
};

}  // namespace cs

#include "cameraserver_oo.inl"

#endif  /* CAMERASERVER_OO_H_ */
