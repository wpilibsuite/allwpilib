// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/other/Mechanism2D.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <frc/geometry/Pose2d.h>
#include <frc/geometry/Rotation2d.h>
#include <frc/geometry/Transform2d.h>
#include <frc/geometry/Translation2d.h>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_stdlib.h>
#include <portable-file-dialogs.h>
#include <units/angle.h>
#include <units/length.h>
#include <wpi/print.h>
#include <wpigui.h>

#include "glass/Context.h"
#include "glass/Storage.h"

using namespace glass;

namespace gui = wpi::gui;

namespace {

// Per-frame data (not persistent)
struct FrameData {
  frc::Translation2d GetPosFromScreen(const ImVec2& cursor) const {
    return {
        units::meter_t{(std::clamp(cursor.x, min.x, max.x) - min.x) / scale},
        units::meter_t{(max.y - std::clamp(cursor.y, min.y, max.y)) / scale}};
  }
  ImVec2 GetScreenFromPos(const frc::Translation2d& pos) const {
    return {min.x + scale * pos.X().to<float>(),
            max.y - scale * pos.Y().to<float>()};
  }
  void DrawObject(ImDrawList* drawList, MechanismObjectModel& objModel,
                  const frc::Pose2d& pose) const;
  void DrawGroup(ImDrawList* drawList, MechanismObjectGroup& group,
                 const frc::Pose2d& pose) const;

  // in screen coordinates
  ImVec2 imageMin;
  ImVec2 imageMax;
  ImVec2 min;
  ImVec2 max;

  float scale;  // scaling from meters to screen units
};

class BackgroundInfo {
 public:
  explicit BackgroundInfo(Storage& storage);

  void DisplaySettings();

  void LoadImage();
  FrameData GetFrameData(ImVec2 min, ImVec2 max, frc::Translation2d dims) const;
  void Draw(ImDrawList* drawList, const FrameData& frameData,
            ImU32 bgColor) const;

 private:
  void Reset();
  bool LoadImageImpl(const std::string& fn);

  std::unique_ptr<pfd::open_file> m_fileOpener;

  std::string& m_filename;
  gui::Texture m_texture;

  // in image pixels
  int m_imageWidth;
  int m_imageHeight;
};

}  // namespace

BackgroundInfo::BackgroundInfo(Storage& storage)
    : m_filename{storage.GetString("image")} {}

void BackgroundInfo::DisplaySettings() {
  if (ImGui::Button("Choose image...")) {
    m_fileOpener = std::make_unique<pfd::open_file>(
        "Choose background image", "",
        std::vector<std::string>{"Image File",
                                 "*.jpg *.jpeg *.png *.bmp *.psd *.tga *.gif "
                                 "*.hdr *.pic *.ppm *.pgm"});
  }
  if (ImGui::Button("Reset background image")) {
    Reset();
  }
}

void BackgroundInfo::Reset() {
  m_texture = gui::Texture{};
  m_filename.clear();
  m_imageWidth = 0;
  m_imageHeight = 0;
}

void BackgroundInfo::LoadImage() {
  if (m_fileOpener && m_fileOpener->ready(0)) {
    auto result = m_fileOpener->result();
    if (!result.empty()) {
      LoadImageImpl(result[0].c_str());
    }
    m_fileOpener.reset();
  }
  if (!m_texture && !m_filename.empty()) {
    if (!LoadImageImpl(m_filename)) {
      m_filename.clear();
    }
  }
}

bool BackgroundInfo::LoadImageImpl(const std::string& fn) {
  wpi::print("GUI: loading background image '{}'\n", fn);
  auto texture = gui::Texture::CreateFromFile(fn.c_str());
  if (!texture) {
    std::puts("GUI: could not read background image");
    return false;
  }
  m_texture = std::move(texture);
  m_imageWidth = m_texture.GetWidth();
  m_imageHeight = m_texture.GetHeight();
  m_filename = fn;
  return true;
}

FrameData BackgroundInfo::GetFrameData(ImVec2 min, ImVec2 max,
                                       frc::Translation2d dims) const {
  // fit the image into the window
  if (m_texture && m_imageHeight != 0 && m_imageWidth != 0) {
    gui::MaxFit(&min, &max, m_imageWidth, m_imageHeight);
  }

  FrameData frameData;
  frameData.imageMin = min;
  frameData.imageMax = max;

  // determine the "active area"
  float width = dims.X().to<float>();
  float height = dims.Y().to<float>();
  gui::MaxFit(&min, &max, width, height);

  frameData.min = min;
  frameData.max = max;
  frameData.scale = (max.x - min.x) / width;
  return frameData;
}

void BackgroundInfo::Draw(ImDrawList* drawList, const FrameData& frameData,
                          ImU32 bgColor) const {
  if (m_texture && m_imageHeight != 0 && m_imageWidth != 0) {
    drawList->AddImage(m_texture, frameData.imageMin, frameData.imageMax);
  } else {
    drawList->AddRectFilled(frameData.min, frameData.max, bgColor);
  }
}

void glass::DisplayMechanism2DSettings(Mechanism2DModel* model) {
  auto& storage = GetStorage();
  auto bg = storage.GetData<BackgroundInfo>();
  if (!bg) {
    storage.SetData(std::make_shared<BackgroundInfo>(storage));
    bg = storage.GetData<BackgroundInfo>();
  }
  bg->DisplaySettings();
}

void FrameData::DrawObject(ImDrawList* drawList, MechanismObjectModel& objModel,
                           const frc::Pose2d& pose) const {
  const char* type = objModel.GetType();
  if (std::string_view{type} == "line") {
    auto startPose =
        pose + frc::Transform2d{frc::Translation2d{}, objModel.GetAngle()};
    auto endPose =
        startPose +
        frc::Transform2d{frc::Translation2d{objModel.GetLength(), 0_m}, 0_deg};
    drawList->AddLine(GetScreenFromPos(startPose.Translation()),
                      GetScreenFromPos(endPose.Translation()),
                      objModel.GetColor(), objModel.GetWeight());
    DrawGroup(drawList, objModel, endPose);
  }
}

void FrameData::DrawGroup(ImDrawList* drawList, MechanismObjectGroup& group,
                          const frc::Pose2d& pose) const {
  group.ForEachObject(
      [&](auto& objModel) { DrawObject(drawList, objModel, pose); });
}

void glass::DisplayMechanism2D(Mechanism2DModel* model,
                               const ImVec2& contentSize) {
  auto& storage = GetStorage();
  auto bg = storage.GetData<BackgroundInfo>();
  if (!bg) {
    storage.SetData(std::make_shared<BackgroundInfo>(storage));
    bg = storage.GetData<BackgroundInfo>();
  }

  if (contentSize.x <= 0 || contentSize.y <= 0) {
    return;
  }

  // screen coords
  ImVec2 cursorPos = ImGui::GetWindowPos() + ImGui::GetCursorPos();

  ImGui::InvisibleButton("background", contentSize);

  // auto mousePos = ImGui::GetIO().MousePos;
  auto drawList = ImGui::GetWindowDrawList();
  // bool isHovered = ImGui::IsItemHovered();

  // background
  bg->LoadImage();
  auto frameData = bg->GetFrameData(cursorPos, cursorPos + contentSize,
                                    model->GetDimensions());
  bg->Draw(drawList, frameData, model->GetBackgroundColor());

  // elements
  model->ForEachRoot([&](auto& rootModel) {
    frameData.DrawGroup(drawList, rootModel,
                        frc::Pose2d{rootModel.GetPosition(), 0_deg});
  });

#if 0
  if (target) {
    // show tooltip and highlight
    auto pos = frameData.GetPosFromScreen(target->poseCenter);
    ImGui::SetTooltip(
        "%s[%d]\nx: %0.3f y: %0.3f rot: %0.3f", target->name.c_str(),
        static_cast<int>(target->index), ConvertDisplayLength(pos.X()),
        ConvertDisplayLength(pos.Y()), ConvertDisplayAngle(target->rot));
  }
#endif
}

void Mechanism2DView::Display() {
  DisplayMechanism2D(m_model, ImGui::GetWindowContentRegionMax() -
                                  ImGui::GetWindowContentRegionMin());
}

void Mechanism2DView::Settings() {
  DisplayMechanism2DSettings(m_model);
}

bool Mechanism2DView::HasSettings() {
  return true;
}
