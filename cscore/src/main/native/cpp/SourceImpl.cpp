// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "SourceImpl.h"

#include <algorithm>
#include <cstring>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <wpi/StringExtras.h>
#include <wpi/json.h>
#include <wpi/timestamp.h>

#include "Log.h"
#include "Notifier.h"
#include "Telemetry.h"

using namespace cs;

static constexpr size_t kMaxImagesAvail = 32;

SourceImpl::SourceImpl(std::string_view name, wpi::Logger& logger,
                       Notifier& notifier, Telemetry& telemetry)
    : m_logger(logger),
      m_notifier(notifier),
      m_telemetry(telemetry),
      m_name{name} {
  m_frame = Frame{*this, std::string_view{}, 0, WPI_TIMESRC_UNKNOWN};
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

void SourceImpl::SetDescription(std::string_view description) {
  std::scoped_lock lock(m_mutex);
  m_description = description;
}

std::string_view SourceImpl::GetDescription(
    wpi::SmallVectorImpl<char>& buf) const {
  std::scoped_lock lock(m_mutex);
  buf.append(m_description.begin(), m_description.end());
  return {buf.data(), buf.size()};
}

void SourceImpl::SetConnected(bool connected) {
  bool wasConnected = m_connected.exchange(connected);
  if (wasConnected && !connected) {
    m_notifier.NotifySource(*this, CS_SOURCE_DISCONNECTED);
  } else if (!wasConnected && connected) {
    m_notifier.NotifySource(*this, CS_SOURCE_CONNECTED);
  }
}

uint64_t SourceImpl::GetCurFrameTime() {
  std::unique_lock lock{m_frameMutex};
  return m_frame.GetTime();
}

Frame SourceImpl::GetCurFrame() {
  std::unique_lock lock{m_frameMutex};
  return m_frame;
}

Frame SourceImpl::GetNextFrame() {
  std::unique_lock lock{m_frameMutex};
  auto oldTime = m_frame.GetTime();
  m_frameCv.wait(
      lock, [=, this] { return oldTime == 0 || m_frame.GetTime() != oldTime; });
  return m_frame;
}

Frame SourceImpl::GetNextFrame(double timeout, Frame::Time lastFrameTime) {
  std::unique_lock lock{m_frameMutex};

  if (lastFrameTime == 0) {
    lastFrameTime = m_frame.GetTime();
  }

  // Wait unitl m_frame has a timestamp other than lastFrameTime
  if (!m_frameCv.wait_for(
          lock, std::chrono::milliseconds(static_cast<int>(timeout * 1000)),
          [=, this] { return m_frame.GetTime() != lastFrameTime; })) {
    m_frame = Frame{*this, "timed out getting frame", wpi::Now(),
                    WPI_TIMESRC_UNKNOWN};
  }
  return m_frame;
}

void SourceImpl::Wakeup() {
  {
    std::scoped_lock lock{m_frameMutex};
    m_frame = Frame{*this, std::string_view{}, 0, WPI_TIMESRC_UNKNOWN};
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
  if (!m_properties_cached && !CacheProperties(status)) {
    return {};
  }
  std::scoped_lock lock(m_mutex);
  return m_mode;
}

bool SourceImpl::SetPixelFormat(VideoMode::PixelFormat pixelFormat,
                                CS_Status* status) {
  auto mode = GetVideoMode(status);
  if (!mode) {
    return false;
  }
  mode.pixelFormat = pixelFormat;
  return SetVideoMode(mode, status);
}

bool SourceImpl::SetResolution(int width, int height, CS_Status* status) {
  auto mode = GetVideoMode(status);
  if (!mode) {
    return false;
  }
  mode.width = width;
  mode.height = height;
  return SetVideoMode(mode, status);
}

bool SourceImpl::SetFPS(int fps, CS_Status* status) {
  auto mode = GetVideoMode(status);
  if (!mode) {
    return false;
  }
  mode.fps = fps;
  return SetVideoMode(mode, status);
}

bool SourceImpl::SetConfigJson(std::string_view config, CS_Status* status) {
  wpi::json j;
  try {
    j = wpi::json::parse(config);
  } catch (const wpi::json::parse_error& e) {
    SWARNING("SetConfigJson: parse error at byte {}: {}", e.byte, e.what());
    *status = CS_PROPERTY_WRITE_FAILED;
    return false;
  }
  return SetConfigJson(j, status);
}

bool SourceImpl::SetConfigJson(const wpi::json& config, CS_Status* status) {
  VideoMode mode;

  // pixel format
  if (config.count("pixel format") != 0) {
    try {
      auto str = config.at("pixel format").get<std::string>();
      if (wpi::equals_lower(str, "mjpeg")) {
        mode.pixelFormat = cs::VideoMode::kMJPEG;
      } else if (wpi::equals_lower(str, "yuyv")) {
        mode.pixelFormat = cs::VideoMode::kYUYV;
      } else if (wpi::equals_lower(str, "rgb565")) {
        mode.pixelFormat = cs::VideoMode::kRGB565;
      } else if (wpi::equals_lower(str, "bgr")) {
        mode.pixelFormat = cs::VideoMode::kBGR;
      } else if (wpi::equals_lower(str, "bgra")) {
        mode.pixelFormat = cs::VideoMode::kBGRA;
      } else if (wpi::equals_lower(str, "gray")) {
        mode.pixelFormat = cs::VideoMode::kGray;
      } else if (wpi::equals_lower(str, "y16")) {
        mode.pixelFormat = cs::VideoMode::kY16;
      } else if (wpi::equals_lower(str, "uyvy")) {
        mode.pixelFormat = cs::VideoMode::kUYVY;
      } else {
        SWARNING("SetConfigJson: could not understand pixel format value '{}'",
                 str);
      }
    } catch (const wpi::json::exception& e) {
      SWARNING("SetConfigJson: could not read pixel format: {}", e.what());
    }
  }

  // width
  if (config.count("width") != 0) {
    try {
      mode.width = config.at("width").get<unsigned int>();
    } catch (const wpi::json::exception& e) {
      SWARNING("SetConfigJson: could not read width: {}", e.what());
    }
  }

  // height
  if (config.count("height") != 0) {
    try {
      mode.height = config.at("height").get<unsigned int>();
    } catch (const wpi::json::exception& e) {
      SWARNING("SetConfigJson: could not read height: {}", e.what());
    }
  }

  // fps
  if (config.count("fps") != 0) {
    try {
      mode.fps = config.at("fps").get<unsigned int>();
    } catch (const wpi::json::exception& e) {
      SWARNING("SetConfigJson: could not read fps: {}", e.what());
    }
  }

  // if all of video mode is set, use SetVideoMode, otherwise piecemeal it
  if (mode.pixelFormat != VideoMode::kUnknown && mode.width != 0 &&
      mode.height != 0 && mode.fps != 0) {
    SINFO(
        "SetConfigJson: setting video mode to pixelFormat {}, width {}, height "
        "{}, fps {}",
        mode.pixelFormat, mode.width, mode.height, mode.fps);
    SetVideoMode(mode, status);
  } else {
    if (mode.pixelFormat != cs::VideoMode::kUnknown) {
      SINFO("SetConfigJson: setting pixelFormat {}", mode.pixelFormat);
      SetPixelFormat(static_cast<cs::VideoMode::PixelFormat>(mode.pixelFormat),
                     status);
    }
    if (mode.width != 0 && mode.height != 0) {
      SINFO("SetConfigJson: setting width {}, height {}", mode.width,
            mode.height);
      SetResolution(mode.width, mode.height, status);
    }
    if (mode.fps != 0) {
      SINFO("SetConfigJson: setting fps {}", mode.fps);
      SetFPS(mode.fps, status);
    }
  }

  // brightness
  if (config.count("brightness") != 0) {
    try {
      int val = config.at("brightness").get<int>();
      SINFO("SetConfigJson: setting brightness to {}", val);
      SetBrightness(val, status);
    } catch (const wpi::json::exception& e) {
      SWARNING("SetConfigJson: could not read brightness: {}", e.what());
    }
  }

  // white balance
  if (config.count("white balance") != 0) {
    try {
      auto& setting = config.at("white balance");
      if (setting.is_string()) {
        auto str = setting.get<std::string>();
        if (wpi::equals_lower(str, "auto")) {
          SINFO("SetConfigJson: setting white balance to {}", "auto");
          SetWhiteBalanceAuto(status);
        } else if (wpi::equals_lower(str, "hold")) {
          SINFO("SetConfigJson: setting white balance to {}", "hold current");
          SetWhiteBalanceHoldCurrent(status);
        } else {
          SWARNING(
              "SetConfigJson: could not understand white balance value '{}'",
              str);
        }
      } else {
        int val = setting.get<int>();
        SINFO("SetConfigJson: setting white balance to {}", val);
        SetWhiteBalanceManual(val, status);
      }
    } catch (const wpi::json::exception& e) {
      SWARNING("SetConfigJson: could not read white balance: {}", e.what());
    }
  }

  // exposure
  if (config.count("exposure") != 0) {
    try {
      auto& setting = config.at("exposure");
      if (setting.is_string()) {
        auto str = setting.get<std::string>();
        if (wpi::equals_lower(str, "auto")) {
          SINFO("SetConfigJson: setting exposure to {}", "auto");
          SetExposureAuto(status);
        } else if (wpi::equals_lower(str, "hold")) {
          SINFO("SetConfigJson: setting exposure to {}", "hold current");
          SetExposureHoldCurrent(status);
        } else {
          SWARNING("SetConfigJson: could not understand exposure value '{}'",
                   str);
        }
      } else {
        int val = setting.get<int>();
        SINFO("SetConfigJson: setting exposure to {}", val);
        SetExposureManual(val, status);
      }
    } catch (const wpi::json::exception& e) {
      SWARNING("SetConfigJson: could not read exposure: {}", e.what());
    }
  }

  // properties
  if (config.count("properties") != 0) {
    SetPropertiesJson(config.at("properties"), m_logger, GetName(), status);
  }

  return true;
}

std::string SourceImpl::GetConfigJson(CS_Status* status) {
  std::string rv;
  wpi::raw_string_ostream os(rv);
  GetConfigJsonObject(status).dump(os, 4);
  os.flush();
  return rv;
}

wpi::json SourceImpl::GetConfigJsonObject(CS_Status* status) {
  wpi::json j;

  // pixel format
  std::string_view pixelFormat;
  switch (m_mode.pixelFormat) {
    case VideoMode::kMJPEG:
      pixelFormat = "mjpeg";
      break;
    case VideoMode::kYUYV:
      pixelFormat = "yuyv";
      break;
    case VideoMode::kRGB565:
      pixelFormat = "rgb565";
      break;
    case VideoMode::kBGR:
      pixelFormat = "bgr";
      break;
    case VideoMode::kBGRA:
      pixelFormat = "bgra";
      break;
    case VideoMode::kGray:
      pixelFormat = "gray";
      break;
    case VideoMode::kY16:
      pixelFormat = "y16";
      break;
    case VideoMode::kUYVY:
      pixelFormat = "uyvy";
      break;
    default:
      break;
  }
  if (!pixelFormat.empty()) {
    j.emplace("pixel format", pixelFormat);
  }

  // width
  if (m_mode.width != 0) {
    j.emplace("width", m_mode.width);
  }

  // height
  if (m_mode.height != 0) {
    j.emplace("height", m_mode.height);
  }

  // fps
  if (m_mode.fps != 0) {
    j.emplace("fps", m_mode.fps);
  }

  // TODO: output brightness, white balance, and exposure?

  // properties
  wpi::json props = GetPropertiesJsonObject(status);
  if (props.is_array()) {
    j.emplace("properties", props);
  }

  return j;
}

std::vector<VideoMode> SourceImpl::EnumerateVideoModes(
    CS_Status* status) const {
  if (!m_properties_cached && !CacheProperties(status)) {
    return {};
  }
  std::scoped_lock lock(m_mutex);
  return m_videoModes;
}

std::unique_ptr<Image> SourceImpl::AllocImage(
    VideoMode::PixelFormat pixelFormat, int width, int height, size_t size) {
  std::unique_ptr<Image> image;
  {
    std::scoped_lock lock{m_poolMutex};
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
    if (found < 0) {
      image = std::make_unique<Image>(size);
    } else {
      image = std::move(m_imagesAvail[found]);
    }
  }

  // Initialize image
  image->SetSize(size);
  image->pixelFormat = pixelFormat;
  image->width = width;
  image->height = height;

  return image;
}

void SourceImpl::PutFrame(VideoMode::PixelFormat pixelFormat, int width,
                          int height, std::string_view data, Frame::Time time,
                          WPI_TimestampSource timeSrc) {
  if (pixelFormat == VideoMode::PixelFormat::kBGRA) {
    // Write BGRA as BGR to save a copy
    auto image =
        CreateImageFromBGRA(this, width, height, width * 4,
                            reinterpret_cast<const uint8_t*>(data.data()));
    PutFrame(std::move(image), time);
    return;
  }

  auto image = AllocImage(pixelFormat, width, height, data.size());

  // Copy in image data
  SDEBUG4("Copying data to {} from {} ({} bytes)", fmt::ptr(image->data()),
          fmt::ptr(data.data()), data.size());
  std::memcpy(image->data(), data.data(), data.size());

  PutFrame(std::move(image), time, timeSrc);
}

void SourceImpl::PutFrame(std::unique_ptr<Image> image, Frame::Time time,
                          WPI_TimestampSource timeSrc) {
  // Update telemetry
  m_telemetry.RecordSourceFrames(*this, 1);
  m_telemetry.RecordSourceBytes(*this, static_cast<int>(image->size()));

  // Update frame
  {
    std::scoped_lock lock{m_frameMutex};
    m_frame = Frame{*this, std::move(image), time, timeSrc};
  }

  // Signal listeners
  m_frameCv.notify_all();
}

void SourceImpl::PutError(std::string_view msg, Frame::Time time) {
  // Update frame
  {
    std::scoped_lock lock{m_frameMutex};
    m_frame = Frame{*this, msg, time, WPI_TIMESRC_UNKNOWN};
  }

  // Signal listeners
  m_frameCv.notify_all();
}

void SourceImpl::NotifyPropertyCreated(int propIndex, PropertyImpl& prop) {
  m_notifier.NotifySourceProperty(*this, CS_SOURCE_PROPERTY_CREATED, prop.name,
                                  propIndex, prop.propKind, prop.value,
                                  prop.valueStr);
  // also notify choices updated event for enum types
  if (prop.propKind == CS_PROP_ENUM) {
    m_notifier.NotifySourceProperty(*this, CS_SOURCE_PROPERTY_CHOICES_UPDATED,
                                    prop.name, propIndex, prop.propKind,
                                    prop.value, {});
  }
}

void SourceImpl::UpdatePropertyValue(int property, bool setString, int value,
                                     std::string_view valueStr) {
  auto prop = GetProperty(property);
  if (!prop) {
    return;
  }

  if (setString) {
    prop->SetValue(valueStr);
  } else {
    prop->SetValue(value);
  }

  // Only notify updates after we've notified created
  if (m_properties_cached) {
    m_notifier.NotifySourceProperty(*this, CS_SOURCE_PROPERTY_VALUE_UPDATED,
                                    prop->name, property, prop->propKind,
                                    prop->value, prop->valueStr);
  }
}

void SourceImpl::ReleaseImage(std::unique_ptr<Image> image) {
  std::scoped_lock lock{m_poolMutex};
  if (m_destroyFrames) {
    return;
  }
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
    if ((*it2)->capacity() < image->capacity()) {
      *it2 = std::move(image);
    }
  } else {
    m_imagesAvail.emplace_back(std::move(image));
  }
}

std::unique_ptr<Frame::Impl> SourceImpl::AllocFrameImpl() {
  std::scoped_lock lock{m_poolMutex};

  if (m_framesAvail.empty()) {
    return std::make_unique<Frame::Impl>(*this);
  }

  auto impl = std::move(m_framesAvail.back());
  m_framesAvail.pop_back();
  return impl;
}

void SourceImpl::ReleaseFrameImpl(std::unique_ptr<Frame::Impl> impl) {
  std::scoped_lock lock{m_poolMutex};
  if (m_destroyFrames) {
    return;
  }
  m_framesAvail.push_back(std::move(impl));
}
