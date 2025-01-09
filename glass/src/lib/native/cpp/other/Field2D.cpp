// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/other/Field2D.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <fields/fields.h>
#include <frc/geometry/Pose2d.h>
#include <frc/geometry/Rotation2d.h>
#include <frc/geometry/Translation2d.h>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_stdlib.h>
#include <portable-file-dialogs.h>
#include <units/angle.h>
#include <units/length.h>
#include <wpi/MemoryBuffer.h>
#include <wpi/SmallString.h>
#include <wpi/StringExtras.h>
#include <wpi/StringMap.h>
#include <wpi/fs.h>
#include <wpi/json.h>
#include <wpi/print.h>
#include <wpigui.h>

#include "glass/Context.h"
#include "glass/Storage.h"
#include "glass/support/ColorSetting.h"
#include "glass/support/EnumSetting.h"

using namespace glass;

namespace gui = wpi::gui;

namespace {

enum DisplayUnits { kDisplayMeters = 0, kDisplayFeet, kDisplayInches };

// Per-frame field data (not persistent)
struct FieldFrameData {
  frc::Translation2d GetPosFromScreen(const ImVec2& cursor) const {
    return {
        units::meter_t{(std::clamp(cursor.x, min.x, max.x) - min.x) / scale},
        units::meter_t{(max.y - std::clamp(cursor.y, min.y, max.y)) / scale}};
  }
  ImVec2 GetScreenFromPos(const frc::Translation2d& pos) const {
    return {min.x + scale * pos.X().to<float>(),
            max.y - scale * pos.Y().to<float>()};
  }

  // in screen coordinates
  ImVec2 imageMin;
  ImVec2 imageMax;
  ImVec2 min;
  ImVec2 max;

  float scale;  // scaling from meters to screen units
};

// Pose drag target info
struct SelectedTargetInfo {
  FieldObjectModel* objModel = nullptr;
  std::string name;
  size_t index;
  units::radian_t rot;
  ImVec2 poseCenter;  // center of the pose (screen coordinates)
  ImVec2 center;      // center of the target (screen coordinates)
  float radius;       // target radius
  float dist;         // distance from center to mouse
  int corner;         // corner (1 = center)
};

// Pose drag state
struct PoseDragState {
  SelectedTargetInfo target;
  ImVec2 initialOffset;
  units::radian_t initialAngle = 0_rad;
};

// Popup edit state
class PopupState {
 public:
  void Open(SelectedTargetInfo* target, const frc::Translation2d& pos);
  void Close();

  SelectedTargetInfo* GetTarget() { return &m_target; }
  FieldObjectModel* GetInsertModel() { return m_insertModel; }
  std::span<const frc::Pose2d> GetInsertPoses() const { return m_insertPoses; }

  void Display(Field2DModel* model, const FieldFrameData& ffd);

 private:
  void DisplayTarget(Field2DModel* model, const FieldFrameData& ffd);
  void DisplayInsert(Field2DModel* model);

  SelectedTargetInfo m_target;

  // for insert
  FieldObjectModel* m_insertModel;
  std::vector<frc::Pose2d> m_insertPoses;
  std::string m_insertName;
  int m_insertIndex;
};

struct DisplayOptions {
  explicit DisplayOptions(const gui::Texture& texture) : texture{texture} {}

  enum Style { kBoxImage = 0, kLine, kLineClosed, kTrack, kHidden };

  static constexpr Style kDefaultStyle = kBoxImage;
  static constexpr float kDefaultWeight = 4.0f;
  static constexpr float kDefaultColorFloat[] = {255, 0, 0, 255};
  static constexpr ImU32 kDefaultColor = IM_COL32(255, 0, 0, 255);
  static constexpr auto kDefaultWidth = 0.6858_m;
  static constexpr auto kDefaultLength = 0.8204_m;
  static constexpr bool kDefaultArrows = true;
  static constexpr int kDefaultArrowSize = 50;
  static constexpr float kDefaultArrowWeight = 4.0f;
  static constexpr float kDefaultArrowColorFloat[] = {0, 255, 0, 255};
  static constexpr ImU32 kDefaultArrowColor = IM_COL32(0, 255, 0, 255);
  static constexpr bool kDefaultSelectable = true;

  Style style = kDefaultStyle;
  float weight = kDefaultWeight;
  int color = kDefaultColor;

  units::meter_t width = kDefaultWidth;
  units::meter_t length = kDefaultLength;

  bool arrows = kDefaultArrows;
  int arrowSize = kDefaultArrowSize;
  float arrowWeight = kDefaultArrowWeight;
  int arrowColor = kDefaultArrowColor;

  bool selectable = kDefaultSelectable;

  const gui::Texture& texture;
};

// Per-frame pose data (not persistent)
class PoseFrameData {
 public:
  explicit PoseFrameData(const frc::Pose2d& pose, FieldObjectModel& model,
                         size_t index, const FieldFrameData& ffd,
                         const DisplayOptions& displayOptions);
  void SetPosition(const frc::Translation2d& pos);
  void SetRotation(units::radian_t rot);
  const frc::Rotation2d& GetRotation() const { return m_pose.Rotation(); }
  const frc::Pose2d& GetPose() const { return m_pose; }
  float GetHitRadius() const { return m_hitRadius; }
  void UpdateFrameData();
  std::pair<int, float> IsHovered(const ImVec2& cursor) const;
  SelectedTargetInfo GetDragTarget(int corner, float dist) const;
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
  explicit ObjectInfo(Storage& storage);

  DisplayOptions GetDisplayOptions() const;
  void DisplaySettings();
  void DrawLine(ImDrawList* drawList, std::span<const ImVec2> points) const;

  void LoadImage();
  const gui::Texture& GetTexture() const { return m_texture; }

 private:
  void Reset();
  bool LoadImageImpl(const std::string& fn);

  std::unique_ptr<pfd::open_file> m_fileOpener;

  // in meters
  float& m_width;
  float& m_length;

  EnumSetting m_style;  // DisplayOptions::Style
  float& m_weight;
  ColorSetting m_color;

  bool& m_arrows;
  int& m_arrowSize;
  float& m_arrowWeight;
  ColorSetting m_arrowColor;

  bool& m_selectable;

  std::string& m_filename;
  gui::Texture m_texture;
};

class FieldInfo {
 public:
  static constexpr auto kDefaultWidth = 16.541052_m;
  static constexpr auto kDefaultHeight = 8.211_m;

  explicit FieldInfo(Storage& storage);

  void DisplaySettings();

  void LoadImage();
  FieldFrameData GetFrameData(ImVec2 min, ImVec2 max) const;
  void Draw(ImDrawList* drawList, const FieldFrameData& frameData) const;

  wpi::StringMap<ObjectInfo> m_objects;

 private:
  void Reset();
  bool LoadImageImpl(const std::string& fn);
  bool LoadJson(std::span<const char> is, std::string_view filename);
  void LoadJsonFile(std::string_view jsonfile);

  std::unique_ptr<pfd::open_file> m_fileOpener;

  std::string& m_builtin;
  std::string& m_filename;
  gui::Texture m_texture;

  // in meters
  float& m_width;
  float& m_height;

  // in image pixels
  int m_imageWidth;
  int m_imageHeight;
  int& m_top;
  int& m_left;
  int& m_bottom;
  int& m_right;
};

}  // namespace

static PoseDragState gDragState;
static PopupState gPopupState;
static DisplayUnits gDisplayUnits = kDisplayMeters;

static double ConvertDisplayLength(units::meter_t v) {
  switch (gDisplayUnits) {
    case kDisplayFeet:
      return v.convert<units::feet>().value();
    case kDisplayInches:
      return v.convert<units::inches>().value();
    case kDisplayMeters:
    default:
      return v.value();
  }
}

static double ConvertDisplayAngle(units::degree_t v) {
  return v.value();
}

static bool InputLength(const char* label, units::meter_t* v, double step = 0.0,
                        double step_fast = 0.0, const char* format = "%.6f",
                        ImGuiInputTextFlags flags = 0) {
  double dv = ConvertDisplayLength(*v);
  if (ImGui::InputDouble(label, &dv, step, step_fast, format, flags)) {
    switch (gDisplayUnits) {
      case kDisplayFeet:
        *v = units::foot_t{dv};
        break;
      case kDisplayInches:
        *v = units::inch_t{dv};
        break;
      case kDisplayMeters:
      default:
        *v = units::meter_t{dv};
        break;
    }
    return true;
  }
  return false;
}

static bool InputFloatLength(const char* label, float* v, double step = 0.0,
                             double step_fast = 0.0,
                             const char* format = "%.3f",
                             ImGuiInputTextFlags flags = 0) {
  units::meter_t uv{*v};
  if (InputLength(label, &uv, step, step_fast, format, flags)) {
    *v = uv.to<float>();
    return true;
  }
  return false;
}

static bool InputAngle(const char* label, units::degree_t* v, double step = 0.0,
                       double step_fast = 0.0, const char* format = "%.6f",
                       ImGuiInputTextFlags flags = 0) {
  double dv = ConvertDisplayAngle(*v);
  if (ImGui::InputDouble(label, &dv, step, step_fast, format, flags)) {
    *v = units::degree_t{dv};
    return true;
  }
  return false;
}

static bool InputPose(frc::Pose2d* pose) {
  auto x = pose->X();
  auto y = pose->Y();
  auto rot = pose->Rotation().Degrees();

  bool changed;
  changed = InputLength("x", &x);
  changed = InputLength("y", &y) || changed;
  changed = InputAngle("rot", &rot) || changed;
  if (changed) {
    *pose = frc::Pose2d{x, y, rot};
  }
  return changed;
}

FieldInfo::FieldInfo(Storage& storage)
    : m_builtin{storage.GetString("builtin", "2024 Crescendo")},
      m_filename{storage.GetString("image")},
      m_width{storage.GetFloat("width", kDefaultWidth.to<float>())},
      m_height{storage.GetFloat("height", kDefaultHeight.to<float>())},
      m_top{storage.GetInt("top", 0)},
      m_left{storage.GetInt("left", 0)},
      m_bottom{storage.GetInt("bottom", -1)},
      m_right{storage.GetInt("right", -1)} {}

void FieldInfo::DisplaySettings() {
  ImGui::SetNextItemWidth(ImGui::GetFontSize() * 10);
  if (ImGui::BeginCombo("Image",
                        m_builtin.empty() ? "Custom" : m_builtin.c_str())) {
    if (ImGui::Selectable("Custom", m_builtin.empty())) {
      Reset();
    }
    for (auto&& field : fields::GetFields()) {
      bool selected = field.name == m_builtin;
      if (ImGui::Selectable(field.name, selected)) {
        Reset();
        m_builtin = field.name;
      }
      if (selected) {
        ImGui::SetItemDefaultFocus();
      }
    }
    ImGui::EndCombo();
  }
  if (m_builtin.empty() && ImGui::Button("Load JSON/image...")) {
    m_fileOpener = std::make_unique<pfd::open_file>(
        "Choose field JSON/image", "",
        std::vector<std::string>{"PathWeaver JSON File", "*.json", "Image File",
                                 "*.jpg *.jpeg *.png *.bmp *.psd *.tga *.gif "
                                 "*.hdr *.pic *.ppm *.pgm"});
  }
  if (ImGui::Button("Reset image")) {
    Reset();
  }
  InputFloatLength("Field Width", &m_width);
  InputFloatLength("Field Height", &m_height);
  // ImGui::InputInt("Field Top", &m_top);
  // ImGui::InputInt("Field Left", &m_left);
  // ImGui::InputInt("Field Right", &m_right);
  // ImGui::InputInt("Field Bottom", &m_bottom);
}

void FieldInfo::Reset() {
  m_texture = gui::Texture{};
  m_builtin.clear();
  m_filename.clear();
  m_imageWidth = 0;
  m_imageHeight = 0;
  m_top = 0;
  m_left = 0;
  m_bottom = -1;
  m_right = -1;
}

void FieldInfo::LoadImage() {
  if (m_fileOpener && m_fileOpener->ready(0)) {
    auto result = m_fileOpener->result();
    if (!result.empty()) {
      if (wpi::ends_with(result[0], ".json")) {
        LoadJsonFile(result[0]);
      } else {
        LoadImageImpl(result[0].c_str());
        m_top = 0;
        m_left = 0;
        m_bottom = -1;
        m_right = -1;
      }
    }
    m_fileOpener.reset();
  }
  if (!m_texture) {
    if (!m_builtin.empty()) {
      for (auto&& field : fields::GetFields()) {
        if (field.name == m_builtin) {
          auto jsonstr = field.getJson();
          auto imagedata = field.getImage();
          auto texture = gui::Texture::CreateFromImage(
              reinterpret_cast<const unsigned char*>(imagedata.data()),
              imagedata.size());
          if (texture && LoadJson({jsonstr.data(), jsonstr.size()}, {})) {
            m_texture = std::move(texture);
            m_imageWidth = m_texture.GetWidth();
            m_imageHeight = m_texture.GetHeight();
          } else {
            m_builtin.clear();
          }
        }
      }
    } else if (!m_filename.empty()) {
      if (!LoadImageImpl(m_filename)) {
        m_filename.clear();
      }
    }
  }
}

bool FieldInfo::LoadJson(std::span<const char> is, std::string_view filename) {
  // parse file
  wpi::json j;
  try {
    j = wpi::json::parse(is);
  } catch (const wpi::json::parse_error& e) {
    wpi::print(stderr, "GUI: JSON: could not parse: {}\n", e.what());
    return false;
  }

  // top level must be an object
  if (!j.is_object()) {
    std::fputs("GUI: JSON: does not contain a top object\n", stderr);
    return false;
  }

  // image filename
  std::string image;
  try {
    image = j.at("field-image").get<std::string>();
  } catch (const wpi::json::exception& e) {
    wpi::print(stderr, "GUI: JSON: could not read field-image: {}\n", e.what());
    return false;
  }

  // corners
  int top, left, bottom, right;
  try {
    top = j.at("field-corners").at("top-left").at(1).get<int>();
    left = j.at("field-corners").at("top-left").at(0).get<int>();
    bottom = j.at("field-corners").at("bottom-right").at(1).get<int>();
    right = j.at("field-corners").at("bottom-right").at(0).get<int>();
  } catch (const wpi::json::exception& e) {
    wpi::print(stderr, "GUI: JSON: could not read field-corners: {}\n",
               e.what());
    return false;
  }

  // size
  float width;
  float height;
  try {
    width = j.at("field-size").at(0).get<float>();
    height = j.at("field-size").at(1).get<float>();
  } catch (const wpi::json::exception& e) {
    wpi::print(stderr, "GUI: JSON: could not read field-size: {}\n", e.what());
    return false;
  }

  // units for size
  std::string unit;
  try {
    unit = j.at("field-unit").get<std::string>();
  } catch (const wpi::json::exception& e) {
    wpi::print(stderr, "GUI: JSON: could not read field-unit: {}\n", e.what());
    return false;
  }

  // convert size units to meters
  if (unit == "foot" || unit == "feet") {
    width = units::convert<units::feet, units::meters>(width);
    height = units::convert<units::feet, units::meters>(height);
  }

  // check scaling
  int fieldWidth = m_right - m_left;
  int fieldHeight = m_bottom - m_top;
  if (std::abs((fieldWidth / width) - (fieldHeight / height)) > 0.3) {
    wpi::print(stderr,
               "GUI: Field X and Y scaling substantially different: "
               "xscale={} yscale={}\n",
               (fieldWidth / width), (fieldHeight / height));
  }

  if (!filename.empty()) {
    // the image filename is relative to the json file
    auto pathname = fs::path{filename}.replace_filename(image).string();

    // load field image
    if (!LoadImageImpl(pathname.c_str())) {
      return false;
    }
    m_filename = pathname;
  }

  // save to field info
  m_top = top;
  m_left = left;
  m_bottom = bottom;
  m_right = right;
  m_width = width;
  m_height = height;
  return true;
}

void FieldInfo::LoadJsonFile(std::string_view jsonfile) {
  auto fileBuffer = wpi::MemoryBuffer::GetFile(jsonfile);
  if (!fileBuffer) {
    std::fputs("GUI: could not open field JSON file\n", stderr);
    return;
  }
  LoadJson({reinterpret_cast<const char*>(fileBuffer.value()->begin()),
            fileBuffer.value()->size()},
           jsonfile);
}

bool FieldInfo::LoadImageImpl(const std::string& fn) {
  wpi::print("GUI: loading field image '{}'\n", fn);
  auto texture = gui::Texture::CreateFromFile(fn.c_str());
  if (!texture) {
    std::puts("GUI: could not read field image");
    return false;
  }
  m_texture = std::move(texture);
  m_imageWidth = m_texture.GetWidth();
  m_imageHeight = m_texture.GetHeight();
  m_filename = fn;
  return true;
}

FieldFrameData FieldInfo::GetFrameData(ImVec2 min, ImVec2 max) const {
  // fit the image into the window
  if (m_texture && m_imageHeight != 0 && m_imageWidth != 0) {
    gui::MaxFit(&min, &max, m_imageWidth, m_imageHeight);
  } else {
    gui::MaxFit(&min, &max, m_width, m_height);
  }

  FieldFrameData ffd;
  ffd.imageMin = min;
  ffd.imageMax = max;

  if (m_bottom > 0 && m_right > 0 && m_imageWidth != 0) {
    // size down the box by the image corners
    float scale = (max.x - min.x) / m_imageWidth;
    min.x += m_left * scale;
    min.y += m_top * scale;
    max.x -= (m_imageWidth - m_right) * scale;
    max.y -= (m_imageHeight - m_bottom) * scale;
  } else if ((max.x - min.x) > 40 && (max.y - min.y > 40)) {
    // scale padding to be proportional to aspect ratio
    float width = max.x - min.x;
    float height = max.y - min.y;
    float padX, padY;
    if (width > height) {
      padX = 20 * width / height;
      padY = 20;
    } else {
      padX = 20;
      padY = 20 * height / width;
    }

    // ensure there's some padding
    min.x += padX;
    max.x -= padX;
    min.y += padY;
    max.y -= padY;

    // also pad the image so it's the same size as the box
    ffd.imageMin.x += padX;
    ffd.imageMax.x -= padX;
    ffd.imageMin.y += padY;
    ffd.imageMax.y -= padY;
  }

  ffd.min = min;
  ffd.max = max;
  ffd.scale = (max.x - min.x) / m_width;
  return ffd;
}

void FieldInfo::Draw(ImDrawList* drawList, const FieldFrameData& ffd) const {
  if (m_texture && m_imageHeight != 0 && m_imageWidth != 0) {
    drawList->AddImage(m_texture, ffd.imageMin, ffd.imageMax);
  }

  // draw the field "active area" as a yellow boundary box
  drawList->AddRect(ffd.min, ffd.max, IM_COL32(255, 255, 0, 255));
}

ObjectInfo::ObjectInfo(Storage& storage)
    : m_width{storage.GetFloat("width",
                               DisplayOptions::kDefaultWidth.to<float>())},
      m_length{storage.GetFloat("length",
                                DisplayOptions::kDefaultLength.to<float>())},
      m_style{storage.GetString("style"),
              DisplayOptions::kDefaultStyle,
              {"Box/Image", "Line", "Line (Closed)", "Track", "Hidden"}},
      m_weight{storage.GetFloat("weight", DisplayOptions::kDefaultWeight)},
      m_color{
          storage.GetFloatArray("color", DisplayOptions::kDefaultColorFloat)},
      m_arrows{storage.GetBool("arrows", DisplayOptions::kDefaultArrows)},
      m_arrowSize{
          storage.GetInt("arrowSize", DisplayOptions::kDefaultArrowSize)},
      m_arrowWeight{
          storage.GetFloat("arrowWeight", DisplayOptions::kDefaultArrowWeight)},
      m_arrowColor{storage.GetFloatArray(
          "arrowColor", DisplayOptions::kDefaultArrowColorFloat)},
      m_selectable{
          storage.GetBool("selectable", DisplayOptions::kDefaultSelectable)},
      m_filename{storage.GetString("image")} {}

DisplayOptions ObjectInfo::GetDisplayOptions() const {
  DisplayOptions rv{m_texture};
  rv.style = static_cast<DisplayOptions::Style>(m_style.GetValue());
  rv.weight = m_weight;
  rv.color = ImGui::ColorConvertFloat4ToU32(m_color.GetColor());
  rv.width = units::meter_t{m_width};
  rv.length = units::meter_t{m_length};
  rv.arrows = m_arrows;
  rv.arrowSize = m_arrowSize;
  rv.arrowWeight = m_arrowWeight;
  rv.arrowColor = ImGui::ColorConvertFloat4ToU32(m_arrowColor.GetColor());
  rv.selectable = m_selectable;
  return rv;
}

void ObjectInfo::DisplaySettings() {
  ImGui::SetNextItemWidth(ImGui::GetFontSize() * 8);
  m_style.Combo("Style");
  switch (m_style.GetValue()) {
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
      InputFloatLength("Width", &m_width);
      InputFloatLength("Length", &m_length);
      break;
    case DisplayOptions::kTrack:
      InputFloatLength("Width", &m_width);
      break;
    default:
      break;
  }

  ImGui::InputFloat("Line Weight", &m_weight);
  m_color.ColorEdit3("Line Color", ImGuiColorEditFlags_NoInputs);
  ImGui::Checkbox("Arrows", &m_arrows);
  if (m_arrows) {
    ImGui::SliderInt("Arrow Size", &m_arrowSize, 0, 100, "%d%%",
                     ImGuiSliderFlags_AlwaysClamp);
    ImGui::InputFloat("Arrow Weight", &m_arrowWeight);
    m_arrowColor.ColorEdit3("Arrow Color", ImGuiColorEditFlags_NoInputs);
  }

  ImGui::Checkbox("Selectable", &m_selectable);
}

void ObjectInfo::DrawLine(ImDrawList* drawList,
                          std::span<const ImVec2> points) const {
  if (points.empty()) {
    return;
  }

  if (points.size() == 1) {
    drawList->AddCircleFilled(points.front(), m_weight, m_weight);
    return;
  }

  ImU32 color = ImGui::ColorConvertFloat4ToU32(m_color.GetColor());

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

    drawList->AddPolyline(&points[i], nlin, color, false, m_weight);
    i += nlin - 1;
  }

  if (points.size() > 2 && m_style.GetValue() == DisplayOptions::kLineClosed) {
    drawList->AddLine(points.back(), points.front(), color, m_weight);
  }
}

void ObjectInfo::Reset() {
  m_texture = gui::Texture{};
  m_filename.clear();
}

void ObjectInfo::LoadImage() {
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

bool ObjectInfo::LoadImageImpl(const std::string& fn) {
  wpi::print("GUI: loading object image '{}'\n", fn);
  auto texture = gui::Texture::CreateFromFile(fn.c_str());
  if (!texture) {
    std::fputs("GUI: could not read object image\n", stderr);
    return false;
  }
  m_texture = std::move(texture);
  m_filename = fn;
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

void PoseFrameData::SetPosition(const frc::Translation2d& pos) {
  m_pose = frc::Pose2d{pos, m_pose.Rotation()};
  m_model.SetPose(m_index, m_pose);
}

void PoseFrameData::SetRotation(units::radian_t rot) {
  m_pose = frc::Pose2d{m_pose.Translation(), rot};
  m_model.SetPose(m_index, m_pose);
}

void PoseFrameData::UpdateFrameData() {
  // (0,0) origin is bottom left
  ImVec2 center = m_ffd.GetScreenFromPos(m_pose.Translation());

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

SelectedTargetInfo PoseFrameData::GetDragTarget(int corner, float dist) const {
  SelectedTargetInfo info;
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
    SetPosition(m_ffd.GetPosFromScreen(cursor - gDragState.initialOffset));
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
    case DisplayOptions::kHidden:
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
    storage.SetData(std::make_shared<FieldInfo>(storage));
    field = storage.GetData<FieldInfo>();
  }

  EnumSetting displayUnits{GetStorage().GetString("units"),
                           kDisplayMeters,
                           {"meters", "feet", "inches"}};
  ImGui::SetNextItemWidth(ImGui::GetFontSize() * 8);
  displayUnits.Combo("Units");
  gDisplayUnits = static_cast<DisplayUnits>(displayUnits.GetValue());

  ImGui::PushItemWidth(ImGui::GetFontSize() * 4);
  if (ImGui::CollapsingHeader("Field")) {
    ImGui::PushID("Field");
    field->DisplaySettings();
    ImGui::PopID();
  }

  model->ForEachFieldObject([&](auto& objModel, auto name) {
    if (!objModel.Exists()) {
      return;
    }
    PushID(name);

    wpi::SmallString<64> nameBuf{name};
    if (ImGui::CollapsingHeader(nameBuf.c_str())) {
      auto& obj =
          field->m_objects.try_emplace(name, GetStorage()).first->second;
      obj.DisplaySettings();
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
  void DisplayObject(FieldObjectModel& model, std::string_view name);

  FieldInfo* m_field;
  ImVec2 m_mousePos;
  ImDrawList* m_drawList;

  // only allow initiation of dragging when invisible button is hovered;
  // this prevents the window resize handles from simultaneously activating
  // the drag functionality
  bool m_isHovered;

  FieldFrameData m_ffd;

  // drag targets
  std::vector<SelectedTargetInfo> m_targets;

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
  }

  // clear popup target if popup closed
  bool isPopupOpen = ImGui::IsPopupOpen("edit");
  if (!isPopupOpen) {
    gPopupState.Close();
  }

  // field objects
  m_targets.resize(0);
  model->ForEachFieldObject([this](auto& objModel, auto name) {
    if (objModel.Exists()) {
      DisplayObject(objModel, name);
    }
  });

  SelectedTargetInfo* target = nullptr;

  if (gDragState.target.objModel) {
    target = &gDragState.target;
  } else if (gPopupState.GetTarget()->objModel) {
    target = gPopupState.GetTarget();
  } else if (!m_targets.empty()) {
    // Find the "best" drag target of the available options.  Prefer
    // center to non-center, and then pick the closest hit.
    std::sort(m_targets.begin(), m_targets.end(),
              [](const auto& a, const auto& b) {
                return a.corner == 0 || a.dist < b.dist;
              });
    target = &m_targets.front();
  }

  if (target) {
    // draw the target circle; also draw a smaller circle on the pose center
    m_drawList->AddCircle(target->center, target->radius,
                          IM_COL32(0, 255, 0, 255));
    if (target->corner != 1) {
      m_drawList->AddCircle(target->poseCenter, target->radius / 2.0,
                            IM_COL32(0, 255, 0, 255));
    }
  }

  // right-click popup for editing
  if (m_isHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
    gPopupState.Open(target, m_ffd.GetPosFromScreen(m_mousePos));
    ImGui::OpenPopup("edit");
  }
  if (ImGui::BeginPopup("edit")) {
    gPopupState.Display(model, m_ffd);
    ImGui::EndPopup();
  } else if (target) {
    if (m_isHovered && ImGui::IsMouseClicked(0)) {
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

    // show tooltip and highlight
    auto pos = m_ffd.GetPosFromScreen(target->poseCenter);
    ImGui::SetTooltip(
        "%s[%d]\nx: %0.3f y: %0.3f rot: %0.3f", target->name.c_str(),
        static_cast<int>(target->index), ConvertDisplayLength(pos.X()),
        ConvertDisplayLength(pos.Y()), ConvertDisplayAngle(target->rot));
  }
}

void FieldDisplay::DisplayObject(FieldObjectModel& model,
                                 std::string_view name) {
  PushID(name);
  auto& obj = m_field->m_objects.try_emplace(name, GetStorage()).first->second;
  obj.LoadImage();

  auto displayOptions = obj.GetDisplayOptions();

  m_centerLine.resize(0);
  m_leftLine.resize(0);
  m_rightLine.resize(0);

  m_drawSplit.Split(m_drawList, 2);
  m_drawSplit.SetCurrentChannel(m_drawList, 1);
  auto poses = gPopupState.GetInsertModel() == &model
                   ? gPopupState.GetInsertPoses()
                   : model.GetPoses();
  size_t i = 0;
  for (auto&& pose : poses) {
    PoseFrameData pfd{pose, model, i, m_ffd, displayOptions};

    // check for potential drag targets
    if (displayOptions.selectable && m_isHovered &&
        !gDragState.target.objModel) {
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
  obj.DrawLine(m_drawList, m_centerLine);
  obj.DrawLine(m_drawList, m_leftLine);
  obj.DrawLine(m_drawList, m_rightLine);
  m_drawSplit.Merge(m_drawList);

  PopID();
}

void PopupState::Open(SelectedTargetInfo* target,
                      const frc::Translation2d& pos) {
  if (target) {
    m_target = *target;
  } else {
    m_target.objModel = nullptr;
    m_insertModel = nullptr;
    m_insertPoses.resize(0);
    m_insertPoses.emplace_back(pos, 0_deg);
    m_insertName.clear();
    m_insertIndex = 0;
  }
}

void PopupState::Close() {
  m_target.objModel = nullptr;
  m_insertModel = nullptr;
  m_insertPoses.resize(0);
}

void PopupState::Display(Field2DModel* model, const FieldFrameData& ffd) {
  if (m_target.objModel) {
    DisplayTarget(model, ffd);
  } else {
    DisplayInsert(model);
  }
}

void PopupState::DisplayTarget(Field2DModel* model, const FieldFrameData& ffd) {
  ImGui::Text("%s[%d]", m_target.name.c_str(),
              static_cast<int>(m_target.index));
  frc::Pose2d pose{ffd.GetPosFromScreen(m_target.poseCenter), m_target.rot};
  if (InputPose(&pose)) {
    m_target.poseCenter = ffd.GetScreenFromPos(pose.Translation());
    m_target.rot = pose.Rotation().Radians();
    m_target.objModel->SetPose(m_target.index, pose);
  }
  if (ImGui::Button("Delete Pose")) {
    auto posesRef = m_target.objModel->GetPoses();
    std::vector<frc::Pose2d> poses{posesRef.begin(), posesRef.end()};
    if (m_target.index < poses.size()) {
      poses.erase(poses.begin() + m_target.index);
      m_target.objModel->SetPoses(poses);
    }
    ImGui::CloseCurrentPopup();
  }
  if (ImGui::Button("Delete Object (ALL Poses)")) {
    model->RemoveFieldObject(m_target.name);
    ImGui::CloseCurrentPopup();
  }
}

void PopupState::DisplayInsert(Field2DModel* model) {
  ImGui::TextUnformatted("Insert New Pose");

  InputPose(&m_insertPoses[m_insertIndex]);

  const char* insertName = m_insertModel ? m_insertName.c_str() : "<new>";
  if (ImGui::BeginCombo("Object", insertName)) {
    bool selected = !m_insertModel;
    if (ImGui::Selectable("<new>", selected)) {
      m_insertModel = nullptr;
      auto pose = m_insertPoses[m_insertIndex];
      m_insertPoses.resize(0);
      m_insertPoses.emplace_back(std::move(pose));
      m_insertName.clear();
      m_insertIndex = 0;
    }
    if (selected) {
      ImGui::SetItemDefaultFocus();
    }
    model->ForEachFieldObject([&](auto& objModel, auto name) {
      bool selected = m_insertModel == &objModel;
      if (ImGui::Selectable(name.data(), selected)) {
        m_insertModel = &objModel;
        auto pose = m_insertPoses[m_insertIndex];
        auto posesRef = objModel.GetPoses();
        m_insertPoses.assign(posesRef.begin(), posesRef.end());
        m_insertPoses.emplace_back(std::move(pose));
        m_insertName = name;
        m_insertIndex = m_insertPoses.size() - 1;
      }
      if (selected) {
        ImGui::SetItemDefaultFocus();
      }
    });
    ImGui::EndCombo();
  }
  if (m_insertModel) {
    int oldIndex = m_insertIndex;
    if (ImGui::InputInt("Pos", &m_insertIndex, 1, 5)) {
      if (m_insertIndex < 0) {
        m_insertIndex = 0;
      }
      size_t size = m_insertPoses.size();
      if (static_cast<size_t>(m_insertIndex) >= size) {
        m_insertIndex = size - 1;
      }
      if (m_insertIndex < oldIndex) {
        auto begin = m_insertPoses.begin();
        std::rotate(begin + m_insertIndex, begin + oldIndex,
                    begin + oldIndex + 1);
      } else if (m_insertIndex > oldIndex) {
        auto rbegin = m_insertPoses.rbegin();
        std::rotate(rbegin + (size - m_insertIndex), rbegin + (size - oldIndex),
                    rbegin + (size - oldIndex - 1));
      }
    }
  } else {
    ImGui::InputText("Name", &m_insertName);
  }

  if (ImGui::Button("Apply")) {
    if (m_insertModel) {
      m_insertModel->SetPoses(m_insertPoses);
    } else if (!m_insertName.empty()) {
      model->AddFieldObject(m_insertName)->SetPoses(m_insertPoses);
    }
    ImGui::CloseCurrentPopup();
  }
  ImGui::SameLine();
  if (ImGui::Button("Cancel")) {
    ImGui::CloseCurrentPopup();
  }
}

void glass::DisplayField2D(Field2DModel* model, const ImVec2& contentSize) {
  auto& storage = GetStorage();
  auto field = storage.GetData<FieldInfo>();
  if (!field) {
    storage.SetData(std::make_shared<FieldInfo>(storage));
    field = storage.GetData<FieldInfo>();
  }

  if (contentSize.x <= 0 || contentSize.y <= 0) {
    return;
  }

  static FieldDisplay display;
  display.Display(field, model, contentSize);
}

void Field2DView::Display() {
  DisplayField2D(m_model, ImGui::GetWindowContentRegionMax() -
                              ImGui::GetWindowContentRegionMin());
}

void Field2DView::Settings() {
  DisplayField2DSettings(m_model);
}

bool Field2DView::HasSettings() {
  return true;
}
