/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef CAMERASERVER_SOURCEIMPL_H_
#define CAMERASERVER_SOURCEIMPL_H_

#include <condition_variable>
#include <cstddef>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "llvm/ArrayRef.h"
#include "llvm/StringRef.h"
#include "cameraserver_cpp.h"
#include "Frame.h"

namespace cs {

class SourceImpl {
  friend class Frame;

 public:
  SourceImpl(llvm::StringRef name);
  virtual ~SourceImpl();
  SourceImpl(const SourceImpl& queue) = delete;
  SourceImpl& operator=(const SourceImpl& queue) = delete;

  llvm::StringRef GetName() const { return m_name; }
  virtual llvm::StringRef GetDescription(
      llvm::SmallVectorImpl<char>& buf) const = 0;
  virtual bool IsConnected() const = 0;

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

  // Force a wakeup of all GetNextFrame() callers by sending an empty frame.
  void Wakeup();

  // Property functions
  virtual int GetPropertyIndex(llvm::StringRef name) const = 0;
  virtual llvm::ArrayRef<int> EnumerateProperties(
      llvm::SmallVectorImpl<int>& vec, CS_Status* status) const = 0;
  virtual CS_PropertyType GetPropertyType(int property) const = 0;
  virtual llvm::StringRef GetPropertyName(int property,
                                          llvm::SmallVectorImpl<char>& buf,
                                          CS_Status* status) const = 0;
  virtual int GetProperty(int property, CS_Status* status) const = 0;
  virtual void SetProperty(int property, int value, CS_Status* status) = 0;
  virtual int GetPropertyMin(int property, CS_Status* status) const = 0;
  virtual int GetPropertyMax(int property, CS_Status* status) const = 0;
  virtual int GetPropertyStep(int property, CS_Status* status) const = 0;
  virtual int GetPropertyDefault(int property, CS_Status* status) const = 0;
  virtual llvm::StringRef GetStringProperty(int property,
                                            llvm::SmallVectorImpl<char>& buf,
                                            CS_Status* status) const = 0;
  virtual void SetStringProperty(int property, llvm::StringRef value,
                                 CS_Status* status) = 0;
  virtual std::vector<std::string> GetEnumPropertyChoices(
      int property, CS_Status* status) const = 0;

  // Video mode functions
  virtual VideoMode GetVideoMode(CS_Status* status) const = 0;
  virtual bool SetVideoMode(const VideoMode& mode, CS_Status* status) = 0;

  // These have default implementations but can be overridden for custom
  // or optimized behavior.
  virtual bool SetPixelFormat(VideoMode::PixelFormat pixelFormat,
                              CS_Status* status);
  virtual bool SetResolution(int width, int height, CS_Status* status);
  virtual bool SetFPS(int fps, CS_Status* status);

  virtual std::vector<VideoMode> EnumerateVideoModes(
      CS_Status* status) const = 0;

 protected:
  void PutFrame(VideoMode::PixelFormat pixelFormat, llvm::StringRef data,
                Frame::Time time);

  // Notification functions for corresponding atomics
  virtual void NumSinksChanged() = 0;
  virtual void NumSinksEnabledChanged() = 0;

  std::atomic_int m_numSinks{0};
  std::atomic_int m_numSinksEnabled{0};

 private:
  void ReleaseFrame(std::unique_ptr<Frame::Data> data);

  std::string m_name;

  std::mutex m_mutex;

  std::mutex m_frameMutex;
  std::condition_variable m_frameCv;

  // Most recent frame (returned to callers of GetNextFrame)
  // Access protected by m_frameMutex.
  Frame m_frame;

  bool m_destroyFrames{false};

  // Pool of frame data to reduce malloc traffic.
  std::vector<std::unique_ptr<Frame::Data>> m_framesAvail;
};

}  // namespace cs

#endif  // CAMERASERVER_SOURCEIMPL_H_
