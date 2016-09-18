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
#include "cameraserver_c.h"
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
  bool IsConnected() const { return m_connected; }

  // Functions to keep track of the overall number of sinks connected to this
  // source.  Primarily used by sinks to determine if other sinks are using
  // the same source.
  int GetNumSinks() const { return m_numSinks; }
  void AddSink() { ++m_numSinks; }
  void RemoveSink() { --m_numSinks; }

  // Functions to keep track of the number of sinks connected to this source
  // that are "enabled", in other words, listening for new images.  Primarily
  // used by sources to determine whether they should actually bother trying
  // to get source frames.
  int GetNumSinksEnabled() const {
    std::lock_guard<std::mutex> lock{m_numSinksEnabledMutex};
    return m_numSinksEnabled;
  }

  void EnableSink() {
    std::lock_guard<std::mutex> lock{m_numSinksEnabledMutex};
    ++m_numSinksEnabled;
    m_numSinksEnabledCv.notify_all();
  }

  void DisableSink() {
    std::lock_guard<std::mutex> lock{m_numSinksEnabledMutex};
    --m_numSinksEnabled;
  }

  void WaitForEnabledSink() {
    std::unique_lock<std::mutex> lock{m_numSinksEnabledMutex};
    m_numSinksEnabledCv.wait(lock, [this] { return m_numSinksEnabled > 0; });
  }

  // Gets the current frame (without waiting for a new one).
  Frame GetCurFrame();

  // Blocking function that waits for the next frame and returns it.
  Frame GetNextFrame();

  // Force a wakeup of all GetNextFrame() callers by sending an empty frame.
  void Wakeup();

  // Property functions
  virtual int GetProperty(llvm::StringRef name) const = 0;
  virtual llvm::ArrayRef<int> EnumerateProperties(
      llvm::SmallVectorImpl<int>& vec) const = 0;
  virtual CS_PropertyType GetPropertyType(int property) const = 0;
  virtual llvm::StringRef GetPropertyName(
      int property, llvm::SmallVectorImpl<char>& buf) const = 0;
  virtual bool GetBooleanProperty(int property) const = 0;
  virtual void SetBooleanProperty(int property, bool value) = 0;
  virtual double GetDoubleProperty(int property) const = 0;
  virtual void SetDoubleProperty(int property, double value) = 0;
  virtual double GetPropertyMin(int property) const = 0;
  virtual double GetPropertyMax(int property) const = 0;
  virtual llvm::StringRef GetStringProperty(
      int property, llvm::SmallVectorImpl<char>& buf) const = 0;
  virtual void SetStringProperty(int property, llvm::StringRef value) = 0;
  virtual int GetEnumProperty(int property) const = 0;
  virtual void SetEnumProperty(int property, int value) = 0;
  virtual std::vector<std::string> GetEnumPropertyChoices(
      int property) const = 0;

 protected:
  void StartFrame();
  Image& AddImage(std::size_t size);
  void FinishFrame();

  std::atomic_bool m_connected{false};
  std::atomic_int m_numSinks{0};

 private:
  void ReleaseFrame(Frame::Data* data);

  std::string m_name;

  std::mutex m_mutex;

  mutable std::mutex m_numSinksEnabledMutex;
  std::condition_variable m_numSinksEnabledCv;
  int m_numSinksEnabled;

  std::mutex m_frameMutex;
  std::condition_variable m_frameCv;

  // Most recent complete frame (returned to callers of GetNextFrame)
  // Access protected by m_frameMutex.
  Frame m_frame;

  // In-progress frame.  Will be moved to m_frame when complete.
  std::unique_ptr<Frame::Data> m_frameData;

  // Pools of frames and images to reduce malloc traffic.
  std::vector<std::unique_ptr<Frame::Data>> m_framesAvail;
  std::vector<Image> m_imagesAvail;
};

}  // namespace cs

#endif  // CAMERASERVER_SOURCEIMPL_H_
