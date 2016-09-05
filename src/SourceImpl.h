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
  virtual void GetDescription(llvm::SmallVectorImpl<char>& desc) const = 0;
  int GetNumChannels() const { return m_numChannels; }
  bool IsConnected() const { return m_connected; }

  // Gets the current frame (without waiting for a new one).
  Frame GetCurFrame();

  // Blocking function that waits for the next frame and returns it.
  Frame GetNextFrame();

  // Force a wakeup of all GetNextFrame() callers by sending an empty frame.
  void Wakeup();

  // Property functions
  virtual int GetProperty(llvm::StringRef name) const = 0;
  virtual void EnumerateProperties(
      llvm::SmallVectorImpl<int>& properties) const = 0;
  virtual CS_PropertyType GetPropertyType(int property) const = 0;
  virtual void GetPropertyName(int property,
                               llvm::SmallVectorImpl<char>& name) const = 0;
  virtual bool GetBooleanProperty(int property) const = 0;
  virtual void SetBooleanProperty(int property, bool value) = 0;
  virtual double GetDoubleProperty(int property) const = 0;
  virtual void SetDoubleProperty(int property, double value) = 0;
  virtual double GetPropertyMin(int property) const = 0;
  virtual double GetPropertyMax(int property) const = 0;
  virtual void GetStringProperty(int property,
                                 llvm::SmallVectorImpl<char>& value) const = 0;
  virtual void SetStringProperty(int property, llvm::StringRef value) = 0;
  virtual int GetEnumProperty(int property) const = 0;
  virtual void SetEnumProperty(int property, int value) = 0;
  virtual std::vector<std::string> GetEnumPropertyChoices(
      int property) const = 0;

 protected:
  void StartFrame();
  Image& AddImage(std::size_t channel, std::size_t size);
  void FinishFrame();

  std::atomic_int m_numChannels{0};
  std::atomic_bool m_connected{false};

 private:
  void ReleaseFrame(Frame::Data* data);

  std::string m_name;

  std::mutex m_mutex;
  std::mutex m_frameMutex;
  std::condition_variable m_cv;

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
