/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "SourceImpl.h"

#include <algorithm>
#include <cstring>

#include <wpi/json.h>
#include <wpi/timestamp.h>

#include "FramePool.h"
#include "Instance.h"
#include "Log.h"

using namespace cs;

SourceImpl::SourceImpl(const wpi::Twine& name, wpi::Logger& logger)
    : m_logger(logger),
      m_framePool(Instance::GetInstance().GetFramePool()),
      m_name{name.str()} {}

void SourceImpl::SetDescription(const wpi::Twine& description) {
  std::scoped_lock lock(m_mutex);
  m_description = description.str();
}

wpi::StringRef SourceImpl::GetDescription(
    wpi::SmallVectorImpl<char>& buf) const {
  std::scoped_lock lock(m_mutex);
  buf.append(m_description.begin(), m_description.end());
  return wpi::StringRef{buf.data(), buf.size()};
}

void SourceImpl::SetConnected(bool isConnected) {
  bool wasConnected = m_isConnected.exchange(isConnected);
  if (wasConnected && !isConnected)
    disconnected();
  else if (!wasConnected && isConnected)
    connected();
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
  std::scoped_lock lock(m_mutex);
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
      } else if (s.equals_lower("h264")) {
        mode.pixelFormat = cs::VideoMode::kH264;
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
  if (config.count("properties") != 0)
    SetPropertiesJson(config.at("properties"), m_logger, GetName(), status);

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
    case VideoMode::kH264:
      pixelFormat = "h264";
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
  wpi::json props = GetPropertiesJsonObject(status);
  if (props.is_array()) j.emplace("properties", props);

  return j;
}

std::vector<VideoMode> SourceImpl::EnumerateVideoModes(
    CS_Status* status) const {
  if (!m_properties_cached && !CacheProperties(status))
    return std::vector<VideoMode>{};
  std::scoped_lock lock(m_mutex);
  return m_videoModes;
}

void SourceImpl::PutFrame(Frame frame) {
  if (auto image = frame.GetExistingImage()) {
    // Update telemetry
    recordTelemetry(CS_SOURCE_FRAMES_RECEIVED, 1);
    recordTelemetry(CS_SOURCE_BYTES_RECEIVED,
                    static_cast<int64_t>(image->size()));
  }

  // TODO: This needs to be moved to another thread so that PutFrame doesn't
  // block
  Image* originalImage = frame.GetExistingImage(0);
  if (originalImage) {
    for (auto&& it : m_outputPixelFormats) {
      VideoMode::PixelFormat fmt = it.first;
      frame.Convert(originalImage, fmt, m_compressionCtx);
    }
  }

  // Signal listeners
  newFrame(frame);
}

void SourceImpl::PutError(const wpi::Twine& msg, Frame::Time time) {
  PutFrame(m_framePool.MakeErrorFrame(msg, time));
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
  if (m_properties_cached) propertyValueUpdated(property, *prop);
}
