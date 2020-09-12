// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/camera/Camera.h"

#include <chrono>
#include <thread>
#include "cscore_c.h"
#include "cscore_cpp.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_stdlib.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc.hpp>
#include <wpi/Twine.h>

#include "glass/Context.h"

using namespace glass;

CameraModel::CameraModel(const wpi::Twine& name) {
  CS_Status status = 0;
  m_cvSink = cs::CreateCvSink("glass::CameraModel_" + name, &status);

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

CameraModel::~CameraModel() {
  m_stopCamera = true;
  CS_Status status = 0;
  cs::ReleaseSink(m_cvSink, &status);
  m_frameThread.join();
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

void CameraModel::SetSource(CS_Source source) {
  CS_Status status = 0;
  m_source = source;
  cs::SetSinkSource(m_cvSink, m_source, &status);
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

void CameraModel::SetUrls(wpi::ArrayRef<std::string> urls) {
  CS_Status status = 0;
  cs::SetHttpCameraUrls(m_source, urls, &status);
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

static void VideoModeToString(char* buf, size_t len,
                              const cs::VideoMode& mode) {
  const char* fmt;
  switch (mode.pixelFormat) {
    case cs::VideoMode::kBGR:
      fmt = "BGR";
      break;
    case cs::VideoMode::kGray:
      fmt = "Gray";
      break;
    case cs::VideoMode::kMJPEG:
      fmt = "MJPEG";
      break;
    case cs::VideoMode::kRGB565:
      fmt = "RGB565";
      break;
    case cs::VideoMode::kYUYV:
      fmt = "YUYV";
      break;
    default:
      fmt = "Unknown";
      break;
  }
  std::snprintf(buf, len, "%s (%dx%d %d FPS)", fmt, mode.width, mode.height,
                mode.fps);
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

  bool* pShowFpsDataRate = storage.GetBoolRef("showFpsDataRate", true);
  ImGui::Checkbox("Show FPS and data rate", pShowFpsDataRate);

  bool* pStreamEnabled = storage.GetBoolRef("streamEnabled", true);
  ImGui::Checkbox("Stream enabled", pStreamEnabled);

  CS_Source source = model->GetSource();
  CS_Status status = 0;

  auto kind = cs::GetSourceKind(source, &status);

  // video mode; split out width/height/fps for HTTP cameras
  if (kind == CS_SOURCE_HTTP) {
    static int res[2];
    static int fps;
    if (ImGui::IsWindowAppearing()) {
      auto mode = cs::GetSourceVideoMode(source, &status);
      res[0] = mode.width;
      res[1] = mode.height;
      fps = mode.fps;
    }
    ImGui::Separator();
    ImGui::InputInt2("Resolution", res);
    ImGui::SliderInt("FPS", &fps, 0, 30);
    if (ImGui::Button("Apply")) {
      cs::SetSourceVideoMode(
          source, {cs::VideoMode::PixelFormat::kMJPEG, res[0], res[1], fps},
          &status);
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset")) {
      cs::SetSourceVideoMode(
          source, {cs::VideoMode::PixelFormat::kMJPEG, 0, 0, 0}, &status);
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
          cs::SetSourceVideoMode(source, amode, &status);
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
      if (!wpi::StringRef{name}.startswith("raw_")) {
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
      if (wpi::StringRef{name}.startswith("raw_")) {
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
