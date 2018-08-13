/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "SourceImpl.h"

#include <algorithm>
#include <cstring>

#include <wpi/STLExtras.h>
#include <wpi/timestamp.h>

#include "Log.h"
#include "Notifier.h"
#include "Telemetry.h"

using namespace cs;

static constexpr size_t kMaxImagesAvail = 32;

SourceImpl::SourceImpl(wpi::StringRef name) : m_name{name} {
  m_frame = Frame{*this, wpi::StringRef{}, 0};
}

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

void SourceImpl::SetDescription(wpi::StringRef description) {
  std::lock_guard<wpi::mutex> lock(m_mutex);
  m_description = description;
}

wpi::StringRef SourceImpl::GetDescription(
    wpi::SmallVectorImpl<char>& buf) const {
  std::lock_guard<wpi::mutex> lock(m_mutex);
  buf.append(m_description.begin(), m_description.end());
  return wpi::StringRef{buf.data(), buf.size()};
}

void SourceImpl::SetConnected(bool connected) {
  bool wasConnected = m_connected.exchange(connected);
  if (wasConnected && !connected)
    Notifier::GetInstance().NotifySource(*this, CS_SOURCE_DISCONNECTED);
  else if (!wasConnected && connected)
    Notifier::GetInstance().NotifySource(*this, CS_SOURCE_CONNECTED);
}

uint64_t SourceImpl::GetCurFrameTime() {
  std::unique_lock<wpi::mutex> lock{m_frameMutex};
  return m_frame.GetTime();
}

Frame SourceImpl::GetCurFrame() {
  std::unique_lock<wpi::mutex> lock{m_frameMutex};
  return m_frame;
}

Frame SourceImpl::GetNextFrame() {
  std::unique_lock<wpi::mutex> lock{m_frameMutex};
  auto oldTime = m_frame.GetTime();
  m_frameCv.wait(lock, [=] { return m_frame.GetTime() != oldTime; });
  return m_frame;
}

Frame SourceImpl::GetNextFrame(double timeout) {
  std::unique_lock<wpi::mutex> lock{m_frameMutex};
  auto oldTime = m_frame.GetTime();
  if (!m_frameCv.wait_for(
          lock, std::chrono::milliseconds(static_cast<int>(timeout * 1000)),
          [=] { return m_frame.GetTime() != oldTime; })) {
    m_frame = Frame{*this, "timed out getting frame", wpi::Now()};
  }
  return m_frame;
}

void SourceImpl::Wakeup() {
  {
    std::lock_guard<wpi::mutex> lock{m_frameMutex};
    m_frame = Frame{*this, wpi::StringRef{}, 0};
  }
  m_frameCv.notify_all();
}

void SourceImpl::SetBrightness(int brightness, CS_Status* status) {
  *status = CS_INVALID_HANDLE;
}

int SourceImpl::GetBrightness(CS_Status* status) const {
  *status = CS_INVALID_HANDLE;
  return 0;
}

void SourceImpl::SetWhiteBalanceAuto(CS_Status* status) {
  *status = CS_INVALID_HANDLE;
}

void SourceImpl::SetWhiteBalanceHoldCurrent(CS_Status* status) {
  *status = CS_INVALID_HANDLE;
}

void SourceImpl::SetWhiteBalanceManual(int value, CS_Status* status) {
  *status = CS_INVALID_HANDLE;
}

void SourceImpl::SetExposureAuto(CS_Status* status) {
  *status = CS_INVALID_HANDLE;
}

void SourceImpl::SetExposureHoldCurrent(CS_Status* status) {
  *status = CS_INVALID_HANDLE;
}

void SourceImpl::SetExposureManual(int value, CS_Status* status) {
  *status = CS_INVALID_HANDLE;
}

VideoMode SourceImpl::GetVideoMode(CS_Status* status) const {
  if (!m_properties_cached && !CacheProperties(status)) return VideoMode{};
  std::lock_guard<wpi::mutex> lock(m_mutex);
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
  std::lock_guard<wpi::mutex> lock(m_mutex);
  return m_videoModes;
}

std::unique_ptr<Image> SourceImpl::AllocImage(
    VideoMode::PixelFormat pixelFormat, int width, int height, size_t size) {
  std::unique_ptr<Image> image;
  {
    std::lock_guard<wpi::mutex> lock{m_poolMutex};
    // find the smallest existing frame that is at least big enough.
    int found = -1;
    for (size_t i = 0; i < m_imagesAvail.size(); ++i) {
      // is it big enough?
      if (m_imagesAvail[i] && m_imagesAvail[i]->capacity() >= size) {
        // is it smaller than the last found?
        if (found < 0 ||
            m_imagesAvail[i]->capacity() < m_imagesAvail[found]->capacity()) {
          // yes, update
          found = i;
        }
      }
    }

    // if nothing found, allocate a new buffer
    if (found < 0)
      image.reset(new Image{size});
    else
      image = std::move(m_imagesAvail[found]);
  }

  // Initialize image
  image->SetSize(size);
  image->pixelFormat = pixelFormat;
  image->width = width;
  image->height = height;

  return image;
}

void SourceImpl::PutFrame(VideoMode::PixelFormat pixelFormat, int width,
                          int height, wpi::StringRef data, Frame::Time time) {
  auto image = AllocImage(pixelFormat, width, height, data.size());

  // Copy in image data
  SDEBUG4("Copying data to "
          << reinterpret_cast<const void*>(image->data()) << " from "
          << reinterpret_cast<const void*>(data.data()) << " (" << data.size()
          << " bytes)");
  std::memcpy(image->data(), data.data(), data.size());

  PutFrame(std::move(image), time);
}

void SourceImpl::PutFrame(std::unique_ptr<Image> image, Frame::Time time) {
  // Update telemetry
  Telemetry::GetInstance().RecordSourceFrames(*this, 1);
  Telemetry::GetInstance().RecordSourceBytes(*this,
                                             static_cast<int>(image->size()));

  // Update frame
  {
    std::lock_guard<wpi::mutex> lock{m_frameMutex};
    m_frame = Frame{*this, std::move(image), time};
  }

  // Signal listeners
  m_frameCv.notify_all();
}

void SourceImpl::PutError(wpi::StringRef msg, Frame::Time time) {
  // Update frame
  {
    std::lock_guard<wpi::mutex> lock{m_frameMutex};
    m_frame = Frame{*this, msg, time};
  }

  // Signal listeners
  m_frameCv.notify_all();
}

void SourceImpl::NotifyPropertyCreated(int propIndex, PropertyImpl& prop) {
  auto& notifier = Notifier::GetInstance();
  notifier.NotifySourceProperty(*this, CS_SOURCE_PROPERTY_CREATED, prop.name,
                                propIndex, prop.propKind, prop.value,
                                prop.valueStr);
  // also notify choices updated event for enum types
  if (prop.propKind == CS_PROP_ENUM)
    notifier.NotifySourceProperty(*this, CS_SOURCE_PROPERTY_CHOICES_UPDATED,
                                  prop.name, propIndex, prop.propKind,
                                  prop.value, wpi::StringRef{});
}

void SourceImpl::UpdatePropertyValue(int property, bool setString, int value,
                                     wpi::StringRef valueStr) {
  auto prop = GetProperty(property);
  if (!prop) return;

  if (setString)
    prop->SetValue(valueStr);
  else
    prop->SetValue(value);

  // Only notify updates after we've notified created
  if (m_properties_cached)
    Notifier::GetInstance().NotifySourceProperty(
        *this, CS_SOURCE_PROPERTY_VALUE_UPDATED, prop->name, property,
        prop->propKind, prop->value, prop->valueStr);
}

void SourceImpl::ReleaseImage(std::unique_ptr<Image> image) {
  std::lock_guard<wpi::mutex> lock{m_poolMutex};
  if (m_destroyFrames) return;
  // Return the frame to the pool.  First try to find an empty slot, otherwise
  // add it to the end.
  auto it = std::find(m_imagesAvail.begin(), m_imagesAvail.end(), nullptr);
  if (it != m_imagesAvail.end()) {
    *it = std::move(image);
  } else if (m_imagesAvail.size() > kMaxImagesAvail) {
    // Replace smallest buffer; don't need to check for null because the above
    // find would have found it.
    auto it2 = std::min_element(
        m_imagesAvail.begin(), m_imagesAvail.end(),
        [](const std::unique_ptr<Image>& a, const std::unique_ptr<Image>& b) {
          return a->capacity() < b->capacity();
        });
    if ((*it2)->capacity() < image->capacity()) *it2 = std::move(image);
  } else {
    m_imagesAvail.emplace_back(std::move(image));
  }
}

std::unique_ptr<Frame::Impl> SourceImpl::AllocFrameImpl() {
  std::lock_guard<wpi::mutex> lock{m_poolMutex};

  if (m_framesAvail.empty()) return wpi::make_unique<Frame::Impl>(*this);

  auto impl = std::move(m_framesAvail.back());
  m_framesAvail.pop_back();
  return impl;
}

void SourceImpl::ReleaseFrameImpl(std::unique_ptr<Frame::Impl> impl) {
  std::lock_guard<wpi::mutex> lock{m_poolMutex};
  if (m_destroyFrames) return;
  m_framesAvail.push_back(std::move(impl));
}
