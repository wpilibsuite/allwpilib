// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/other/Field2D.h"

#include <algorithm>
#include <cmath>
#include <memory>
#include <utility>
#include "frc/geometry/Pose2d.h"

#include <frc/geometry/Rotation2d.h>
#include <frc/geometry/Translation2d.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_internal.h>
#include <portable-file-dialogs.h>
#include <units/angle.h>
#include <units/length.h>
#include <wpi/Path.h>
#include <wpi/SmallString.h>
#include <wpi/StringMap.h>
#include <wpi/json.h>
#include <wpi/raw_istream.h>
#include <wpi/raw_ostream.h>
#include <wpigui.h>

#include "glass/Context.h"

using namespace glass;

namespace gui = wpi::gui;

namespace {

// Per-frame field data (not persistent)
struct FieldFrameData {
  std::pair<double, double> GetPosFromScreen(const ImVec2& cursor) const {
    return {(std::clamp(cursor.x, min.x, max.x) - min.x) / scale,
            (max.y - std::clamp(cursor.y, min.y, max.y)) / scale};
  }

  // in screen coordinates
  ImVec2 imageMin;
  ImVec2 imageMax;
  ImVec2 min;
  ImVec2 max;

  float scale;  // scaling from field units to screen units
};

// Pose drag target info
struct PoseDragTargetInfo {
  FieldObjectModel* objModel = nullptr;
  std::string name;
  size_t index;
  units::radian_t rot;
  ImVec2 poseCenter;  // center of the pose (screen coordinates)
  ImVec2 center;      // center of the target (screen coordinates)
  float radius = 0;   // target radius
  float dist;         // distance from center to mouse
  int corner;         // corner (1 = center)
};

// Pose drag state
struct PoseDragState {
  PoseDragTargetInfo target;
  ImVec2 initialOffset;
  units::radian_t initialAngle = 0_rad;
};

struct DisplayOptions {
  explicit DisplayOptions(const gui::Texture& texture) : texture{texture} {}

  enum Style { kBoxImage = 0, kLine, kLineClosed, kTrack };
  Style style = kBoxImage;
  float weight = 4.0f;
  int color = 0;

  float width = 0.0f;
  float length = 0.0f;

  bool arrows = true;
  int arrowSize = 50;
  float arrowWeight = 4.0f;
  int arrowColor = 0;

  const gui::Texture& texture;
};

// Per-frame pose data (not persistent)
class PoseFrameData {
 public:
  explicit PoseFrameData(const frc::Pose2d& pose, FieldObjectModel& model,
                         size_t index, const FieldFrameData& ffd,
                         const DisplayOptions& displayOptions);
  void SetPosition(double x, double y);
  void SetRotation(units::radian_t rot);
  const frc::Rotation2d& GetRotation() const { return m_pose.Rotation(); }
  const frc::Pose2d& GetPose() const { return m_pose; }
  float GetHitRadius() const { return m_hitRadius; }
  void UpdateFrameData();
  std::pair<int, float> IsHovered(const ImVec2& cursor) const;
  PoseDragTargetInfo GetDragTarget(int corner, float dist) const;
  void HandleDrag(const ImVec2& cursor);
  void Draw(ImDrawList* drawList, std::vector<ImVec2>* center,
            std::vector<ImVec2>* left, std::vector<ImVec2>* right) const;

  // in window coordinates
  ImVec2 m_center;
  ImVec2 m_corners[6];  // 5 and 6 are used for track width
  ImVec2 m_arrow[3];

 private:
  FieldObjectModel& m_model;
  size_t m_index;
  const FieldFrameData& m_ffd;
  const DisplayOptions& m_displayOptions;

  // scaled width/2 and length/2, in screen units
  float m_width2;
  float m_length2;

  float m_hitRadius;

  frc::Pose2d m_pose;
};

class ObjectInfo {
 public:
  static constexpr float kDefaultWidth = 0.6858f;
  static constexpr float kDefaultLength = 0.8204f;

  ObjectInfo();

  std::unique_ptr<pfd::open_file> m_fileOpener;

  DisplayOptions GetDisplayOptions() const;
  void DisplaySettings();
  void DrawLine(ImDrawList* drawList, wpi::ArrayRef<ImVec2> points) const;

  void Reset();
  void LoadImage();
  const gui::Texture& GetTexture() const { return m_texture; }

 private:
  bool LoadImageImpl(const char* fn);

  float* m_pWidth;
  float* m_pLength;

  int* m_pStyle;  // DisplayOptions::Style
  float* m_pWeight;
  int* m_pColor;

  bool* m_pArrows;
  int* m_pArrowSize;
  float* m_pArrowWeight;
  int* m_pArrowColor;

  std::string* m_pFilename;
  gui::Texture m_texture;
};

class FieldInfo {
 public:
  static constexpr float kDefaultWidth = 15.98f;
  static constexpr float kDefaultHeight = 8.21f;

  FieldInfo();

  std::unique_ptr<pfd::open_file> m_fileOpener;
  float* m_pWidth;
  float* m_pHeight;

  void Reset();
  void LoadImage();
  void LoadJson(const wpi::Twine& jsonfile);
  FieldFrameData GetFrameData(ImVec2 min, ImVec2 max) const;
  void Draw(ImDrawList* drawList, const FieldFrameData& frameData) const;

  wpi::StringMap<std::unique_ptr<ObjectInfo>> m_objects;

 private:
  bool LoadImageImpl(const char* fn);

  std::string* m_pFilename;
  gui::Texture m_texture;
  int m_imageWidth;
  int m_imageHeight;
  int* m_pTop;
  int* m_pLeft;
  int* m_pBottom;
  int* m_pRight;
};

}  // namespace

static PoseDragState gDragState;

FieldInfo::FieldInfo() {
  auto& storage = GetStorage();
  m_pFilename = storage.GetStringRef("image");
  m_pTop = storage.GetIntRef("top", 0);
  m_pLeft = storage.GetIntRef("left", 0);
  m_pBottom = storage.GetIntRef("bottom", -1);
  m_pRight = storage.GetIntRef("right", -1);
  m_pWidth = storage.GetFloatRef("width", kDefaultWidth);
  m_pHeight = storage.GetFloatRef("height", kDefaultHeight);
}

void FieldInfo::Reset() {
  m_texture = gui::Texture{};
  m_pFilename->clear();
  m_imageWidth = 0;
  m_imageHeight = 0;
  *m_pTop = 0;
  *m_pLeft = 0;
  *m_pBottom = -1;
  *m_pRight = -1;
}

void FieldInfo::LoadImage() {
  if (m_fileOpener && m_fileOpener->ready(0)) {
    auto result = m_fileOpener->result();
    if (!result.empty()) {
      if (wpi::StringRef(result[0]).endswith(".json")) {
        LoadJson(result[0]);
      } else {
        LoadImageImpl(result[0].c_str());
        *m_pTop = 0;
        *m_pLeft = 0;
        *m_pBottom = -1;
        *m_pRight = -1;
      }
    }
    m_fileOpener.reset();
  }
  if (!m_texture && !m_pFilename->empty()) {
    if (!LoadImageImpl(m_pFilename->c_str())) {
      m_pFilename->clear();
    }
  }
}

void FieldInfo::LoadJson(const wpi::Twine& jsonfile) {
  std::error_code ec;
  wpi::raw_fd_istream f(jsonfile, ec);
  if (ec) {
    wpi::errs() << "GUI: could not open field JSON file\n";
    return;
  }

  // parse file
  wpi::json j;
  try {
    j = wpi::json::parse(f);
  } catch (const wpi::json::parse_error& e) {
    wpi::errs() << "GUI: JSON: could not parse: " << e.what() << '\n';
  }

  // top level must be an object
  if (!j.is_object()) {
    wpi::errs() << "GUI: JSON: does not contain a top object\n";
    return;
  }

  // image filename
  std::string image;
  try {
    image = j.at("field-image").get<std::string>();
  } catch (const wpi::json::exception& e) {
    wpi::errs() << "GUI: JSON: could not read field-image: " << e.what()
                << '\n';
    return;
  }

  // corners
  int top, left, bottom, right;
  try {
    top = j.at("field-corners").at("top-left").at(1).get<int>();
    left = j.at("field-corners").at("top-left").at(0).get<int>();
    bottom = j.at("field-corners").at("bottom-right").at(1).get<int>();
    right = j.at("field-corners").at("bottom-right").at(0).get<int>();
  } catch (const wpi::json::exception& e) {
    wpi::errs() << "GUI: JSON: could not read field-corners: " << e.what()
                << '\n';
    return;
  }

  // size
  float width;
  float height;
  try {
    width = j.at("field-size").at(0).get<float>();
    height = j.at("field-size").at(1).get<float>();
  } catch (const wpi::json::exception& e) {
    wpi::errs() << "GUI: JSON: could not read field-size: " << e.what() << '\n';
    return;
  }

  // units for size
  std::string unit;
  try {
    unit = j.at("field-unit").get<std::string>();
  } catch (const wpi::json::exception& e) {
    wpi::errs() << "GUI: JSON: could not read field-unit: " << e.what() << '\n';
    return;
  }

  // convert size units to meters
  if (unit == "foot" || unit == "feet") {
    width = units::convert<units::feet, units::meters>(width);
    height = units::convert<units::feet, units::meters>(height);
  }

  // the image filename is relative to the json file
  wpi::SmallString<128> pathname;
  jsonfile.toVector(pathname);
  wpi::sys::path::remove_filename(pathname);
  wpi::sys::path::append(pathname, image);

  // load field image
  if (!LoadImageImpl(pathname.c_str())) {
    return;
  }

  // save to field info
  *m_pFilename = pathname.str();
  *m_pTop = top;
  *m_pLeft = left;
  *m_pBottom = bottom;
  *m_pRight = right;
  *m_pWidth = width;
  *m_pHeight = height;
}

bool FieldInfo::LoadImageImpl(const char* fn) {
  wpi::outs() << "GUI: loading field image '" << fn << "'\n";
  auto texture = gui::Texture::CreateFromFile(fn);
  if (!texture) {
    wpi::errs() << "GUI: could not read field image\n";
    return false;
  }
  m_texture = std::move(texture);
  m_imageWidth = m_texture.GetWidth();
  m_imageHeight = m_texture.GetHeight();
  *m_pFilename = fn;
  return true;
}

FieldFrameData FieldInfo::GetFrameData(ImVec2 min, ImVec2 max) const {
  // fit the image into the window
  if (m_texture && m_imageHeight != 0 && m_imageWidth != 0) {
    gui::MaxFit(&min, &max, m_imageWidth, m_imageHeight);
  }

  FieldFrameData ffd;
  ffd.imageMin = min;
  ffd.imageMax = max;

  // size down the box by the image corners (if any)
  if (*m_pBottom > 0 && *m_pRight > 0) {
    min.x += *m_pLeft * (max.x - min.x) / m_imageWidth;
    min.y += *m_pTop * (max.y - min.y) / m_imageHeight;
    max.x -= (m_imageWidth - *m_pRight) * (max.x - min.x) / m_imageWidth;
    max.y -= (m_imageHeight - *m_pBottom) * (max.y - min.y) / m_imageHeight;
  }

  // draw the field "active area" as a yellow boundary box
  gui::MaxFit(&min, &max, *m_pWidth, *m_pHeight);

  ffd.min = min;
  ffd.max = max;
  ffd.scale = (max.x - min.x) / *m_pWidth;
  return ffd;
}

void FieldInfo::Draw(ImDrawList* drawList, const FieldFrameData& ffd) const {
  if (m_texture && m_imageHeight != 0 && m_imageWidth != 0) {
    drawList->AddImage(m_texture, ffd.imageMin, ffd.imageMax);
  }

  // draw the field "active area" as a yellow boundary box
  drawList->AddRect(ffd.min, ffd.max, IM_COL32(255, 255, 0, 255));
}

ObjectInfo::ObjectInfo() {
  auto& storage = GetStorage();
  m_pFilename = storage.GetStringRef("image");
  m_pWidth = storage.GetFloatRef("width", kDefaultWidth);
  m_pLength = storage.GetFloatRef("length", kDefaultLength);
  m_pStyle = storage.GetIntRef("style", DisplayOptions::kBoxImage);
  m_pWeight = storage.GetFloatRef("weight", 4.0);
  m_pColor = storage.GetIntRef("color", IM_COL32(255, 0, 0, 255));
  m_pArrows = storage.GetBoolRef("arrows", true);
  m_pArrowSize = storage.GetIntRef("arrowSize", 50);
  m_pArrowWeight = storage.GetFloatRef("arrowWeight", 4.0);
  m_pArrowColor = storage.GetIntRef("arrowColor", IM_COL32(0, 255, 0, 255));
}

DisplayOptions ObjectInfo::GetDisplayOptions() const {
  DisplayOptions rv{m_texture};
  rv.style = static_cast<DisplayOptions::Style>(*m_pStyle);
  rv.weight = *m_pWeight;
  rv.color = *m_pColor;
  rv.width = *m_pWidth;
  rv.length = *m_pLength;
  rv.arrows = *m_pArrows;
  rv.arrowSize = *m_pArrowSize;
  rv.arrowWeight = *m_pArrowWeight;
  rv.arrowColor = *m_pArrowColor;
  return rv;
}

void ObjectInfo::DisplaySettings() {
  static const char* styleChoices[] = {"Box/Image", "Line", "Line (Closed)",
                                       "Track"};
  ImGui::SetNextItemWidth(ImGui::GetFontSize() * 8);
  ImGui::Combo("Style", m_pStyle, styleChoices,
               sizeof(styleChoices) / sizeof(styleChoices[0]));
  switch (*m_pStyle) {
    case DisplayOptions::kBoxImage:
      if (ImGui::Button("Choose image...")) {
        m_fileOpener = std::make_unique<pfd::open_file>(
            "Choose object image", "",
            std::vector<std::string>{
                "Image File",
                "*.jpg *.jpeg *.png *.bmp *.psd *.tga *.gif "
                "*.hdr *.pic *.ppm *.pgm"});
      }
      if (ImGui::Button("Reset image")) {
        Reset();
      }
      ImGui::InputFloat("Width", m_pWidth);
      ImGui::InputFloat("Length", m_pLength);
      break;
    case DisplayOptions::kTrack:
      ImGui::InputFloat("Width", m_pWidth);
      break;
    default:
      break;
  }
  ImGui::InputFloat("Line Weight", m_pWeight);
  ImColor col(*m_pColor);
  if (ImGui::ColorEdit3("Line Color", &col.Value.x,
                        ImGuiColorEditFlags_NoInputs)) {
    *m_pColor = col;
  }
  ImGui::Checkbox("Arrows", m_pArrows);
  if (*m_pArrows) {
    ImGui::SliderInt("Arrow Size", m_pArrowSize, 0, 100, "%d%%",
                     ImGuiSliderFlags_AlwaysClamp);
    ImGui::InputFloat("Arrow Weight", m_pArrowWeight);
    ImColor col(*m_pArrowColor);
    if (ImGui::ColorEdit3("Arrow Color", &col.Value.x,
                          ImGuiColorEditFlags_NoInputs)) {
      *m_pArrowColor = col;
    }
  }
}

void ObjectInfo::DrawLine(ImDrawList* drawList,
                          wpi::ArrayRef<ImVec2> points) const {
  if (points.empty()) {
    return;
  }

  if (points.size() == 1) {
    drawList->AddCircleFilled(points.front(), *m_pWeight, *m_pWeight);
    return;
  }

  // PolyLine doesn't handle acute angles well; workaround from
  // https://github.com/ocornut/imgui/issues/3366
  size_t i = 0;
  while (i + 1 < points.size()) {
    int nlin = 2;
    while (i + nlin < points.size()) {
      auto [x0, y0] = points[i + nlin - 2];
      auto [x1, y1] = points[i + nlin - 1];
      auto [x2, y2] = points[i + nlin];
      auto s0x = x1 - x0, s0y = y1 - y0;
      auto s1x = x2 - x1, s1y = y2 - y1;
      auto dotprod = s1x * s0x + s1y * s0y;
      if (dotprod < 0) {
        break;
      }
      ++nlin;
    }

    drawList->AddPolyline(&points[i], nlin, *m_pColor, false, *m_pWeight);
    i += nlin - 1;
  }

  if (points.size() > 2 && *m_pStyle == DisplayOptions::kLineClosed) {
    drawList->AddLine(points.back(), points.front(), *m_pColor, *m_pWeight);
  }
}

void ObjectInfo::Reset() {
  m_texture = gui::Texture{};
  m_pFilename->clear();
}

void ObjectInfo::LoadImage() {
  if (m_fileOpener && m_fileOpener->ready(0)) {
    auto result = m_fileOpener->result();
    if (!result.empty()) {
      LoadImageImpl(result[0].c_str());
    }
    m_fileOpener.reset();
  }
  if (!m_texture && !m_pFilename->empty()) {
    if (!LoadImageImpl(m_pFilename->c_str())) {
      m_pFilename->clear();
    }
  }
}

bool ObjectInfo::LoadImageImpl(const char* fn) {
  wpi::outs() << "GUI: loading object image '" << fn << "'\n";
  auto texture = gui::Texture::CreateFromFile(fn);
  if (!texture) {
    wpi::errs() << "GUI: could not read object image\n";
    return false;
  }
  m_texture = std::move(texture);
  *m_pFilename = fn;
  return true;
}

PoseFrameData::PoseFrameData(const frc::Pose2d& pose, FieldObjectModel& model,
                             size_t index, const FieldFrameData& ffd,
                             const DisplayOptions& displayOptions)
    : m_model{model},
      m_index{index},
      m_ffd{ffd},
      m_displayOptions{displayOptions},
      m_width2(ffd.scale * displayOptions.width / 2),
      m_length2(ffd.scale * displayOptions.length / 2),
      m_hitRadius((std::min)(m_width2, m_length2) / 2),
      m_pose{pose} {
  UpdateFrameData();
}

void PoseFrameData::SetPosition(double x, double y) {
  m_pose = frc::Pose2d{frc::Translation2d{units::meter_t{x}, units::meter_t{y}},
                       m_pose.Rotation()};
  m_model.SetPose(m_index, m_pose);
}

void PoseFrameData::SetRotation(units::radian_t rot) {
  m_pose = frc::Pose2d{m_pose.Translation(), rot};
  m_model.SetPose(m_index, m_pose);
}

void PoseFrameData::UpdateFrameData() {
  // (0,0) origin is bottom left
  ImVec2 center(m_ffd.min.x + m_ffd.scale * m_pose.X().to<double>(),
                m_ffd.max.y - m_ffd.scale * m_pose.Y().to<double>());

  // build rotated points around center
  float length2 = m_length2;
  float width2 = m_width2;
  auto& rot = GetRotation();
  float cos_a = rot.Cos();
  float sin_a = -rot.Sin();

  m_corners[0] = center + ImRotate(ImVec2(-length2, -width2), cos_a, sin_a);
  m_corners[1] = center + ImRotate(ImVec2(length2, -width2), cos_a, sin_a);
  m_corners[2] = center + ImRotate(ImVec2(length2, width2), cos_a, sin_a);
  m_corners[3] = center + ImRotate(ImVec2(-length2, width2), cos_a, sin_a);
  m_corners[4] = center + ImRotate(ImVec2(0, -width2), cos_a, sin_a);
  m_corners[5] = center + ImRotate(ImVec2(0, width2), cos_a, sin_a);

  float arrowScale = m_displayOptions.arrowSize / 100.0f;
  m_arrow[0] =
      center + ImRotate(ImVec2(-length2 * arrowScale, -width2 * arrowScale),
                        cos_a, sin_a);
  m_arrow[1] = center + ImRotate(ImVec2(length2 * arrowScale, 0), cos_a, sin_a);
  m_arrow[2] =
      center + ImRotate(ImVec2(-length2 * arrowScale, width2 * arrowScale),
                        cos_a, sin_a);

  m_center = center;
}

std::pair<int, float> PoseFrameData::IsHovered(const ImVec2& cursor) const {
  float hitRadiusSquared = m_hitRadius * m_hitRadius;
  float dist;

  // it's within the hit radius of the center?
  dist = gui::GetDistSquared(cursor, m_center);
  if (dist < hitRadiusSquared) {
    return {1, dist};
  }

  if (m_displayOptions.style == DisplayOptions::kBoxImage) {
    dist = gui::GetDistSquared(cursor, m_corners[0]);
    if (dist < hitRadiusSquared) {
      return {2, dist};
    }

    dist = gui::GetDistSquared(cursor, m_corners[1]);
    if (dist < hitRadiusSquared) {
      return {3, dist};
    }

    dist = gui::GetDistSquared(cursor, m_corners[2]);
    if (dist < hitRadiusSquared) {
      return {4, dist};
    }

    dist = gui::GetDistSquared(cursor, m_corners[3]);
    if (dist < hitRadiusSquared) {
      return {5, dist};
    }
  } else if (m_displayOptions.style == DisplayOptions::kTrack) {
    dist = gui::GetDistSquared(cursor, m_corners[4]);
    if (dist < hitRadiusSquared) {
      return {6, dist};
    }

    dist = gui::GetDistSquared(cursor, m_corners[5]);
    if (dist < hitRadiusSquared) {
      return {7, dist};
    }
  }

  return {0, 0.0};
}

PoseDragTargetInfo PoseFrameData::GetDragTarget(int corner, float dist) const {
  PoseDragTargetInfo info;
  info.objModel = &m_model;
  info.rot = GetRotation().Radians();
  info.poseCenter = m_center;
  if (corner == 1) {
    info.center = m_center;
  } else {
    info.center = m_corners[corner - 2];
  }
  info.radius = m_hitRadius;
  info.dist = dist;
  info.corner = corner;
  return info;
}

void PoseFrameData::HandleDrag(const ImVec2& cursor) {
  if (gDragState.target.corner == 1) {
    auto [x, y] = m_ffd.GetPosFromScreen(cursor - gDragState.initialOffset);
    SetPosition(x, y);
    UpdateFrameData();
    gDragState.target.center = m_center;
    gDragState.target.poseCenter = m_center;
  } else {
    ImVec2 off = cursor - m_center;
    SetRotation(gDragState.initialAngle -
                units::radian_t{std::atan2(off.y, off.x)});
    gDragState.target.center = m_corners[gDragState.target.corner - 2];
    gDragState.target.rot = GetRotation().Radians();
  }
}

void PoseFrameData::Draw(ImDrawList* drawList, std::vector<ImVec2>* center,
                         std::vector<ImVec2>* left,
                         std::vector<ImVec2>* right) const {
  switch (m_displayOptions.style) {
    case DisplayOptions::kBoxImage:
      if (m_displayOptions.texture) {
        drawList->AddImageQuad(m_displayOptions.texture, m_corners[0],
                               m_corners[1], m_corners[2], m_corners[3]);
        return;
      }
      drawList->AddQuad(m_corners[0], m_corners[1], m_corners[2], m_corners[3],
                        m_displayOptions.color, m_displayOptions.weight);
      break;
    case DisplayOptions::kLine:
    case DisplayOptions::kLineClosed:
      center->emplace_back(m_center);
      break;
    case DisplayOptions::kTrack:
      center->emplace_back(m_center);
      left->emplace_back(m_corners[4]);
      right->emplace_back(m_corners[5]);
      break;
  }

  if (m_displayOptions.arrows) {
    drawList->AddTriangle(m_arrow[0], m_arrow[1], m_arrow[2],
                          m_displayOptions.arrowColor,
                          m_displayOptions.arrowWeight);
  }
}

void glass::DisplayField2DSettings(Field2DModel* model) {
  auto& storage = GetStorage();
  auto field = storage.GetData<FieldInfo>();
  if (!field) {
    storage.SetData(std::make_shared<FieldInfo>());
    field = storage.GetData<FieldInfo>();
  }

  ImGui::PushItemWidth(ImGui::GetFontSize() * 4);
  if (ImGui::CollapsingHeader("Field")) {
    ImGui::PushID("Field");
    if (ImGui::Button("Choose image...")) {
      field->m_fileOpener = std::make_unique<pfd::open_file>(
          "Choose field image", "",
          std::vector<std::string>{"Image File",
                                   "*.jpg *.jpeg *.png *.bmp *.psd *.tga *.gif "
                                   "*.hdr *.pic *.ppm *.pgm",
                                   "PathWeaver JSON File", "*.json"});
    }
    if (ImGui::Button("Reset image")) {
      field->Reset();
    }
    ImGui::InputFloat("Field Width", field->m_pWidth);
    ImGui::InputFloat("Field Height", field->m_pHeight);
    // ImGui::InputInt("Field Top", field->m_pTop);
    // ImGui::InputInt("Field Left", field->m_pLeft);
    // ImGui::InputInt("Field Right", field->m_pRight);
    // ImGui::InputInt("Field Bottom", field->m_pBottom);
    ImGui::PopID();
  }

  model->ForEachFieldObject([&](auto& objModel, auto name) {
    if (!objModel.Exists()) {
      return;
    }
    PushID(name);
    auto& objRef = field->m_objects[name];
    if (!objRef) {
      objRef = std::make_unique<ObjectInfo>();
    }
    auto obj = objRef.get();

    wpi::SmallString<64> nameBuf = name;
    if (ImGui::CollapsingHeader(nameBuf.c_str())) {
      obj->DisplaySettings();
    }
    PopID();
  });
  ImGui::PopItemWidth();
}

namespace {
class FieldDisplay {
 public:
  void Display(FieldInfo* field, Field2DModel* model,
               const ImVec2& contentSize);

 private:
  void DisplayObject(FieldObjectModel& model, wpi::StringRef name);

  FieldInfo* m_field;
  ImVec2 m_mousePos;
  ImDrawList* m_drawList;

  // only allow initiation of dragging when invisible button is hovered;
  // this prevents the window resize handles from simultaneously activating
  // the drag functionality
  bool m_isHovered;

  FieldFrameData m_ffd;

  // drag targets
  std::vector<PoseDragTargetInfo> m_targets;

  // splitter so lines are put behind arrows
  ImDrawListSplitter m_drawSplit;

  // lines; static so buffer gets reused
  std::vector<ImVec2> m_centerLine, m_leftLine, m_rightLine;
};
}  // namespace

void FieldDisplay::Display(FieldInfo* field, Field2DModel* model,
                           const ImVec2& contentSize) {
  // screen coords
  ImVec2 cursorPos = ImGui::GetWindowPos() + ImGui::GetCursorPos();

  // for dragging to work, there needs to be a button (otherwise the window is
  // dragged)
  ImGui::InvisibleButton("field", contentSize);

  m_field = field;
  m_mousePos = ImGui::GetIO().MousePos;
  m_drawList = ImGui::GetWindowDrawList();
  m_isHovered = ImGui::IsItemHovered();

  // field
  field->LoadImage();
  m_ffd = field->GetFrameData(cursorPos, cursorPos + contentSize);
  field->Draw(m_drawList, m_ffd);

  // stop dragging if mouse button not down
  bool isDown = ImGui::IsMouseDown(0);
  if (!isDown) {
    gDragState.target.objModel = nullptr;
    gDragState.target.radius = 0;
  }

  // field objects
  m_targets.resize(0);
  m_drawSplit.Split(m_drawList, 2);
  model->ForEachFieldObject(
      [this](auto& objModel, auto name) { DisplayObject(objModel, name); });
  m_drawSplit.Merge(m_drawList);

  PoseDragTargetInfo* target = &gDragState.target;

  if (!m_targets.empty()) {
    // Find the "best" drag target of the available options.  Prefer
    // center to non-center, and then pick the closest hit.
    std::sort(m_targets.begin(), m_targets.end(),
              [](const auto& a, const auto& b) {
                return a.corner == 0 || a.dist < b.dist;
              });
    target = &m_targets.front();

    if (ImGui::IsMouseClicked(0)) {
      // initialize drag state
      gDragState.target = *target;
      gDragState.initialOffset = m_mousePos - target->poseCenter;
      if (target->corner != 1) {
        gDragState.initialAngle =
            units::radian_t{std::atan2(gDragState.initialOffset.y,
                                       gDragState.initialOffset.x)} +
            target->rot;
      }
    }
  }

  // draw the target circle; also draw a smaller circle on the pose center
  if (target->radius != 0) {
    auto [x, y] = m_ffd.GetPosFromScreen(target->poseCenter);
    ImGui::SetTooltip("%s[%d]\nx: %0.3f y: %0.3f rot: %0.3f",
                      target->name.c_str(), static_cast<int>(target->index), x,
                      y, target->rot.convert<units::degree>().to<double>());
    m_drawList->AddCircle(target->center, target->radius,
                          IM_COL32(0, 255, 0, 255));
    if (target->corner != 1) {
      m_drawList->AddCircle(target->poseCenter, target->radius / 2.0,
                            IM_COL32(0, 255, 0, 255));
    }
  }
}

void FieldDisplay::DisplayObject(FieldObjectModel& model, wpi::StringRef name) {
  if (!model.Exists()) {
    return;
  }
  PushID(name);
  auto& objRef = m_field->m_objects[name];
  if (!objRef) {
    objRef = std::make_unique<ObjectInfo>();
  }
  auto obj = objRef.get();
  obj->LoadImage();

  auto displayOptions = obj->GetDisplayOptions();

  m_centerLine.resize(0);
  m_leftLine.resize(0);
  m_rightLine.resize(0);

  m_drawSplit.SetCurrentChannel(m_drawList, 1);
  size_t i = 0;
  for (auto&& pose : model.GetPoses()) {
    PoseFrameData pfd{pose, model, i, m_ffd, displayOptions};

    // check for potential drag targets
    if (m_isHovered && !gDragState.target.objModel) {
      auto [corner, dist] = pfd.IsHovered(m_mousePos);
      if (corner > 0) {
        m_targets.emplace_back(pfd.GetDragTarget(corner, dist));
        m_targets.back().name = name;
        m_targets.back().index = i;
      }
    }

    // handle active dragging of this object
    if (gDragState.target.objModel == &model && gDragState.target.index == i) {
      pfd.HandleDrag(m_mousePos);
    }

    // draw
    pfd.Draw(m_drawList, &m_centerLine, &m_leftLine, &m_rightLine);
    ++i;
  }

  m_drawSplit.SetCurrentChannel(m_drawList, 0);
  obj->DrawLine(m_drawList, m_centerLine);
  obj->DrawLine(m_drawList, m_leftLine);
  obj->DrawLine(m_drawList, m_rightLine);

  PopID();
}

void glass::DisplayField2D(Field2DModel* model, const ImVec2& contentSize) {
  auto& storage = GetStorage();
  auto field = storage.GetData<FieldInfo>();
  if (!field) {
    storage.SetData(std::make_shared<FieldInfo>());
    field = storage.GetData<FieldInfo>();
  }

  if (contentSize.x <= 0 || contentSize.y <= 0) {
    return;
  }

  static FieldDisplay display;
  display.Display(field, model, contentSize);
}

void Field2DView::Display() {
  if (ImGui::BeginPopupContextItem()) {
    DisplayField2DSettings(m_model);
    ImGui::EndPopup();
  }
  DisplayField2D(m_model, ImGui::GetWindowContentRegionMax() -
                              ImGui::GetWindowContentRegionMin());
}
