// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/camera/Camera.h"

#include <chrono>
#include <thread>

#include <cscore_cpp.h>
#include <fmt/format.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_stdlib.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc.hpp>
#include <wpi/SmallVector.h>
#include <wpi/StringExtras.h>

#include "glass/Context.h"
#include "glass/Storage.h"

using namespace glass;

static const char* PixelFormatToString(int pixelFormat) {
  switch (pixelFormat) {
    case cs::VideoMode::kBGR:
      return "BGR";
    case cs::VideoMode::kGray:
      return "Gray";
    case cs::VideoMode::kMJPEG:
      return "MJPEG";
    case cs::VideoMode::kRGB565:
      return "RGB565";
    case cs::VideoMode::kYUYV:
      return "YUYV";
    default:
      return "Unknown";
  }
}

static int StringToPixelFormat(std::string_view str) {
  if (str == "BGR") {
    return cs::VideoMode::kBGR;
  } else if (str == "Gray") {
    return cs::VideoMode::kGray;
  } else if (str == "MJPEG") {
    return cs::VideoMode::kMJPEG;
  } else if (str == "RGB565") {
    return cs::VideoMode::kRGB565;
  } else if (str == "YUYV") {
    return cs::VideoMode::kYUYV;
  } else {
    return cs::VideoMode::kUnknown;
  }
}

static void VideoModeToString(char* buf, size_t len,
                              const cs::VideoMode& mode) {
  std::snprintf(buf, len, "%s (%dx%d %d FPS)",
                PixelFormatToString(mode.pixelFormat), mode.width, mode.height,
                mode.fps);
}

CameraModel::CameraModel(Storage& storage, std::string_view id)
    : m_id{id},
      m_cameraType{storage.GetString("cameraType")},
      m_usbPath{storage.GetString("usbPath")},
      m_urls{storage.GetStringArray("httpUrls")},
      m_pixelFormatStr{storage.GetString("pixelFormat")},
      m_width{storage.GetInt("width")},
      m_height{storage.GetInt("height")},
      m_fps{storage.GetInt("fps")} {
  if (m_cameraType == "usb") {
    CS_Status status = 0;
    m_source = cs::CreateUsbCameraPath(fmt::format("glass::usb::{}", m_id),
                                       m_usbPath, &status);
  } else if (m_cameraType == "http") {
    CS_Status status = 0;
    m_source = cs::CreateHttpCamera(fmt::format("glass::http::{}", m_id),
                                    m_urls, CS_HTTP_UNKNOWN, &status);
  }

  if (!m_pixelFormatStr.empty() && m_width != 0 && m_height != 0 &&
      m_fps != 0) {
    cs::VideoMode mode;
    mode.pixelFormat = StringToPixelFormat(m_pixelFormatStr);
    mode.width = m_width;
    mode.height = m_height;
    mode.fps = m_fps;
    SetVideoMode(mode);
  }
}

CameraModel::~CameraModel() {
  Stop();
  delete m_latestFrame.load();
  for (auto frame : m_sharedFreeList) {
    delete frame;
  }
  for (auto frame : m_sourceFreeList) {
    delete frame;
  }
}

void CameraModel::Update() {
  // create or update texture when we get a new frame
  if (auto frame = m_latestFrame.exchange(nullptr)) {
    if (!m_tex || frame->cols != m_tex.GetWidth() ||
        frame->rows != m_tex.GetHeight()) {
      m_tex = wpi::gui::Texture(wpi::gui::kPixelRGBA, frame->cols, frame->rows,
                                frame->data);
    } else {
      m_tex.Update(frame->data);
    }
    // put back on shared freelist
    std::scoped_lock lock(m_sharedFreeListMutex);
    m_sharedFreeList.emplace_back(frame);
  }
}

bool CameraModel::Exists() {
  CS_Status status = 0;
  return m_source != 0 && cs::IsSourceConnected(m_source, &status);
}

void CameraModel::Start() {
  if (m_cvSink != 0) {
    return;
  }

  CS_Status status = 0;
  m_cvSink = cs::CreateCvSink(fmt::format("glass::CameraModel_{}", m_id),
                              cs::VideoMode::kBGR, &status);

  m_frameThread = std::thread([this] {
    cv::Mat frame;
    while (!m_stopCamera) {
      // get frame from camera
      CS_Status status = 0;
      uint64_t time = cs::GrabSinkFrameTimeout(m_cvSink, frame, 0.25, &status);
      if (m_stopCamera) {
        break;
      }

      cv::Mat* out = AllocMat();

      if (time == 0) {
        *out = cv::Mat::zeros(16, 16, CV_8UC4);
      } else {
        // convert to RGBA
        cv::cvtColor(frame, *out, cv::COLOR_BGR2RGBA);
      }

      // make available
      auto prev = m_latestFrame.exchange(out);

      // put prev on free list
      if (prev) {
        m_sourceFreeList.emplace_back(prev);
      }
    }
  });
}

void CameraModel::Stop() {
  m_stopCamera = true;
  if (m_cvSink != 0) {
    CS_Status status = 0;
    cs::ReleaseSink(m_cvSink, &status);
  }
  if (m_frameThread.joinable()) {
    m_frameThread.join();
  }
  m_cvSink = 0;
}

void CameraModel::SetSource(CS_Source source) {
  m_source = source;
}

void CameraModel::SetEnabled(bool enabled) {
  CS_Status status = 0;
  cs::SetSinkSource(m_cvSink, enabled ? m_source : 0, &status);
}

double CameraModel::GetActualFPS() {
  CS_Status status = 0;
  double rv = cs::GetTelemetryAverageValue(m_source, CS_SOURCE_FRAMES_RECEIVED,
                                           &status);
  return rv;
}

double CameraModel::GetActualDataRate() {
  CS_Status status = 0;
  double rv =
      cs::GetTelemetryAverageValue(m_source, CS_SOURCE_BYTES_RECEIVED, &status);
  return rv;
}

void CameraModel::SetUrls(std::span<const std::string> urls) {
  CS_Status status = 0;
  cs::SetHttpCameraUrls(m_source, urls, &status);
  m_urls.assign(urls.begin(), urls.end());
}

CS_SourceKind CameraModel::GetKind() const {
  CS_Status status = 0;
  return cs::GetSourceKind(m_source, &status);
}

cs::VideoMode CameraModel::GetVideoMode() const {
  CS_Status status = 0;
  return cs::GetSourceVideoMode(m_source, &status);
}

void CameraModel::SetVideoMode(const cs::VideoMode& mode) {
  CS_Status status = 0;
  cs::SetSourceVideoMode(m_source, mode, &status);
  m_videoMode = mode;
  m_pixelFormatStr = PixelFormatToString(m_videoMode.pixelFormat);
  m_width = mode.width;
  m_height = mode.height;
  m_fps = mode.fps;
}

void CameraModel::ResetVideoMode() {
  SetVideoMode({cs::VideoMode::PixelFormat::kMJPEG, 0, 0, 0});
}

cv::Mat* CameraModel::AllocMat() {
  // get or create a mat, prefer sourceFreeList over sharedFreeList
  cv::Mat* out;
  if (!m_sourceFreeList.empty()) {
    out = m_sourceFreeList.back();
    m_sourceFreeList.pop_back();
  } else {
    {
      std::scoped_lock lock(m_sharedFreeListMutex);
      for (auto mat : m_sharedFreeList) {
        m_sourceFreeList.emplace_back(mat);
      }
      m_sharedFreeList.clear();
    }
    if (!m_sourceFreeList.empty()) {
      out = m_sourceFreeList.back();
      m_sourceFreeList.pop_back();
    } else {
      out = new cv::Mat;
    }
  }
  return out;
}

static void EditProperty(const std::string& name, CS_Property prop) {
  CS_Status status = 0;
  switch (cs::GetPropertyKind(prop, &status)) {
    case CS_PROP_BOOLEAN: {
      bool val = cs::GetProperty(prop, &status);
      if (ImGui::Checkbox(name.c_str(), &val)) {
        cs::SetProperty(prop, val, &status);
      }
      break;
    }
    case CS_PROP_ENUM: {
      int val = cs::GetProperty(prop, &status);
      auto options = cs::GetEnumPropertyChoices(prop, &status);
      if (ImGui::BeginCombo(name.c_str(), options[val].c_str())) {
        for (int i = 0; i < static_cast<int>(options.size()); ++i) {
          if (options[i].empty()) {
            continue;
          }
          bool selected = (val == i);
          if (ImGui::Selectable(options[i].c_str(), selected)) {
            cs::SetProperty(prop, i, &status);
          }
          if (selected) {
            ImGui::SetItemDefaultFocus();
          }
        }
        ImGui::EndCombo();
      }
      break;
    }
    case CS_PROP_INTEGER: {
      int val = cs::GetProperty(prop, &status);
      // int step = cs::GetPropertyStep(prop, &status);
      int min = cs::GetPropertyMin(prop, &status);
      int max = cs::GetPropertyMax(prop, &status);
      if (min == 0 && max == 1) {
        // treat like boolean
        bool boolVal = val;
        if (ImGui::Checkbox(name.c_str(), &boolVal)) {
          cs::SetProperty(prop, boolVal, &status);
        }
        break;
      }
      // if (ImGui::InputInt(name.c_str(), &val, step, step * 10)) {
      if (ImGui::SliderInt(name.c_str(), &val, min, max)) {
        if (val < min) {
          val = min;
        }
        if (val > max) {
          val = max;
        }
        cs::SetProperty(prop, val, &status);
      }
      break;
    }
    case CS_PROP_STRING: {
      std::string val = cs::GetStringProperty(prop, &status);
      if (ImGui::InputText(name.c_str(), &val)) {
        cs::SetStringProperty(prop, val, &status);
      }
      break;
    }
    default:
      break;
  }
}

void glass::DisplayCameraSettings(CameraModel* model) {
  auto& storage = GetStorage();

  bool& showFpsDataRate = storage.GetBool("showFpsDataRate", true);
  ImGui::Checkbox("Show FPS and data rate", &showFpsDataRate);

  bool& streamEnabled = storage.GetBool("streamEnabled", true);
  ImGui::Checkbox("Stream enabled", &streamEnabled);

  CS_Source source = model->GetSource();
  CS_Status status = 0;

  auto kind = model->GetKind();

  // video mode; split out width/height/fps for HTTP cameras
  if (kind == CS_SOURCE_HTTP) {
    static int res[2];
    static int fps;
    if (ImGui::IsWindowAppearing()) {
      auto mode = model->GetVideoMode();
      res[0] = mode.width;
      res[1] = mode.height;
      fps = mode.fps;
    }
    ImGui::Separator();
    ImGui::InputInt2("Resolution", res);
    ImGui::SliderInt("FPS", &fps, 0, 30);
    if (ImGui::Button("Apply")) {
      model->SetVideoMode(
          {cs::VideoMode::PixelFormat::kMJPEG, res[0], res[1], fps});
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset")) {
      model->ResetVideoMode();
      ImGui::CloseCurrentPopup();
    }
  } else {
    auto mode = cs::GetSourceVideoMode(source, &status);
    char modeStr[64];
    VideoModeToString(modeStr, sizeof(modeStr), mode);
    if (ImGui::BeginCombo("Video Mode", modeStr)) {
      auto modes = cs::EnumerateSourceVideoModes(source, &status);
      for (auto&& amode : modes) {
        bool selected = (amode == mode);
        VideoModeToString(modeStr, sizeof(modeStr), amode);
        if (ImGui::Selectable(modeStr, selected)) {
          model->SetVideoMode(amode);
        }
        if (selected) {
          ImGui::SetItemDefaultFocus();
        }
      }
      ImGui::EndCombo();
    }
  }

  if (ImGui::CollapsingHeader("Properties")) {
    wpi::SmallVector<CS_Property, 16> propertiesArr;
    for (auto&& prop :
         cs::EnumerateSourceProperties(source, propertiesArr, &status)) {
      std::string name = cs::GetPropertyName(prop, &status);
      if (!wpi::starts_with(name, "raw_")) {
        EditProperty(name, prop);
      }
    }
  }

  if (ImGui::CollapsingHeader("Raw Properties")) {
    CS_Source source = model->GetSource();
    wpi::SmallVector<CS_Property, 16> propertiesArr;
    CS_Status status = 0;
    for (auto&& prop :
         cs::EnumerateSourceProperties(source, propertiesArr, &status)) {
      std::string name = cs::GetPropertyName(prop, &status);
      if (wpi::starts_with(name, "raw_")) {
        EditProperty(name, prop);
      }
    }
  }
}

void glass::DisplayCamera(CameraModel* model, const ImVec2& contentSize,
                          bool showFpsDataRate) {
  ImVec2 size = contentSize;
  if (showFpsDataRate) {
    size.y -= ImGui::GetStyle().ItemSpacing.y;
  }

  // render (best fit)
  auto& tex = model->GetTexture();
  if (tex && tex.GetWidth() != 0 && tex.GetHeight() != 0) {
    auto drawList = ImGui::GetWindowDrawList();
    ImVec2 pos = ImGui::GetWindowPos() + ImGui::GetCursorPos();
    ImVec2 imageMin = pos;
    ImVec2 imageMax = pos + size;
    wpi::gui::MaxFit(&imageMin, &imageMax, tex.GetWidth(), tex.GetHeight());
    drawList->AddImage(tex, imageMin, imageMax);
  }

  // fill the space
  ImGui::Dummy(size);

  if (showFpsDataRate) {
    ImGui::Text("%2.1f FPS", model->GetActualFPS());

    char buf[64];
    std::snprintf(buf, sizeof(buf), "%.1f Mbps",
                  model->GetActualDataRate() * 8 / 1000000);
    ImGui::SameLine(size.x - ImGui::CalcTextSize(buf).x -
                    ImGui::GetStyle().ItemSpacing.x);
    ImGui::TextUnformatted(buf);
  }
}

void CameraView::Display() {
  if (ImGui::BeginPopupContextItem()) {
    DisplayCameraSettings(m_model);
    ImGui::EndPopup();
  }
  auto& storage = GetStorage();
  bool showFpsDataRate = storage.GetBool("showFpsDataRate", true);
  bool streamEnabled = storage.GetBool("streamEnabled", true);

  m_model->SetEnabled(streamEnabled);

  ImVec2 size =
      ImGui::GetWindowContentRegionMax() - ImGui::GetWindowContentRegionMin();
  if (showFpsDataRate) {
    size -= ImVec2(0, ImGui::GetTextLineHeightWithSpacing());
  }
  DisplayCamera(m_model, size, showFpsDataRate);
}

void CameraView::Hidden() {
  m_model->SetEnabled(false);
}
