/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef CSCORE_SOURCEIMPL_H_
#define CSCORE_SOURCEIMPL_H_

#include <atomic>
#include <cstddef>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include <wpi/ArrayRef.h>
#include <wpi/Logger.h>
#include <wpi/Signal.h>
#include <wpi/StringRef.h>
#include <wpi/Twine.h>
#include <wpi/mutex.h>

#include "Frame.h"
#include "Handle.h"
#include "Image.h"
#include "PropertyContainer.h"
#include "CompressionContext.h"
#include "cscore_cpp.h"

namespace wpi {
class json;
}  // namespace wpi

namespace cs {

class FramePool;

class SourceImpl : public PropertyContainer {
 public:
  SourceImpl(const wpi::Twine& name, wpi::Logger& logger);
  virtual ~SourceImpl() = default;
  SourceImpl(const SourceImpl& oth) = delete;
  SourceImpl& operator=(const SourceImpl& oth) = delete;

  virtual void Start() = 0;

  wpi::StringRef GetName() const { return m_name; }

  void SetDescription(const wpi::Twine& description);
  wpi::StringRef GetDescription(wpi::SmallVectorImpl<char>& buf) const;

  void SetConnectionStrategy(CS_ConnectionStrategy strategy) {
    m_strategy = static_cast<int>(strategy);
  }
  bool IsEnabled() const {
    return m_strategy == CS_CONNECTION_KEEP_OPEN ||
           (m_strategy == CS_CONNECTION_AUTO_MANAGE && m_numSinksEnabled > 0);
  }

  // User-visible connection status
  void SetConnected(bool isConnected);
  bool IsConnected() const { return m_isConnected; }

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

  // Sinks must ask for a specific pixel format. This is so the source can perform the conversions to all requested pixel formats ahead of time.
  void EnableSink(VideoMode::PixelFormat outputPixelFormat) {
    ++m_numSinksEnabled;
    NumSinksEnabledChanged();

    int& numRegisteredOnPixFmt = m_outputPixelFormats[outputPixelFormat];
    if (!numRegisteredOnPixFmt)
        numRegisteredOnPixFmt = 0;
    ++numRegisteredOnPixFmt;
  }

  // TODO (for review): I don't love requiring the user to pass in the pixel format that they were using if they wish to disable their sink... We could return a handle instead, but that feels like it overcomplicates things.
  // Perhaps the answer is to register a output pixel formats differently?
  void DisableSink(VideoMode::PixelFormat outputPixelFormat) {
    --m_numSinksEnabled;
    NumSinksEnabledChanged();

      int& numRegisteredOnPixFmt = m_outputPixelFormats[outputPixelFormat];
      if (!numRegisteredOnPixFmt)
          numRegisteredOnPixFmt = 0;
      --numRegisteredOnPixFmt;
  }

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

  // The compression context is mostly useful because it can be used to manipulate compression settings
  const CompressionContext& GetCompressionContext() const { return m_compressionCtx; };

  // These have default implementations but can be overridden for custom
  // or optimized behavior.
  virtual bool SetPixelFormat(VideoMode::PixelFormat pixelFormat,
                              CS_Status* status);
  virtual bool SetResolution(int width, int height, CS_Status* status);
  virtual bool SetFPS(int fps, CS_Status* status);

  bool SetConfigJson(wpi::StringRef config, CS_Status* status);
  virtual bool SetConfigJson(const wpi::json& config, CS_Status* status);
  std::string GetConfigJson(CS_Status* status);
  virtual wpi::json GetConfigJsonObject(CS_Status* status);

  std::vector<VideoMode> EnumerateVideoModes(CS_Status* status) const;

  wpi::sig::Signal<> connected;
  wpi::sig::Signal<> disconnected;
  wpi::sig::Signal<> videoModesUpdated;
  wpi::sig::Signal<const VideoMode&> videoModeChanged;

  /**
   * Signal that is called for each frame
   */
  wpi::sig::Signal_mt<Frame> newFrame;

  /**
   * Signal to record telemetry.  Parameters are the telemetry kind and the
   * quantity to record.
   */
  wpi::sig::Signal<CS_TelemetryKind, int64_t> recordTelemetry;

 protected:
  void UpdatePropertyValue(int property, bool setString, int value,
                           const wpi::Twine& valueStr) override;

  void PutFrame(Frame frame);
  void PutError(const wpi::Twine& msg, Frame::Time time);

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
  FramePool& m_framePool;

 private:
  std::string m_name;
  std::string m_description;

  std::atomic_int m_strategy{CS_CONNECTION_AUTO_MANAGE};
  std::atomic_int m_numSinksEnabled{0};

  // Although a source only has one selected video mode (i.e. it may only get one resolution, pixel format, and FPS from the camera at a time), the source may convert images into a different pixel format for consumption by a sink.
  // An example scenario involves a source that outputs raw BGR data and has a sink that wants H264-encoded data; in this case the sink registers itself with the source and requests that data.
  std::map<VideoMode::PixelFormat, int> m_outputPixelFormats;

  std::atomic_bool m_isConnected{false};

  CompressionContext m_compressionCtx;
};

}  // namespace cs

#endif  // CSCORE_SOURCEIMPL_H_
