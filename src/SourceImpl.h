/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef CS_SOURCEIMPL_H_
#define CS_SOURCEIMPL_H_

#include <condition_variable>
#include <cstddef>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "llvm/ArrayRef.h"
#include "llvm/StringMap.h"
#include "llvm/StringRef.h"
#include "cscore_cpp.h"
#include "Frame.h"

namespace cs {

class SourceImpl {
  friend class Frame;

 public:
  SourceImpl(llvm::StringRef name);
  virtual ~SourceImpl();
  SourceImpl(const SourceImpl& oth) = delete;
  SourceImpl& operator=(const SourceImpl& oth) = delete;

  llvm::StringRef GetName() const { return m_name; }

  void SetDescription(llvm::StringRef description);
  llvm::StringRef GetDescription(llvm::SmallVectorImpl<char>& buf) const;

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

  std::unique_ptr<Frame::Data> AllocFrame(VideoMode::PixelFormat pixelFormat,
                                          int width, int height,
                                          std::size_t size, Frame::Time time);

 protected:
  void PutFrame(VideoMode::PixelFormat pixelFormat, int width, int height,
                llvm::StringRef data, Frame::Time time);
  void PutError(llvm::StringRef msg, Frame::Time time) {
    PutFrame(VideoMode::kUnknown, 0, 0, msg, time);
  }

  // Notification functions for corresponding atomics
  virtual void NumSinksChanged() = 0;
  virtual void NumSinksEnabledChanged() = 0;

  std::atomic_int m_numSinks{0};
  std::atomic_int m_numSinksEnabled{0};

 protected:
  // Property data
  class PropertyBase {
   public:
    PropertyBase() = default;
    PropertyBase(llvm::StringRef name_, CS_PropertyKind kind_, int minimum_,
                 int maximum_, int step_, int defaultValue_, int value_)
        : name{name_},
          propKind{kind_},
          minimum{minimum_},
          maximum{maximum_},
          step{step_},
          defaultValue{defaultValue_},
          value{value_} {}
    virtual ~PropertyBase() = default;
    PropertyBase(const PropertyBase& oth) = delete;
    PropertyBase& operator=(const PropertyBase& oth) = delete;

    std::string name;
    CS_PropertyKind propKind{CS_PROP_NONE};
    int minimum;
    int maximum;
    int step;
    int defaultValue;
    int value{0};
    std::string valueStr;
    std::vector<std::string> enumChoices;
    bool valueSet{false};
  };

  // Get a property; must be called with m_mutex held.
  PropertyBase* GetProperty(int property) {
    if (property <= 0 || static_cast<size_t>(property) > m_propertyData.size())
      return nullptr;
    return m_propertyData[property - 1].get();
  }
  const PropertyBase* GetProperty(int property) const {
    if (property <= 0 || static_cast<size_t>(property) > m_propertyData.size())
      return nullptr;
    return m_propertyData[property - 1].get();
  }

  // Cache properties.  Implementations must return false and set status to
  // CS_SOURCE_IS_DISCONNECTED if not possible to cache.
  virtual bool CacheProperties(CS_Status* status) const = 0;

  // Cached properties and video modes (protected with m_mutex)
  mutable std::vector<std::unique_ptr<PropertyBase>> m_propertyData;
  mutable llvm::StringMap<int> m_properties;
  mutable std::vector<VideoMode> m_videoModes;
  // Current video mode
  mutable VideoMode m_mode;
  // Whether CacheProperties() has been successful at least once (and thus
  // should not be called again)
  mutable std::atomic_bool m_properties_cached{false};

  mutable std::mutex m_mutex;

 private:
  void ReleaseFrame(std::unique_ptr<Frame::Data> data);

  std::string m_name;
  std::string m_description;

  std::mutex m_frameMutex;
  std::condition_variable m_frameCv;

  // Most recent frame (returned to callers of GetNextFrame)
  // Access protected by m_frameMutex.
  Frame m_frame;

  bool m_destroyFrames{false};

  // Pool of frame data to reduce malloc traffic.
  std::mutex m_poolMutex;
  std::vector<std::unique_ptr<Frame::Data>> m_framesAvail;
};

}  // namespace cs

#endif  // CS_SOURCEIMPL_H_
