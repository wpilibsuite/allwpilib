/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef CAMERASERVER_CPP_H_
#define CAMERASERVER_CPP_H_

#include <functional>
#include <memory>
#include <string>

#include <stdint.h>

#include "llvm/StringRef.h"

#include "cameraserver_c.h"

namespace cv {
class Mat;
}

namespace cs {

class Property {
  struct PrivateInit {};

 public:
  enum Type {
    kNone = CS_PROP_NONE,
    kBoolean = CS_PROP_BOOLEAN,
    kDouble = CS_PROP_DOUBLE,
    kString = CS_PROP_STRING,
    kEnum = CS_PROP_ENUM
  };

  Property() : m_handle(0), m_property(0), m_type(kNone) {}
  Property(int handle, int property, Type type, const PrivateInit&) :
      m_handle(handle), m_property(property), m_type(type) {}

  Type type() const { return m_type; }

  explicit operator bool() const { return m_type != kNone; }

  // Type checkers
  bool IsBoolean() const { return m_type == kBoolean; }
  bool IsDouble() const { return m_type == kDouble; }
  bool IsString() const { return m_type == kString; }
  bool IsEnum() const { return m_type == kEnum; }

  // Boolean-specific functions
  bool GetBoolean() const;
  void SetBoolean(bool value);

  // Double-specific functions
  double GetDouble() const;
  void SetDouble(double value);
  double GetMin() const;
  double GetMax() const;

  // String-specific functions
  std::string GetString() const;
  void SetString(llvm::StringRef value);

  // Enum-specific functions
  int GetEnum() const;
  void SetEnum(int value);
  std::vector<std::string> GetChoices() const;

 private:
  int m_handle;
  int m_property;
  Type m_type;
};

/// A source for video that provides a sequence of frames.  Each frame may
/// consist of multiple images (e.g. from a stereo or depth camera); these
/// are called channels.
class VideoSource {
 public:
  VideoSource(const VideoSource&) = delete;
  VideoSource& operator=(const VideoSource&) = delete;
  ~VideoSource();

  /// Get the name of the source.  The name is an arbitrary identifier
  /// provided when the source is created, and should be unique.
  llvm::StringRef GetName() const { return m_name; }

  /// Get the source description.  This is source-type specific.
  std::string GetDescription() const;

  /// Get the last time a frame was captured.
  uint64_t GetLastFrameTime() const;

  /// Get the number of channels this source provides.
  int GetNumChannels() const;

  /// Is the source currently connected to whatever is providing the images?
  bool IsConnected() const;

  /// Get a property.
  /// @param name Property name
  /// @return Property contents (of type Property::kNone if no property with
  ///         the given name exists)
  Property GetProperty(llvm::StringRef name);

  /// Enumerate all existing sources.
  /// @return Vector of sources.
  static std::vector<std::shared_ptr<VideoSource>> EnumerateSources();

 protected:
  struct PrivateInit {};
  VideoSource(llvm::StringRef name, unsigned int handle, const PrivateInit&)
      : m_name(name), m_handle(handle) {}

 private:
  std::string m_name;
  int m_handle;
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

/// A source that represents a video camera.
class CameraSource : public VideoSource {
 public:
  /// Create a source for a USB camera based on device number.
  /// @param name Source name (arbitrary unique identifier)
  /// @param dev Device number (e.g. 0 for /dev/video0)
  static std::shared_ptr<CameraSource> CreateUSB(llvm::StringRef name, int dev);

  /// Create a source for a USB camera based on device path.
  /// @param name Source name (arbitrary unique identifier)
  /// @param path Path to device (e.g. "/dev/video0" on Linux)
  static std::shared_ptr<CameraSource> CreateUSB(llvm::StringRef name,
                                                 llvm::StringRef path);

  /// Create a source for a MJPEG-over-HTTP (IP) camera.
  /// @param name Source name (arbitrary unique identifier)
  /// @param url Camera URL (e.g. "http://10.x.y.11/video/stream.mjpg")
  static std::shared_ptr<CameraSource> CreateHTTP(llvm::StringRef name,
                                                  llvm::StringRef url);

  /// Enumerate USB cameras on the local system.
  /// @return Vector of USB camera information (one for each camera)
  static std::vector<USBCameraInfo> EnumerateUSBCameras();

 public:  // can't be private due to internal use of std::make_shared<>
  CameraSource(llvm::StringRef name, unsigned int handle,
               const PrivateInit& init)
      : VideoSource(name, handle, init) {}
};

/// A source for user code to provide OpenCV images as video frames.
class CvSource : public VideoSource {
 public:
  /// Put an OpenCV image onto the specified channel.
  /// @param channel Channel number (range 0 to nChannels-1)
  /// @param image OpenCV image
  void PutImage(int channel, cv::Mat* image);

  /// Signal sinks connected to this source that all new channel images have
  /// been put to the stream and are ready to be read.
  void NotifyFrame();

  /// Put an OpenCV image onto channel 0 and notify sinks.
  /// This is identical in behavior to calling PutImage(0, image) followed by
  /// NotifyFrame().
  /// @param image OpenCV image
  void PutFrame(cv::Mat* image);

  /// Signal sinks that an error has occurred.  This should be called instead
  /// of NotifyFrame when an error occurs.
  void NotifyError(llvm::StringRef msg);

  /// Set source connection status.  Defaults to true.
  /// @param connected True for connected, false for disconnected
  void SetConnected(bool connected);

  /// Create a property.
  /// @param name Property name
  /// @param type Property type
  /// @return Property
  Property CreateProperty(llvm::StringRef name, Property::Type type);

  /// Create a property with a change callback.
  /// @param name Property name
  /// @param type Property type
  /// @param onChange Callback to call when the property value changes
  /// @return Property
  Property CreateProperty(llvm::StringRef name, Property::Type type,
                          std::function<void(llvm::StringRef)> onChange);

  /// Remove a property.
  /// @param name Property name
  void RemoveProperty(llvm::StringRef name);

  /// Create an OpenCV source.
  /// @param name Source name (arbitrary unique identifier)
  /// @param nChannels Number of channels
  static std::shared_ptr<CvSource> Create(llvm::StringRef name,
                                          int nChannels = 1);

 public:  // can't be private due to internal use of std::make_shared<>
  CvSource(llvm::StringRef name, unsigned int handle, const PrivateInit& init)
      : VideoSource(name, handle, init) {}
};

/// A sink for video that accepts a sequence of frames.  Each frame may
/// consist of multiple images (e.g. from a stereo or depth camera); these are
/// called channels.
class VideoSink {
 public:
  VideoSink(const VideoSink&) = delete;
  VideoSink& operator=(const VideoSink&) = delete;
  ~VideoSink();

  /// Get the name of the sink.  The name is an arbitrary identifier
  /// provided when the sink is created, and should be unique.
  llvm::StringRef GetName() const { return m_name; }

  /// Get the sink description.  This is sink-type specific.
  std::string GetDescription() const;

  /// Configure which source should provide frames to this sink.  Each sink
  /// can accept frames from only a single source, but a single source can
  /// provide frames to multiple clients.
  void SetSource(std::shared_ptr<VideoSource> source);

  /// Get a property of the associated source.
  /// @param name Property name
  /// @return Property contents (of type Property::kNone if no property with
  ///         the given name exists)
  Property GetSourceProperty(llvm::StringRef name);

  /// Enumerate all existing sinks.
  /// @return Vector of sinks.
  static std::vector<std::shared_ptr<VideoSink>> EnumerateSinks();

 protected:
  struct PrivateInit {};
  VideoSink(llvm::StringRef name, unsigned int handle, const PrivateInit&)
      : m_name(name), m_handle(handle) {}

 private:
  std::string m_name;
  int m_handle;
};

/// A sink that acts as a network server.
class ServerSink : public VideoSink {
 public:
  /// Set what video channel should be served.
  /// Servers such as MJPEG-HTTP can only serve a single channel of video.
  /// By default, channel 0 is served.
  /// @param channel video channel to serve to clients
  void SetSourceChannel(int channel);

  /// Create a MJPEG-over-HTTP server sink.
  /// @param name Sink name (arbitrary unique identifier)
  /// @param listenAddress TCP listen address (empty string for all addresses)
  /// @param port TCP port number
  static std::shared_ptr<ServerSink> CreateHTTP(llvm::StringRef name,
                                                llvm::StringRef listenAddress,
                                                int port);

  /// Create a MJPEG-over-HTTP server sink.
  /// @param name Sink name (arbitrary unique identifier)
  /// @param port TCP port number
  static std::shared_ptr<ServerSink> CreateHTTP(llvm::StringRef name,
                                                int port) {
    return CreateHTTP(name, "", port);
  }

  //static std::shared_ptr<ServerSink> CreateRTSP(llvm::StringRef name,
  //                                              llvm::StringRef listenAddress,
  //                                              int port);

 public:  // can't be private due to internal use of std::make_shared<>
  ServerSink(llvm::StringRef name, unsigned int handle,
             const PrivateInit& init)
      : VideoSink(name, handle, init) {}
};

/// A sink for user code to accept video frames as OpenCV images.
class CvSink : public VideoSink {
 public:
  /// Wait for the next frame.  This is a blocking call.
  /// @return Frame time, or 0 on error (call GetError() to obtain the error
  ///         message).
  uint64_t WaitForFrame() const;

  /// Get an OpenCV image from the specified channel.
  /// @return False if image could not be obtained for some reason (e.g.
  ///         channel out of range)
  bool GetImage(int channel, cv::Mat* image) const;

  /// Wait for the next frame and get the image from channel 0.  Equivalent
  /// to calling WaitForFrame() followed by GetImage(0, image).
  /// @return Frame time, or 0 on error (call GetError() to obtain the error
  ///         message);
  uint64_t FrameGrab(cv::Mat* image) const;

  /// Get error string.  Call this if WaitForFrame() returns 0 to determine
  /// what the error is.
  std::string GetError() const;

  /// Enable or disable getting new frames.
  /// Disabling will cause processFrame (for callback-based CvSinks) to not
  /// be called and WaitForFrame() to not return.  This can be used to save
  /// processor resources when frames are not needed.
  void SetEnabled(bool enabled);

  /// Create a sink for accepting OpenCV images.
  /// WaitForFrame() must be called on the created sink to get each new
  /// image.
  /// @param name Source name (arbitrary unique identifier)
  static std::shared_ptr<CvSink> Create(llvm::StringRef name);

  /// Create a sink for accepting OpenCV images in a separate thread.
  /// A thread will be created that calls WaitForFrame() and calls the
  /// processFrame() callback each time a new frame arrives.
  /// @param name Source name (arbitrary unique identifier)
  /// @param processFrame Frame processing function; will be called with a
  ///        time=0 if an error occurred.  processFrame should call GetImage()
  ///        or GetError() as needed, but should not call (except in very
  ///        unusual circumstances) WaitForImage().
  static std::shared_ptr<CvSink> Create(
      llvm::StringRef name, std::function<void(uint64_t time)> processFrame);

 public:  // can't be private due to internal use of std::make_shared<>
  CvSink(llvm::StringRef name, unsigned int handle, const PrivateInit& init)
      : VideoSink(name, handle, init) {}
};

int AddSourceListener(
    std::function<void(llvm::StringRef name, std::shared_ptr<VideoSource>, int)>
        callback,
    int eventMask);

void RemoveSourceListener(int handle);

int AddSinkListener(
    std::function<void(llvm::StringRef name, std::shared_ptr<VideoSink>, int)>
        callback,
    int eventMask);

void RemoveSinkListener(int handle);

}  // namespace cs

#endif  /* CAMERASERVER_CPP_H_ */
