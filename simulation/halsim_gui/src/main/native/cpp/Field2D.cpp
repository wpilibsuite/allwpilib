/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Field2D.h"

#include <cmath>

#include <hal/SimDevice.h>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <hal/simulation/SimDeviceData.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <units/angle.h>
#include <units/length.h>
#include <wpi/Path.h>
#include <wpi/SmallString.h>
#include <wpi/json.h>
#include <wpi/raw_istream.h>
#include <wpi/raw_ostream.h>
#include <wpigui.h>

#include "HALSimGui.h"
#include "SimDeviceGui.h"
#include "portable-file-dialogs.h"

using namespace halsimgui;

namespace gui = wpi::gui;

namespace {

// Per-frame field data (not persistent)
struct FieldFrameData {
  // in window coordinates
  ImVec2 imageMin;
  ImVec2 imageMax;
  ImVec2 min;
  ImVec2 max;

  float scale;  // scaling from field units to screen units
};

class FieldInfo {
 public:
  static constexpr float kDefaultWidth = 15.98f;
  static constexpr float kDefaultHeight = 8.21f;

  std::unique_ptr<pfd::open_file> m_fileOpener;
  float m_width = kDefaultWidth;
  float m_height = kDefaultHeight;

  void Reset();
  void LoadImage();
  void LoadJson(const wpi::Twine& jsonfile);
  FieldFrameData GetFrameData() const;
  void Draw(ImDrawList* drawList, const ImVec2& windowPos,
            const FieldFrameData& frameData) const;

  bool ReadIni(wpi::StringRef name, wpi::StringRef value);
  void WriteIni(ImGuiTextBuffer* out) const;

 private:
  bool LoadImageImpl(const char* fn);

  std::string m_filename;
  gui::Texture m_texture;
  int m_imageWidth = 0;
  int m_imageHeight = 0;
  int m_top = 0;
  int m_left = 0;
  int m_bottom = -1;
  int m_right = -1;
};

// Per-frame robot data (not persistent)
struct RobotFrameData {
  // in window coordinates
  ImVec2 center;
  ImVec2 corners[4];
  ImVec2 arrow[3];

  // scaled width/2 and length/2, in screen units
  float width2;
  float length2;
};

class RobotInfo {
 public:
  static constexpr float kDefaultWidth = 0.6858f;
  static constexpr float kDefaultLength = 0.8204f;

  std::unique_ptr<pfd::open_file> m_fileOpener;
  float m_width = kDefaultWidth;
  float m_length = kDefaultLength;

  void Reset();
  void LoadImage();
  void UpdateFromSimDevice();
  void SetPosition(double x, double y);
  // set and get rotation in radians
  void SetRotation(double rot);
  double GetRotation() const {
    return units::convert<units::degrees, units::radians>(m_rot);
  }
  RobotFrameData GetFrameData(const FieldFrameData& ffd) const;
  void Draw(ImDrawList* drawList, const ImVec2& windowPos,
            const RobotFrameData& frameData, int hit, float hitRadius) const;

  bool ReadIni(wpi::StringRef name, wpi::StringRef value);
  void WriteIni(ImGuiTextBuffer* out) const;

 private:
  bool LoadImageImpl(const char* fn);

  std::string m_filename;
  gui::Texture m_texture;

  HAL_SimDeviceHandle m_devHandle = 0;
  hal::SimDouble m_xHandle;
  hal::SimDouble m_yHandle;
  hal::SimDouble m_rotHandle;

  double m_x = 0;
  double m_y = 0;
  double m_rot = 0;
};

}  // namespace

static FieldInfo gField;
static RobotInfo gRobot;
static int gDragRobot = 0;
static ImVec2 gDragInitialOffset;
static double gDragInitialAngle;

// read/write settings to ini file
static void* Field2DReadOpen(ImGuiContext* ctx, ImGuiSettingsHandler* handler,
                             const char* name) {
  if (name == wpi::StringRef{"Field"}) return &gField;
  if (name == wpi::StringRef{"Robot"}) return &gRobot;
  return nullptr;
}

static void Field2DReadLine(ImGuiContext* ctx, ImGuiSettingsHandler* handler,
                            void* entry, const char* lineStr) {
  wpi::StringRef line{lineStr};
  auto [name, value] = line.split('=');
  name = name.trim();
  value = value.trim();
  if (entry == &gField)
    gField.ReadIni(name, value);
  else if (entry == &gRobot)
    gRobot.ReadIni(name, value);
}

static void Field2DWriteAll(ImGuiContext* ctx, ImGuiSettingsHandler* handler,
                            ImGuiTextBuffer* out_buf) {
  gField.WriteIni(out_buf);
  gRobot.WriteIni(out_buf);
}

void FieldInfo::Reset() {
  m_texture = gui::Texture{};
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
      if (wpi::StringRef(result[0]).endswith(".json")) {
        LoadJson(result[0]);
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
  if (!m_texture && !m_filename.empty()) {
    if (!LoadImageImpl(m_filename.c_str())) m_filename.clear();
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
  if (!LoadImageImpl(pathname.c_str())) return;

  // save to field info
  m_filename = pathname.str();
  m_top = top;
  m_left = left;
  m_bottom = bottom;
  m_right = right;
  m_width = width;
  m_height = height;
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
  m_filename = fn;
  return true;
}

FieldFrameData FieldInfo::GetFrameData() const {
  FieldFrameData ffd;

  // get window content region
  ffd.imageMin = ImGui::GetWindowContentRegionMin();
  ffd.imageMax = ImGui::GetWindowContentRegionMax();

  // fit the image into the window
  if (m_texture && m_imageHeight != 0 && m_imageWidth != 0)
    gui::MaxFit(&ffd.imageMin, &ffd.imageMax, m_imageWidth, m_imageHeight);

  ImVec2 min = ffd.imageMin;
  ImVec2 max = ffd.imageMax;

  // size down the box by the image corners (if any)
  if (m_bottom > 0 && m_right > 0) {
    min.x += m_left * (max.x - min.x) / m_imageWidth;
    min.y += m_top * (max.y - min.y) / m_imageHeight;
    max.x -= (m_imageWidth - m_right) * (max.x - min.x) / m_imageWidth;
    max.y -= (m_imageHeight - m_bottom) * (max.y - min.y) / m_imageHeight;
  }

  // draw the field "active area" as a yellow boundary box
  gui::MaxFit(&min, &max, m_width, m_height);

  ffd.min = min;
  ffd.max = max;
  ffd.scale = (max.x - min.x) / m_width;
  return ffd;
}

void FieldInfo::Draw(ImDrawList* drawList, const ImVec2& windowPos,
                     const FieldFrameData& ffd) const {
  if (m_texture && m_imageHeight != 0 && m_imageWidth != 0) {
    drawList->AddImage(m_texture, windowPos + ffd.imageMin,
                       windowPos + ffd.imageMax);
  }

  // draw the field "active area" as a yellow boundary box
  drawList->AddRect(windowPos + ffd.min, windowPos + ffd.max,
                    IM_COL32(255, 255, 0, 255));
}

bool FieldInfo::ReadIni(wpi::StringRef name, wpi::StringRef value) {
  if (name == "image") {
    m_filename = value;
  } else if (name == "top") {
    int num;
    if (value.getAsInteger(10, num)) return true;
    m_top = num;
  } else if (name == "left") {
    int num;
    if (value.getAsInteger(10, num)) return true;
    m_left = num;
  } else if (name == "bottom") {
    int num;
    if (value.getAsInteger(10, num)) return true;
    m_bottom = num;
  } else if (name == "right") {
    int num;
    if (value.getAsInteger(10, num)) return true;
    m_right = num;
  } else if (name == "width") {
    std::sscanf(value.data(), "%f", &m_width);
  } else if (name == "height") {
    std::sscanf(value.data(), "%f", &m_height);
  } else {
    return false;
  }
  return true;
}

void FieldInfo::WriteIni(ImGuiTextBuffer* out) const {
  out->appendf(
      "[Field2D][Field]\nimage=%s\ntop=%d\nleft=%d\nbottom=%d\nright=%d\nwidth="
      "%f\nheight=%f\n\n",
      m_filename.c_str(), m_top, m_left, m_bottom, m_right, m_width, m_height);
}

void RobotInfo::Reset() {
  m_texture = gui::Texture{};
  m_filename.clear();
}

void RobotInfo::LoadImage() {
  if (m_fileOpener && m_fileOpener->ready(0)) {
    auto result = m_fileOpener->result();
    if (!result.empty()) LoadImageImpl(result[0].c_str());
    m_fileOpener.reset();
  }
  if (!m_texture && !m_filename.empty()) {
    if (!LoadImageImpl(m_filename.c_str())) m_filename.clear();
  }
}

bool RobotInfo::LoadImageImpl(const char* fn) {
  wpi::outs() << "GUI: loading robot image '" << fn << "'\n";
  auto texture = gui::Texture::CreateFromFile(fn);
  if (!texture) {
    wpi::errs() << "GUI: could not read robot image\n";
    return false;
  }
  m_texture = std::move(texture);
  m_filename = fn;
  return true;
}

void RobotInfo::UpdateFromSimDevice() {
  if (m_devHandle == 0) m_devHandle = HALSIM_GetSimDeviceHandle("Field2D");
  if (m_devHandle == 0) return;

  if (!m_xHandle) m_xHandle = HALSIM_GetSimValueHandle(m_devHandle, "x");
  if (m_xHandle) m_x = m_xHandle.Get();

  if (!m_yHandle) m_yHandle = HALSIM_GetSimValueHandle(m_devHandle, "y");
  if (m_yHandle) m_y = m_yHandle.Get();

  if (!m_rotHandle) m_rotHandle = HALSIM_GetSimValueHandle(m_devHandle, "rot");
  if (m_rotHandle) m_rot = m_rotHandle.Get();
}

void RobotInfo::SetPosition(double x, double y) {
  m_x = x;
  m_y = y;
  if (m_xHandle) m_xHandle.Set(x);
  if (m_yHandle) m_yHandle.Set(y);
}

void RobotInfo::SetRotation(double rot) {
  double rotDegrees = units::convert<units::radians, units::degrees>(rot);
  // force to -180 to +180 range
  rotDegrees = rotDegrees + std::ceil((-rotDegrees - 180) / 360) * 360;
  m_rot = rotDegrees;
  if (m_rotHandle) m_rotHandle.Set(rotDegrees);
}

RobotFrameData RobotInfo::GetFrameData(const FieldFrameData& ffd) const {
  RobotFrameData rfd;
  float width2 = ffd.scale * m_width / 2;
  float length2 = ffd.scale * m_length / 2;

  // (0,0) origin is bottom left
  ImVec2 center(ffd.min.x + ffd.scale * m_x, ffd.max.y - ffd.scale * m_y);

  // build rotated points around center
  double rot = GetRotation();
  float cos_a = std::cos(-rot);
  float sin_a = std::sin(-rot);

  rfd.corners[0] = center + ImRotate(ImVec2(-length2, -width2), cos_a, sin_a);
  rfd.corners[1] = center + ImRotate(ImVec2(length2, -width2), cos_a, sin_a);
  rfd.corners[2] = center + ImRotate(ImVec2(length2, width2), cos_a, sin_a);
  rfd.corners[3] = center + ImRotate(ImVec2(-length2, width2), cos_a, sin_a);
  rfd.arrow[0] =
      center + ImRotate(ImVec2(-length2 / 2, -width2 / 2), cos_a, sin_a);
  rfd.arrow[1] = center + ImRotate(ImVec2(length2 / 2, 0), cos_a, sin_a);
  rfd.arrow[2] =
      center + ImRotate(ImVec2(-length2 / 2, width2 / 2), cos_a, sin_a);

  rfd.center = center;
  rfd.width2 = width2;
  rfd.length2 = length2;
  return rfd;
}

void RobotInfo::Draw(ImDrawList* drawList, const ImVec2& windowPos,
                     const RobotFrameData& rfd, int hit,
                     float hitRadius) const {
  if (m_texture) {
    drawList->AddImageQuad(
        m_texture, windowPos + rfd.corners[0], windowPos + rfd.corners[1],
        windowPos + rfd.corners[2], windowPos + rfd.corners[3]);
  } else {
    drawList->AddQuad(windowPos + rfd.corners[0], windowPos + rfd.corners[1],
                      windowPos + rfd.corners[2], windowPos + rfd.corners[3],
                      IM_COL32(255, 0, 0, 255), 4.0);
    drawList->AddTriangle(windowPos + rfd.arrow[0], windowPos + rfd.arrow[1],
                          windowPos + rfd.arrow[2], IM_COL32(0, 255, 0, 255),
                          4.0);
  }

  if (hit > 0) {
    if (hit == 1) {
      drawList->AddCircle(windowPos + rfd.center, hitRadius,
                          IM_COL32(0, 255, 0, 255));
    } else {
      drawList->AddCircle(windowPos + rfd.corners[hit - 2], hitRadius,
                          IM_COL32(0, 255, 0, 255));
    }
  }
}

bool RobotInfo::ReadIni(wpi::StringRef name, wpi::StringRef value) {
  if (name == "image") {
    m_filename = value;
  } else if (name == "width") {
    std::sscanf(value.data(), "%f", &m_width);
  } else if (name == "length") {
    std::sscanf(value.data(), "%f", &m_length);
  } else {
    return false;
  }
  return true;
}

void RobotInfo::WriteIni(ImGuiTextBuffer* out) const {
  out->appendf("[Field2D][Robot]\nimage=%s\nwidth=%f\nlength=%f\n\n",
               m_filename.c_str(), m_width, m_length);
}

static void OptionMenuField2D() {
  if (ImGui::BeginMenu("2D Field View")) {
    if (ImGui::MenuItem("Choose field image...")) {
      gField.m_fileOpener = std::make_unique<pfd::open_file>(
          "Choose field image", "",
          std::vector<std::string>{"Image File",
                                   "*.jpg *.jpeg *.png *.bmp *.psd *.tga *.gif "
                                   "*.hdr *.pic *.ppm *.pgm",
                                   "PathWeaver JSON File", "*.json"});
    }
    if (ImGui::MenuItem("Reset field image")) {
      gField.Reset();
    }
    if (ImGui::MenuItem("Choose robot image...")) {
      gRobot.m_fileOpener = std::make_unique<pfd::open_file>(
          "Choose robot image", "",
          std::vector<std::string>{"Image File",
                                   "*.jpg *.jpeg *.png *.bmp *.psd *.tga *.gif "
                                   "*.hdr *.pic *.ppm *.pgm"});
    }
    if (ImGui::MenuItem("Reset robot image")) {
      gRobot.Reset();
    }
    ImGui::EndMenu();
  }
}

static void DisplayField2DSettings() {
  ImGui::PushItemWidth(ImGui::GetFontSize() * 4);
  ImGui::InputFloat("Field Width", &gField.m_width);
  ImGui::InputFloat("Field Height", &gField.m_height);
  // ImGui::InputInt("Field Top", &gField.m_top);
  // ImGui::InputInt("Field Left", &gField.m_left);
  // ImGui::InputInt("Field Right", &gField.m_right);
  // ImGui::InputInt("Field Bottom", &gField.m_bottom);
  ImGui::InputFloat("Robot Width", &gRobot.m_width);
  ImGui::InputFloat("Robot Length", &gRobot.m_length);
  ImGui::PopItemWidth();
}

static void DisplayField2D() {
  // load images
  gField.LoadImage();
  gRobot.LoadImage();

  // get robot coordinates from SimDevice
  gRobot.UpdateFromSimDevice();

  FieldFrameData ffd = gField.GetFrameData();
  RobotFrameData rfd = gRobot.GetFrameData(ffd);

  ImVec2 windowPos = ImGui::GetWindowPos();

  // for dragging to work, there needs to be a button (otherwise the window is
  // dragged)
  ImVec2 contentSize =
      ImGui::GetWindowContentRegionMax() - ImGui::GetWindowContentRegionMin();
  if (contentSize.x <= 0 || contentSize.y <= 0) return;
  ImGui::InvisibleButton("field", contentSize);

  // allow dragging the robot around
  ImVec2 cursor = ImGui::GetIO().MousePos - windowPos;

  int hit = 0;
  float hitRadius = (std::min)(rfd.width2, rfd.length2) / 2;
  // only allow initiation of dragging when invisible button is hovered; this
  // prevents the window resize handles from simultaneously activating the drag
  // functionality
  if (ImGui::IsItemHovered()) {
    float hitRadiusSquared = hitRadius * hitRadius;
    // it's within the hit radius of the center?
    if (gui::GetDistSquared(cursor, rfd.center) < hitRadiusSquared)
      hit = 1;
    else if (gui::GetDistSquared(cursor, rfd.corners[0]) < hitRadiusSquared)
      hit = 2;
    else if (gui::GetDistSquared(cursor, rfd.corners[1]) < hitRadiusSquared)
      hit = 3;
    else if (gui::GetDistSquared(cursor, rfd.corners[2]) < hitRadiusSquared)
      hit = 4;
    else if (gui::GetDistSquared(cursor, rfd.corners[3]) < hitRadiusSquared)
      hit = 5;
    if (hit > 0 && ImGui::IsMouseClicked(0)) {
      if (hit == 1) {
        gDragRobot = hit;
        gDragInitialOffset = cursor - rfd.center;
      } else {
        gDragRobot = hit;
        ImVec2 off = cursor - rfd.center;
        gDragInitialAngle = std::atan2(off.y, off.x) + gRobot.GetRotation();
      }
    }
  }

  if (gDragRobot > 0 && ImGui::IsMouseDown(0)) {
    if (gDragRobot == 1) {
      ImVec2 newPos = cursor - gDragInitialOffset;
      gRobot.SetPosition(
          (std::clamp(newPos.x, ffd.min.x, ffd.max.x) - ffd.min.x) / ffd.scale,
          (ffd.max.y - std::clamp(newPos.y, ffd.min.y, ffd.max.y)) / ffd.scale);
      rfd = gRobot.GetFrameData(ffd);
    } else {
      ImVec2 off = cursor - rfd.center;
      gRobot.SetRotation(gDragInitialAngle - std::atan2(off.y, off.x));
    }
    hit = gDragRobot;  // keep it highlighted
  } else {
    gDragRobot = 0;
  }

  // draw
  auto drawList = ImGui::GetWindowDrawList();
  gField.Draw(drawList, windowPos, ffd);
  gRobot.Draw(drawList, windowPos, rfd, hit, hitRadius);
}

void Field2D::Initialize() {
  // hook ini handler to save settings
  ImGuiSettingsHandler iniHandler;
  iniHandler.TypeName = "Field2D";
  iniHandler.TypeHash = ImHashStr(iniHandler.TypeName);
  iniHandler.ReadOpenFn = Field2DReadOpen;
  iniHandler.ReadLineFn = Field2DReadLine;
  iniHandler.WriteAllFn = Field2DWriteAll;
  ImGui::GetCurrentContext()->SettingsHandlers.push_back(iniHandler);

  HALSimGui::AddOptionMenu(OptionMenuField2D);

  HALSimGui::AddWindow("2D Field Settings", DisplayField2DSettings,
                       ImGuiWindowFlags_AlwaysAutoResize);
  HALSimGui::SetWindowVisibility("2D Field Settings", HALSimGui::kHide);
  HALSimGui::SetDefaultWindowPos("2D Field Settings", 200, 150);

  HALSimGui::AddWindow("2D Field View", DisplayField2D);
  HALSimGui::SetWindowVisibility("2D Field View", HALSimGui::kHide);
  HALSimGui::SetDefaultWindowPos("2D Field View", 200, 200);
  HALSimGui::SetDefaultWindowSize("2D Field View", 400, 200);
  HALSimGui::SetWindowPadding("2D Field View", 0, 0);

  // SimDeviceGui::Hide("Field2D");
}
