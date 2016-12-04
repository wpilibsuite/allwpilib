/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "SourceImpl.h"

#include <algorithm>
#include <cstring>

#include "Log.h"
#include "Notifier.h"

using namespace cs;

static constexpr std::size_t kMaxFramesAvail = 32;

SourceImpl::SourceImpl(llvm::StringRef name)
    : m_name{name}, m_frame{*this, nullptr} {}

SourceImpl::~SourceImpl() {
  // Wake up anyone who is waiting.  This also clears the current frame,
  // which is good because its destructor will call back into the class.
  Wakeup();
  // Set a flag so ReleaseFrame() doesn't re-add them to m_framesAvail.
  // Put in a block so we destroy before the destructor ends.
  {
    m_destroyFrames = true;
    auto frames = std::move(m_framesAvail);
  }
  // Everything else can clean up itself.
}

void SourceImpl::SetDescription(llvm::StringRef description) {
  std::lock_guard<std::mutex> lock(m_mutex);
  m_description = description;
}

llvm::StringRef SourceImpl::GetDescription(
    llvm::SmallVectorImpl<char>& buf) const {
  std::lock_guard<std::mutex> lock(m_mutex);
  buf.append(m_description.begin(), m_description.end());
  return llvm::StringRef{buf.data(), buf.size()};
}

void SourceImpl::SetConnected(bool connected) {
  bool wasConnected = m_connected.exchange(connected);
  if (wasConnected && !connected)
    Notifier::GetInstance().NotifySource(*this, CS_SOURCE_DISCONNECTED);
  else if (!wasConnected && connected)
    Notifier::GetInstance().NotifySource(*this, CS_SOURCE_CONNECTED);
}

uint64_t SourceImpl::GetCurFrameTime() {
  std::unique_lock<std::mutex> lock{m_frameMutex};
  return m_frame.time();
}

Frame SourceImpl::GetCurFrame() {
  std::unique_lock<std::mutex> lock{m_frameMutex};
  return m_frame;
}

Frame SourceImpl::GetNextFrame() {
  std::unique_lock<std::mutex> lock{m_frameMutex};
  auto oldTime = m_frame.time();
  m_frameCv.wait(lock, [=] { return m_frame.time() != oldTime; });
  return m_frame;
}

void SourceImpl::Wakeup() {
  {
    std::lock_guard<std::mutex> lock{m_frameMutex};
    m_frame = Frame{*this, nullptr};
  }
  m_frameCv.notify_all();
}

int SourceImpl::GetPropertyIndex(llvm::StringRef name) const {
  // We can't fail, so instead we create a new index if caching fails.
  CS_Status status = 0;
  if (!m_properties_cached) CacheProperties(&status);
  std::lock_guard<std::mutex> lock(m_mutex);
  int& ndx = m_properties[name];
  if (ndx == 0) {
    // create a new index
    ndx = m_propertyData.size() + 1;
    m_propertyData.emplace_back();
  }
  return ndx;
}

llvm::ArrayRef<int> SourceImpl::EnumerateProperties(
    llvm::SmallVectorImpl<int>& vec, CS_Status* status) const {
  if (!m_properties_cached && !CacheProperties(status))
    return llvm::ArrayRef<int>{};
  std::lock_guard<std::mutex> lock(m_mutex);
  for (int i = 0; i < static_cast<int>(m_propertyData.size()); ++i) {
    if (m_propertyData[i]) vec.push_back(i + 1);
  }
  return vec;
}

CS_PropertyKind SourceImpl::GetPropertyKind(int property) const {
  CS_Status status = 0;
  if (!m_properties_cached && !CacheProperties(&status)) return CS_PROP_NONE;
  std::lock_guard<std::mutex> lock(m_mutex);
  auto prop = GetProperty(property);
  if (!prop) return CS_PROP_NONE;
  return prop->propKind;
}

llvm::StringRef SourceImpl::GetPropertyName(int property,
                                            llvm::SmallVectorImpl<char>& buf,
                                            CS_Status* status) const {
  if (!m_properties_cached && !CacheProperties(status))
    return llvm::StringRef{};
  std::lock_guard<std::mutex> lock(m_mutex);
  auto prop = GetProperty(property);
  if (!prop) {
    *status = CS_INVALID_PROPERTY;
    return llvm::StringRef{};
  }
  // safe to not copy because we never modify it after caching
  return prop->name;
}

int SourceImpl::GetProperty(int property, CS_Status* status) const {
  if (!m_properties_cached && !CacheProperties(status)) return 0;
  std::lock_guard<std::mutex> lock(m_mutex);
  auto prop = GetProperty(property);
  if (!prop) {
    *status = CS_INVALID_PROPERTY;
    return 0;
  }
  if ((prop->propKind & (CS_PROP_BOOLEAN | CS_PROP_INTEGER | CS_PROP_ENUM)) ==
      0) {
    *status = CS_WRONG_PROPERTY_TYPE;
    return 0;
  }
  return prop->value;
}

int SourceImpl::GetPropertyMin(int property, CS_Status* status) const {
  if (!m_properties_cached && !CacheProperties(status)) return 0;
  std::lock_guard<std::mutex> lock(m_mutex);
  auto prop = GetProperty(property);
  if (!prop) {
    *status = CS_INVALID_PROPERTY;
    return 0;
  }
  return prop->minimum;
}

int SourceImpl::GetPropertyMax(int property, CS_Status* status) const {
  if (!m_properties_cached && !CacheProperties(status)) return 0;
  std::lock_guard<std::mutex> lock(m_mutex);
  auto prop = GetProperty(property);
  if (!prop) {
    *status = CS_INVALID_PROPERTY;
    return 0;
  }
  return prop->maximum;
}

int SourceImpl::GetPropertyStep(int property, CS_Status* status) const {
  if (!m_properties_cached && !CacheProperties(status)) return 0;
  std::lock_guard<std::mutex> lock(m_mutex);
  auto prop = GetProperty(property);
  if (!prop) {
    *status = CS_INVALID_PROPERTY;
    return 0;
  }
  return prop->step;
}

int SourceImpl::GetPropertyDefault(int property, CS_Status* status) const {
  if (!m_properties_cached && !CacheProperties(status)) return 0;
  std::lock_guard<std::mutex> lock(m_mutex);
  auto prop = GetProperty(property);
  if (!prop) {
    *status = CS_INVALID_PROPERTY;
    return 0;
  }
  return prop->defaultValue;
}

llvm::StringRef SourceImpl::GetStringProperty(
    int property, llvm::SmallVectorImpl<char>& buf, CS_Status* status) const {
  if (!m_properties_cached && !CacheProperties(status))
    return llvm::StringRef{};
  std::lock_guard<std::mutex> lock(m_mutex);
  auto prop = GetProperty(property);
  if (!prop) {
    *status = CS_INVALID_PROPERTY;
    return llvm::StringRef{};
  }
  if (prop->propKind != CS_PROP_STRING) {
    *status = CS_WRONG_PROPERTY_TYPE;
    return llvm::StringRef{};
  }
  buf.clear();
  buf.append(prop->valueStr.begin(), prop->valueStr.end());
  return llvm::StringRef(buf.data(), buf.size());
}

std::vector<std::string> SourceImpl::GetEnumPropertyChoices(
    int property, CS_Status* status) const {
  if (!m_properties_cached && !CacheProperties(status))
    return std::vector<std::string>{};
  std::lock_guard<std::mutex> lock(m_mutex);
  auto prop = GetProperty(property);
  if (!prop) {
    *status = CS_INVALID_PROPERTY;
    return std::vector<std::string>{};
  }
  if (prop->propKind != CS_PROP_ENUM) {
    *status = CS_WRONG_PROPERTY_TYPE;
    return std::vector<std::string>{};
  }
  return prop->enumChoices;
}

VideoMode SourceImpl::GetVideoMode(CS_Status* status) const {
  if (!m_properties_cached && !CacheProperties(status))
    return VideoMode{};
  std::lock_guard<std::mutex> lock(m_mutex);
  return m_mode;
}

bool SourceImpl::SetPixelFormat(VideoMode::PixelFormat pixelFormat,
                                CS_Status* status) {
  auto mode = GetVideoMode(status);
  if (!mode) return false;
  mode.pixelFormat = pixelFormat;
  return SetVideoMode(mode, status);
}

bool SourceImpl::SetResolution(int width, int height, CS_Status* status) {
  auto mode = GetVideoMode(status);
  if (!mode) return false;
  mode.width = width;
  mode.height = height;
  return SetVideoMode(mode, status);
}

bool SourceImpl::SetFPS(int fps, CS_Status* status) {
  auto mode = GetVideoMode(status);
  if (!mode) return false;
  mode.fps = fps;
  return SetVideoMode(mode, status);
}

std::vector<VideoMode> SourceImpl::EnumerateVideoModes(
    CS_Status* status) const {
  if (!m_properties_cached && !CacheProperties(status))
    return std::vector<VideoMode>{};
  std::lock_guard<std::mutex> lock(m_mutex);
  return m_videoModes;
}

std::unique_ptr<Frame::Data> SourceImpl::AllocFrame(
    VideoMode::PixelFormat pixelFormat, int width, int height, std::size_t size,
    Frame::Time time) {
  std::unique_ptr<Frame::Data> frameData;
  {
    std::lock_guard<std::mutex> lock{m_poolMutex};
    // find the smallest existing frame that is at least big enough.
    int found = -1;
    for (std::size_t i = 0; i < m_framesAvail.size(); ++i) {
      // is it big enough?
      if (m_framesAvail[i] && m_framesAvail[i]->capacity >= size) {
        // is it smaller than the last found?
        if (found < 0 ||
            m_framesAvail[i]->capacity < m_framesAvail[found]->capacity) {
          // yes, update
          found = i;
        }
      }
    }

    // if nothing found, allocate a new buffer
    if (found < 0)
      frameData.reset(new Frame::Data{size});
    else
      frameData = std::move(m_framesAvail[found]);
  }

  // Initialize frame data
  frameData->refcount = 0;
  frameData->time = time;
  frameData->size = size;
  frameData->pixelFormat = pixelFormat;
  frameData->width = width;
  frameData->height = height;

  return frameData;
}

void SourceImpl::PutFrame(VideoMode::PixelFormat pixelFormat, int width,
                          int height, llvm::StringRef data, Frame::Time time) {
  std::unique_ptr<Frame::Data> frameData =
      AllocFrame(pixelFormat, width, height, data.size(), time);

  // Copy in image data
  DEBUG4("Copying data to " << ((void*)frameData->data) << " from "
                            << ((void*)data.data()) << " (" << data.size()
                            << " bytes)");
  std::memcpy(frameData->data, data.data(), data.size());

  PutFrame(std::move(frameData));
}

void SourceImpl::PutFrame(std::unique_ptr<Frame::Data> frameData) {
  // Update frame
  {
    std::lock_guard<std::mutex> lock{m_frameMutex};
    m_frame = Frame{*this, std::move(frameData)};
  }

  // Signal listeners
  m_frameCv.notify_all();
}

void SourceImpl::ReleaseFrame(std::unique_ptr<Frame::Data> data) {
  std::lock_guard<std::mutex> lock{m_poolMutex};
  if (m_destroyFrames) return;
  // Return the frame to the pool.  First try to find an empty slot, otherwise
  // add it to the end.
  auto it = std::find(m_framesAvail.begin(), m_framesAvail.end(), nullptr);
  if (it != m_framesAvail.end())
    (*it) = std::move(data);
  else if (m_framesAvail.size() > kMaxFramesAvail) {
    // Replace smallest buffer; don't need to check for null because the above
    // find would have found it.
    auto it2 = std::min_element(m_framesAvail.begin(), m_framesAvail.end(),
                                [](const std::unique_ptr<Frame::Data>& a,
                                   const std::unique_ptr<Frame::Data>& b) {
                                  return a->capacity < b->capacity;
                                });
    if ((*it2)->capacity < data->capacity)
      *it2 = std::move(data);
  } else
    m_framesAvail.emplace_back(std::move(data));
}
