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
#include <wpi/json.h>
#include <wpi/timestamp.h>

#include "Log.h"
#include "Notifier.h"
#include "Telemetry.h"

using namespace cs;

static constexpr size_t kMaxImagesAvail = 32;

SourceImpl::SourceImpl(const wpi::Twine& name, wpi::Logger& logger,
                       Notifier& notifier, Telemetry& telemetry)
    : m_logger(logger),
      m_notifier(notifier),
      m_telemetry(telemetry),
      m_name{name.str()} {
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

void SourceImpl::SetDescription(const wpi::Twine& description) {
  std::lock_guard<wpi::mutex> lock(m_mutex);
  m_description = description.str();
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
    m_notifier.NotifySource(*this, CS_SOURCE_DISCONNECTED);
  else if (!wasConnected && connected)
    m_notifier.NotifySource(*this, CS_SOURCE_CONNECTED);
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

bool SourceImpl::SetConfigJson(wpi::StringRef config, CS_Status* status) {
  wpi::json j;
  try {
    j = wpi::json::parse(config);
  } catch (const wpi::json::parse_error& e) {
    SWARNING("SetConfigJson: parse error at byte " << e.byte << ": "
                                                   << e.what());
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
      wpi::StringRef s(str);
      if (s.equals_lower("mjpeg")) {
        mode.pixelFormat = cs::VideoMode::kMJPEG;
      } else if (s.equals_lower("yuyv")) {
        mode.pixelFormat = cs::VideoMode::kYUYV;
      } else if (s.equals_lower("rgb565")) {
        mode.pixelFormat = cs::VideoMode::kRGB565;
      } else if (s.equals_lower("bgr")) {
        mode.pixelFormat = cs::VideoMode::kBGR;
      } else if (s.equals_lower("gray")) {
        mode.pixelFormat = cs::VideoMode::kGray;
      } else {
        SWARNING("SetConfigJson: could not understand pixel format value '"
                 << str << '\'');
      }
    } catch (const wpi::json::exception& e) {
      SWARNING("SetConfigJson: could not read pixel format: " << e.what());
    }
  }

  // width
  if (config.count("width") != 0) {
    try {
      mode.width = config.at("width").get<unsigned int>();
    } catch (const wpi::json::exception& e) {
      SWARNING("SetConfigJson: could not read width: " << e.what());
    }
  }

  // height
  if (config.count("height") != 0) {
    try {
      mode.height = config.at("height").get<unsigned int>();
    } catch (const wpi::json::exception& e) {
      SWARNING("SetConfigJson: could not read height: " << e.what());
    }
  }

  // fps
  if (config.count("fps") != 0) {
    try {
      mode.fps = config.at("fps").get<unsigned int>();
    } catch (const wpi::json::exception& e) {
      SWARNING("SetConfigJson: could not read fps: " << e.what());
    }
  }

  // if all of video mode is set, use SetVideoMode, otherwise piecemeal it
  if (mode.pixelFormat != VideoMode::kUnknown && mode.width != 0 &&
      mode.height != 0 && mode.fps != 0) {
    SINFO("SetConfigJson: setting video mode to pixelFormat "
          << mode.pixelFormat << ", width " << mode.width << ", height "
          << mode.height << ", fps " << mode.fps);
    SetVideoMode(mode, status);
  } else {
    if (mode.pixelFormat != cs::VideoMode::kUnknown) {
      SINFO("SetConfigJson: setting pixelFormat " << mode.pixelFormat);
      SetPixelFormat(static_cast<cs::VideoMode::PixelFormat>(mode.pixelFormat),
                     status);
    }
    if (mode.width != 0 && mode.height != 0) {
      SINFO("SetConfigJson: setting width " << mode.width << ", height "
                                            << mode.height);
      SetResolution(mode.width, mode.height, status);
    }
    if (mode.fps != 0) {
      SINFO("SetConfigJson: setting fps " << mode.fps);
      SetFPS(mode.fps, status);
    }
  }

  // brightness
  if (config.count("brightness") != 0) {
    try {
      int val = config.at("brightness").get<int>();
      SINFO("SetConfigJson: setting brightness to " << val);
      SetBrightness(val, status);
    } catch (const wpi::json::exception& e) {
      SWARNING("SetConfigJson: could not read brightness: " << e.what());
    }
  }

  // white balance
  if (config.count("white balance") != 0) {
    try {
      auto& setting = config.at("white balance");
      if (setting.is_string()) {
        auto str = setting.get<std::string>();
        wpi::StringRef s(str);
        if (s.equals_lower("auto")) {
          SINFO("SetConfigJson: setting white balance to auto");
          SetWhiteBalanceAuto(status);
        } else if (s.equals_lower("hold")) {
          SINFO("SetConfigJson: setting white balance to hold current");
          SetWhiteBalanceHoldCurrent(status);
        } else {
          SWARNING("SetConfigJson: could not understand white balance value '"
                   << str << '\'');
        }
      } else {
        int val = setting.get<int>();
        SINFO("SetConfigJson: setting white balance to " << val);
        SetWhiteBalanceManual(val, status);
      }
    } catch (const wpi::json::exception& e) {
      SWARNING("SetConfigJson: could not read white balance: " << e.what());
    }
  }

  // exposure
  if (config.count("exposure") != 0) {
    try {
      auto& setting = config.at("exposure");
      if (setting.is_string()) {
        auto str = setting.get<std::string>();
        wpi::StringRef s(str);
        if (s.equals_lower("auto")) {
          SINFO("SetConfigJson: setting exposure to auto");
          SetExposureAuto(status);
        } else if (s.equals_lower("hold")) {
          SINFO("SetConfigJson: setting exposure to hold current");
          SetExposureHoldCurrent(status);
        } else {
          SWARNING("SetConfigJson: could not understand exposure value '"
                   << str << '\'');
        }
      } else {
        int val = setting.get<int>();
        SINFO("SetConfigJson: setting exposure to " << val);
        SetExposureManual(val, status);
      }
    } catch (const wpi::json::exception& e) {
      SWARNING("SetConfigJson: could not read exposure: " << e.what());
    }
  }

  // properties
  if (config.count("properties") != 0) {
    for (auto&& prop : config.at("properties")) {
      std::string name;
      try {
        name = prop.at("name").get<std::string>();
      } catch (const wpi::json::exception& e) {
        SWARNING("SetConfigJson: could not read property name: " << e.what());
        continue;
      }
      int n = GetPropertyIndex(name);
      try {
        auto& v = prop.at("value");
        if (v.is_string()) {
          std::string val = v.get<std::string>();
          SINFO("SetConfigJson: setting property '" << name << "' to '" << val
                                                    << '\'');
          SetStringProperty(n, val, status);
        } else if (v.is_boolean()) {
          bool val = v.get<bool>();
          SINFO("SetConfigJson: setting property '" << name << "' to " << val);
          SetProperty(n, val, status);
        } else {
          int val = v.get<int>();
          SINFO("SetConfigJson: setting property '" << name << "' to " << val);
          SetProperty(n, val, status);
        }
      } catch (const wpi::json::exception& e) {
        SWARNING("SetConfigJson: could not read property value: " << e.what());
        continue;
      }
    }
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
  wpi::StringRef pixelFormat;
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
    case VideoMode::kGray:
      pixelFormat = "gray";
      break;
    default:
      break;
  }
  if (!pixelFormat.empty()) j.emplace("pixel format", pixelFormat);

  // width
  if (m_mode.width != 0) j.emplace("width", m_mode.width);

  // height
  if (m_mode.height != 0) j.emplace("height", m_mode.height);

  // fps
  if (m_mode.fps != 0) j.emplace("fps", m_mode.fps);

  // TODO: output brightness, white balance, and exposure?

  // properties
  wpi::json props;
  wpi::SmallVector<int, 32> propVec;
  for (int p : EnumerateProperties(propVec, status)) {
    wpi::json prop;
    wpi::SmallString<128> strBuf;
    prop.emplace("name", GetPropertyName(p, strBuf, status));
    switch (GetPropertyKind(p)) {
      case CS_PROP_BOOLEAN:
        prop.emplace("value", static_cast<bool>(GetProperty(p, status)));
        break;
      case CS_PROP_INTEGER:
      case CS_PROP_ENUM:
        prop.emplace("value", GetProperty(p, status));
        break;
      case CS_PROP_STRING:
        prop.emplace("value", GetStringProperty(p, strBuf, status));
        break;
      default:
        continue;
    }
    props.emplace_back(prop);
  }
  if (props.is_array()) j.emplace("properties", props);

  return j;
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
  m_telemetry.RecordSourceFrames(*this, 1);
  m_telemetry.RecordSourceBytes(*this, static_cast<int>(image->size()));

  // Update frame
  {
    std::lock_guard<wpi::mutex> lock{m_frameMutex};
    m_frame = Frame{*this, std::move(image), time};
  }

  // Signal listeners
  m_frameCv.notify_all();
}

void SourceImpl::PutError(const wpi::Twine& msg, Frame::Time time) {
  // Update frame
  {
    std::lock_guard<wpi::mutex> lock{m_frameMutex};
    m_frame = Frame{*this, msg, time};
  }

  // Signal listeners
  m_frameCv.notify_all();
}

void SourceImpl::NotifyPropertyCreated(int propIndex, PropertyImpl& prop) {
  m_notifier.NotifySourceProperty(*this, CS_SOURCE_PROPERTY_CREATED, prop.name,
                                  propIndex, prop.propKind, prop.value,
                                  prop.valueStr);
  // also notify choices updated event for enum types
  if (prop.propKind == CS_PROP_ENUM)
    m_notifier.NotifySourceProperty(*this, CS_SOURCE_PROPERTY_CHOICES_UPDATED,
                                    prop.name, propIndex, prop.propKind,
                                    prop.value, wpi::Twine{});
}

void SourceImpl::UpdatePropertyValue(int property, bool setString, int value,
                                     const wpi::Twine& valueStr) {
  auto prop = GetProperty(property);
  if (!prop) return;

  if (setString)
    prop->SetValue(valueStr);
  else
    prop->SetValue(value);

  // Only notify updates after we've notified created
  if (m_properties_cached) {
    m_notifier.NotifySourceProperty(*this, CS_SOURCE_PROPERTY_VALUE_UPDATED,
                                    prop->name, property, prop->propKind,
                                    prop->value, prop->valueStr);
  }
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
