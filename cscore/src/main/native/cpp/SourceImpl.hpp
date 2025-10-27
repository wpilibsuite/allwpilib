// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef CSCORE_SOURCEIMPL_H_
#define CSCORE_SOURCEIMPL_H_

#include <atomic>
#include <cstddef>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include <wpi/Logger.h>
#include <wpi/RawFrame.h>
#include <wpi/condition_variable.h>
#include <wpi/json_fwd.h>
#include <wpi/mutex.h>

#include "Frame.h"
#include "Handle.h"
#include "Image.h"
#include "PropertyContainer.h"
#include "cscore_cpp.h"

namespace cs {

class Notifier;
class Telemetry;

class SourceImpl : public PropertyContainer {
  friend class Frame;

 public:
  SourceImpl(std::string_view name, wpi::Logger& logger, Notifier& notifier,
             Telemetry& telemetry);
  ~SourceImpl() override;
  SourceImpl(const SourceImpl& oth) = delete;
  SourceImpl& operator=(const SourceImpl& oth) = delete;

  virtual void Start() = 0;

  std::string_view GetName() const { return m_name; }

  void SetDescription(std::string_view description);
  std::string_view GetDescription(wpi::SmallVectorImpl<char>& buf) const;

  void SetConnectionStrategy(CS_ConnectionStrategy strategy) {
    m_strategy = static_cast<int>(strategy);
    NumSinksChanged();
  }
  bool IsEnabled() const {
    return m_strategy == CS_CONNECTION_KEEP_OPEN ||
           (m_strategy == CS_CONNECTION_AUTO_MANAGE && m_numSinksEnabled > 0);
  }

  // User-visible connection status
  void SetConnected(bool connected);
  bool IsConnected() const { return m_connected; }

  // Functions to keep track of the overall number of sinks connected to this
  // source.  Primarily used by sinks to determine if other sinks are using
  // the same source.
  int GetNumSinks() const { return m_numSinks; }
  void AddSink() {
    ++m_numSinks;
    NumSinksChanged();
  }
  void RemoveSink() {
    --m_numSinks;
    NumSinksChanged();
  }

  // Functions to keep track of the number of sinks connected to this source
  // that are "enabled", in other words, listening for new images.  Primarily
  // used by sources to determine whether they should actually bother trying
  // to get source frames.
  int GetNumSinksEnabled() const { return m_numSinksEnabled; }

  void EnableSink() {
    ++m_numSinksEnabled;
    NumSinksEnabledChanged();
  }

  void DisableSink() {
    --m_numSinksEnabled;
    NumSinksEnabledChanged();
  }

  // Gets the current frame time (without waiting for a new one).
  uint64_t GetCurFrameTime();

  // Gets the current frame (without waiting for a new one).
  Frame GetCurFrame();

  // Blocking function that waits for the next frame and returns it.
  Frame GetNextFrame();

  // Blocking function that waits for the next frame and returns it (with
  // timeout in seconds).  If timeout expires, returns empty frame.
  // If lastFrameTime==0, uses m_frame.GetTime() for lastFrameTime
  Frame GetNextFrame(double timeout, Frame::Time lastFrameTime = 0);

  // Force a wakeup of all GetNextFrame() callers by sending an empty frame.
  void Wakeup();

  // Standard common camera properties
  virtual void SetBrightness(int brightness, CS_Status* status);
  virtual int GetBrightness(CS_Status* status) const;
  virtual void SetWhiteBalanceAuto(CS_Status* status);
  virtual void SetWhiteBalanceHoldCurrent(CS_Status* status);
  virtual void SetWhiteBalanceManual(int value, CS_Status* status);
  virtual void SetExposureAuto(CS_Status* status);
  virtual void SetExposureHoldCurrent(CS_Status* status);
  virtual void SetExposureManual(int value, CS_Status* status);

  // Video mode functions
  VideoMode GetVideoMode(CS_Status* status) const;
  virtual bool SetVideoMode(const VideoMode& mode, CS_Status* status) = 0;

  // These have default implementations but can be overridden for custom
  // or optimized behavior.
  virtual bool SetPixelFormat(VideoMode::PixelFormat pixelFormat,
                              CS_Status* status);
  virtual bool SetResolution(int width, int height, CS_Status* status);
  virtual bool SetFPS(int fps, CS_Status* status);

  bool SetConfigJson(std::string_view config, CS_Status* status);
  virtual bool SetConfigJson(const wpi::json& config, CS_Status* status);
  std::string GetConfigJson(CS_Status* status);
  virtual wpi::json GetConfigJsonObject(CS_Status* status);

  std::vector<VideoMode> EnumerateVideoModes(CS_Status* status) const;

  std::unique_ptr<Image> AllocImage(VideoMode::PixelFormat pixelFormat,
                                    int width, int height, size_t size);

 protected:
  void NotifyPropertyCreated(int propIndex, PropertyImpl& prop) override;
  void UpdatePropertyValue(int property, bool setString, int value,
                           std::string_view valueStr) override;

  void PutFrame(VideoMode::PixelFormat pixelFormat, int width, int height,
                std::string_view data, Frame::Time time,
                WPI_TimestampSource timeSrc = WPI_TIMESRC_FRAME_DEQUEUE);
  void PutFrame(std::unique_ptr<Image> image, Frame::Time time,
                WPI_TimestampSource timeSrc = WPI_TIMESRC_FRAME_DEQUEUE);
  void PutError(std::string_view msg, Frame::Time time);

  // Notification functions for corresponding atomics
  virtual void NumSinksChanged() = 0;
  virtual void NumSinksEnabledChanged() = 0;

  std::atomic_int m_numSinks{0};

 protected:
  // Cached video modes (protected with m_mutex)
  mutable std::vector<VideoMode> m_videoModes;
  // Current video mode
  mutable VideoMode m_mode;

  wpi::Logger& m_logger;
  Notifier& m_notifier;
  Telemetry& m_telemetry;

 private:
  void ReleaseImage(std::unique_ptr<Image> image);
  std::unique_ptr<Frame::Impl> AllocFrameImpl();
  void ReleaseFrameImpl(std::unique_ptr<Frame::Impl> data);

  std::string m_name;
  std::string m_description;

  std::atomic_int m_strategy{CS_CONNECTION_AUTO_MANAGE};
  std::atomic_int m_numSinksEnabled{0};

  wpi::mutex m_frameMutex;
  wpi::condition_variable m_frameCv;

  bool m_destroyFrames{false};

  // Pool of frames/images to reduce malloc traffic.
  wpi::mutex m_poolMutex;
  std::vector<std::unique_ptr<Frame::Impl>> m_framesAvail;
  std::vector<std::unique_ptr<Image>> m_imagesAvail;

  std::atomic_bool m_connected{false};

  // Most recent frame (returned to callers of GetNextFrame)
  // Access protected by m_frameMutex.
  // MUST be located below m_poolMutex as the Frame destructor calls back
  // into SourceImpl::ReleaseImage, which locks m_poolMutex.
  Frame m_frame;
};

}  // namespace cs

#endif  // CSCORE_SOURCEIMPL_H_
