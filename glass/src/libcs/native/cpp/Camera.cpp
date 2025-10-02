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

#include "cscore_c.h"
#include "cscore_oo.h"
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
  wpi::format_to_n_c_str(buf, len, "{} ({}x{} {} FPS)",
                         PixelFormatToString(mode.pixelFormat), mode.width,
                         mode.height, mode.fps);
}

CameraModel::CameraModel(Storage& storage, std::string_view id)
    : m_id{id},
      m_cameraType{storage.GetString("cameraType")},
      m_usbPath{storage.GetString("usbPath")},
      m_urls{storage.GetStringArray("httpUrls")},
      m_pixelFormatStr{storage.GetString("pixelFormat")},
      m_width{storage.GetInt("width")},
      m_height{storage.GetInt("height")},
      m_fps{storage.GetInt("fps")} {}

CameraModel::~CameraModel() {
  Stop();
  if (m_frameThread.joinable()) {
    m_frameThread.join();
  }
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
  return m_source && m_source.IsConnected();
}

void CameraModel::Start() {
  if (m_frameThread.joinable()) {
    return;
  }

  if (!m_source) {
    if (m_cameraType == "usb") {
      m_source = cs::UsbCamera(m_id, m_usbPath);
    } else if (m_cameraType == "http") {
      m_source = cs::HttpCamera(m_id, m_urls, cs::HttpCamera::kUnknown);
    } else {
      return;
    }
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

  m_frameThread = std::thread([this, source = m_source] {
    cs::CvSink cvSink{fmt::format("{}_view", m_id), cs::VideoMode::kBGR};
    cvSink.SetSource(source);
    cv::Mat frame;
    while (!m_stopCamera) {
      // get frame from camera
      uint64_t time = cvSink.GrabFrame(frame, 0.25);
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
}

void CameraModel::SetSource(cs::VideoSource source) {
  m_source = source;
  switch (GetKind()) {
    case cs::VideoSource::Kind::kUsb:
      m_cameraType = "usb";
      m_usbPath = static_cast<cs::UsbCamera&>(source).GetPath();
      break;
    case cs::VideoSource::Kind::kHttp:
      m_cameraType = "http";
      m_urls = static_cast<cs::HttpCamera&>(source).GetUrls();
      break;
    case cs::VideoSource::Kind::kCv:
      m_cameraType = "cv";
      break;
    case cs::VideoSource::Kind::kRaw:
      m_cameraType = "raw";
      break;
    default:
      m_cameraType.clear();
      break;
  }
}

void CameraModel::SetUrls(std::span<const std::string> urls) {
  CS_Status status = 0;
  cs::SetHttpCameraUrls(m_source.GetHandle(), urls, &status);
  m_urls.assign(urls.begin(), urls.end());
}

void CameraModel::SetVideoMode(const cs::VideoMode& mode) {
  m_source.SetVideoMode(mode);
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

static void EditProperty(const std::string& name, cs::VideoProperty& prop) {
  switch (prop.GetKind()) {
    case cs::VideoProperty::kBoolean: {
      bool val = prop.Get();
      if (ImGui::Checkbox(name.c_str(), &val)) {
        prop.Set(val);
      }
      break;
    }
    case cs::VideoProperty::kEnum: {
      int val = prop.Get();
      std::vector<std::string> options = prop.GetChoices();
      if (ImGui::BeginCombo(name.c_str(), options[val].c_str())) {
        for (int i = 0; i < static_cast<int>(options.size()); ++i) {
          if (options[i].empty()) {
            continue;
          }
          bool selected = (val == i);
          if (ImGui::Selectable(options[i].c_str(), selected)) {
            prop.Set(i);
          }
          if (selected) {
            ImGui::SetItemDefaultFocus();
          }
        }
        ImGui::EndCombo();
      }
      break;
    }
    case cs::VideoProperty::kInteger: {
      int val = prop.Get();
      // int step = prop.GetStep();
      int min = prop.GetMin();
      int max = prop.GetMax();
      if (min == 0 && max == 1) {
        // treat like boolean
        bool boolVal = val;
        if (ImGui::Checkbox(name.c_str(), &boolVal)) {
          prop.Set(boolVal);
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
        prop.Set(val);
      }
      break;
    }
    case cs::VideoProperty::kString: {
      std::string val = prop.GetString();
      if (ImGui::InputText(name.c_str(), &val)) {
        prop.SetString(val);
      }
      break;
    }
    default:
      break;
  }
}

static bool EditMode(CameraModel* model) {
  bool rv = false;
  cs::VideoSource& source = model->GetSource();
  cs::VideoSource::Kind kind = model->GetKind();

  // video mode; split out width/height/fps for HTTP cameras
  if (kind == cs::VideoSource::kHttp) {
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
    auto mode = source.GetVideoMode();
    char modeStr[64];
    VideoModeToString(modeStr, sizeof(modeStr), mode);
    if (ImGui::BeginCombo("Video Mode", modeStr)) {
      auto modes = source.EnumerateVideoModes();
      for (auto&& amode : modes) {
        bool selected = (amode == mode);
        VideoModeToString(modeStr, sizeof(modeStr), amode);
        if (ImGui::Selectable(modeStr, selected)) {
          model->SetVideoMode(amode);
          rv = true;
        }
        if (selected) {
          ImGui::SetItemDefaultFocus();
        }
      }
      ImGui::EndCombo();
    }
  }
  return rv;
}

static void EditProperties(cs::VideoSource& source) {
  if (ImGui::CollapsingHeader("Properties")) {
    for (auto&& prop : source.EnumerateProperties()) {
      std::string name = prop.GetName();
      if (!wpi::starts_with(name, "raw_")) {
        EditProperty(name, prop);
      }
    }
  }

  if (ImGui::CollapsingHeader("Raw Properties")) {
    for (auto&& prop : source.EnumerateProperties()) {
      std::string name = prop.GetName();
      if (wpi::starts_with(name, "raw_")) {
        EditProperty(name, prop);
      }
    }
  }
}

void glass::DisplayCameraSettings(CameraModel* model) {
  auto& storage = GetStorage();

  bool& showFpsDataRate = storage.GetBool("showFpsDataRate", true);
  ImGui::Checkbox("Show FPS and data rate", &showFpsDataRate);

  bool& streamEnabled = storage.GetBool("streamEnabled", true);
  ImGui::Checkbox("Stream enabled", &streamEnabled);

  EditMode(model);
  EditProperties(model->GetSource());
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
  if (ImGui::BeginDragDropTarget()) {
    if (auto payload = ImGui::AcceptDragDropPayload("Camera")) {
      glass::GetStorage().GetString("camera").assign(
          static_cast<const char*>(payload->Data), payload->DataSize);
    }
    ImGui::EndDragDropTarget();
  }

  if (showFpsDataRate) {
    ImGui::Text("%2.1f FPS", model->GetActualFPS());

    char buf[64];
    wpi::format_to_n_c_str(buf, sizeof(buf), "{:.1f} Mbps",
                           model->GetActualDataRate() * 8 / 1000000);
    ImGui::SameLine(size.x - ImGui::CalcTextSize(buf).x -
                    ImGui::GetStyle().ItemSpacing.x);
    ImGui::TextUnformatted(buf);
  }
}

void glass::DisplayCameraWindow(CameraModel* model) {
  auto& storage = GetStorage();
  bool showFpsDataRate = storage.GetBool("showFpsDataRate", true);

  ImVec2 size =
      ImGui::GetWindowContentRegionMax() - ImGui::GetWindowContentRegionMin();
  if (showFpsDataRate) {
    size -= ImVec2(0, ImGui::GetTextLineHeightWithSpacing());
  }
  DisplayCamera(model, size, showFpsDataRate);
}

void CameraView::Display() {
  if (ImGui::BeginPopupContextItem()) {
    DisplayCameraSettings(m_model);
    ImGui::EndPopup();
  }
  auto& storage = GetStorage();
  bool showFpsDataRate = storage.GetBool("showFpsDataRate", true);

  ImVec2 size =
      ImGui::GetWindowContentRegionMax() - ImGui::GetWindowContentRegionMin();
  if (showFpsDataRate) {
    size -= ImVec2(0, ImGui::GetTextLineHeightWithSpacing());
  }
  DisplayCamera(m_model, size, showFpsDataRate);
}

void CameraView::Hidden() {}
