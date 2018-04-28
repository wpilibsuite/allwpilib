/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef CSCORE_SOURCEIMPL_H_
#define CSCORE_SOURCEIMPL_H_

#include <atomic>
#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include <llvm/ArrayRef.h>
#include <llvm/StringMap.h>
#include <llvm/StringRef.h>
#include <support/condition_variable.h>
#include <support/mutex.h>

#include "Frame.h"
#include "Image.h"
#include "PropertyImpl.h"
#include "cscore_cpp.h"

namespace cs {

class SourceImpl {
  friend class Frame;

 public:
  explicit SourceImpl(llvm::StringRef name);
  virtual ~SourceImpl();
  SourceImpl(const SourceImpl& oth) = delete;
  SourceImpl& operator=(const SourceImpl& oth) = delete;

  llvm::StringRef GetName() const { return m_name; }

  void SetDescription(llvm::StringRef description);
  llvm::StringRef GetDescription(llvm::SmallVectorImpl<char>& buf) const;

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
  Frame GetNextFrame(double timeout);

  // Force a wakeup of all GetNextFrame() callers by sending an empty frame.
  void Wakeup();

  // Property functions
  int GetPropertyIndex(llvm::StringRef name) const;
  llvm::ArrayRef<int> EnumerateProperties(llvm::SmallVectorImpl<int>& vec,
                                          CS_Status* status) const;
  CS_PropertyKind GetPropertyKind(int property) const;
  llvm::StringRef GetPropertyName(int property,
                                  llvm::SmallVectorImpl<char>& buf,
                                  CS_Status* status) const;
  int GetProperty(int property, CS_Status* status) const;
  virtual void SetProperty(int property, int value, CS_Status* status) = 0;
  int GetPropertyMin(int property, CS_Status* status) const;
  int GetPropertyMax(int property, CS_Status* status) const;
  int GetPropertyStep(int property, CS_Status* status) const;
  int GetPropertyDefault(int property, CS_Status* status) const;
  llvm::StringRef GetStringProperty(int property,
                                    llvm::SmallVectorImpl<char>& buf,
                                    CS_Status* status) const;
  virtual void SetStringProperty(int property, llvm::StringRef value,
                                 CS_Status* status) = 0;
  std::vector<std::string> GetEnumPropertyChoices(int property,
                                                  CS_Status* status) const;

  // Standard common camera properties
  virtual void SetBrightness(int brightness, CS_Status* status) = 0;
  virtual int GetBrightness(CS_Status* status) const = 0;
  virtual void SetWhiteBalanceAuto(CS_Status* status) = 0;
  virtual void SetWhiteBalanceHoldCurrent(CS_Status* status) = 0;
  virtual void SetWhiteBalanceManual(int value, CS_Status* status) = 0;
  virtual void SetExposureAuto(CS_Status* status) = 0;
  virtual void SetExposureHoldCurrent(CS_Status* status) = 0;
  virtual void SetExposureManual(int value, CS_Status* status) = 0;

  // Video mode functions
  VideoMode GetVideoMode(CS_Status* status) const;
  virtual bool SetVideoMode(const VideoMode& mode, CS_Status* status) = 0;

  // These have default implementations but can be overridden for custom
  // or optimized behavior.
  virtual bool SetPixelFormat(VideoMode::PixelFormat pixelFormat,
                              CS_Status* status);
  virtual bool SetResolution(int width, int height, CS_Status* status);
  virtual bool SetFPS(int fps, CS_Status* status);

  std::vector<VideoMode> EnumerateVideoModes(CS_Status* status) const;

  std::unique_ptr<Image> AllocImage(VideoMode::PixelFormat pixelFormat,
                                    int width, int height, size_t size);

 protected:
  void PutFrame(VideoMode::PixelFormat pixelFormat, int width, int height,
                llvm::StringRef data, Frame::Time time);
  void PutFrame(std::unique_ptr<Image> image, Frame::Time time);
  void PutError(llvm::StringRef msg, Frame::Time time);

  // Notification functions for corresponding atomics
  virtual void NumSinksChanged() = 0;
  virtual void NumSinksEnabledChanged() = 0;

  std::atomic_int m_numSinks{0};
  std::atomic_int m_numSinksEnabled{0};

 protected:
  // Get a property; must be called with m_mutex held.
  PropertyImpl* GetProperty(int property) {
    if (property <= 0 || static_cast<size_t>(property) > m_propertyData.size())
      return nullptr;
    return m_propertyData[property - 1].get();
  }
  const PropertyImpl* GetProperty(int property) const {
    if (property <= 0 || static_cast<size_t>(property) > m_propertyData.size())
      return nullptr;
    return m_propertyData[property - 1].get();
  }

  // Create an "empty" property.  This is called by GetPropertyIndex to create
  // properties that don't exist (as GetPropertyIndex can't fail).
  // Note: called with m_mutex held.
  virtual std::unique_ptr<PropertyImpl> CreateEmptyProperty(
      llvm::StringRef name) const = 0;

  // Cache properties.  Implementations must return false and set status to
  // CS_SOURCE_IS_DISCONNECTED if not possible to cache.
  virtual bool CacheProperties(CS_Status* status) const = 0;

  void NotifyPropertyCreated(int propIndex, PropertyImpl& prop);

  // Update property value; must be called with m_mutex held.
  void UpdatePropertyValue(int property, bool setString, int value,
                           llvm::StringRef valueStr);

  // Cached properties and video modes (protected with m_mutex)
  mutable std::vector<std::unique_ptr<PropertyImpl>> m_propertyData;
  mutable llvm::StringMap<int> m_properties;
  mutable std::vector<VideoMode> m_videoModes;
  // Current video mode
  mutable VideoMode m_mode;
  // Whether CacheProperties() has been successful at least once (and thus
  // should not be called again)
  mutable std::atomic_bool m_properties_cached{false};

  mutable wpi::mutex m_mutex;

 private:
  void ReleaseImage(std::unique_ptr<Image> image);
  std::unique_ptr<Frame::Impl> AllocFrameImpl();
  void ReleaseFrameImpl(std::unique_ptr<Frame::Impl> data);

  std::string m_name;
  std::string m_description;

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
