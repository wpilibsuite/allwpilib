/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "glass/other/Field2D.h"

#include <cmath>
#include <memory>

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
#include "glass/DataSource.h"

using namespace glass;

namespace gui = wpi::gui;

namespace {

// Per-frame field data (not persistent)
struct FieldFrameData {
  // in screen coordinates
  ImVec2 imageMin;
  ImVec2 imageMax;
  ImVec2 min;
  ImVec2 max;

  float scale;  // scaling from field units to screen units
};

// Object drag state
struct ObjectDragState {
  int object = 0;
  int corner = 0;
  ImVec2 initialOffset;
  double initialAngle = 0;
};

// Per-frame object data (not persistent)
class ObjectFrameData {
 public:
  explicit ObjectFrameData(FieldObjectModel& model, const FieldFrameData& ffd,
                           float width, float length);
  void SetPosition(double x, double y);
  // set and get rotation in radians
  void SetRotation(double rot);
  double GetRotation() const {
    return units::convert<units::degrees, units::radians>(m_rot);
  }
  void UpdateFrameData();
  int IsHovered(const ImVec2& cursor) const;
  bool HandleDrag(const ImVec2& cursor, int hitCorner, ObjectDragState* drag);
  void Draw(ImDrawList* drawList, const gui::Texture& texture,
            int hitCorner) const;

  // in window coordinates
  ImVec2 m_center;
  ImVec2 m_corners[4];
  ImVec2 m_arrow[3];

 private:
  FieldObjectModel& m_model;
  const FieldFrameData& m_ffd;

  // scaled width/2 and length/2, in screen units
  float m_width2;
  float m_length2;

  float m_hitRadius;

  double m_x = 0;
  double m_y = 0;
  double m_rot = 0;
};

class ObjectGroupInfo {
 public:
  static constexpr float kDefaultWidth = 0.6858f;
  static constexpr float kDefaultLength = 0.8204f;

  ObjectGroupInfo();

  std::unique_ptr<pfd::open_file> m_fileOpener;
  float* m_pWidth;
  float* m_pLength;
  ObjectDragState m_dragState;

  void Reset();
  void LoadImage();
  const gui::Texture& GetTexture() const { return m_texture; }

 private:
  bool LoadImageImpl(const char* fn);

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

  wpi::StringMap<std::unique_ptr<ObjectGroupInfo>> m_objectGroups;

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
    if (!LoadImageImpl(m_pFilename->c_str())) m_pFilename->clear();
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
  if (m_texture && m_imageHeight != 0 && m_imageWidth != 0)
    gui::MaxFit(&min, &max, m_imageWidth, m_imageHeight);

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

ObjectGroupInfo::ObjectGroupInfo() {
  auto& storage = GetStorage();
  m_pFilename = storage.GetStringRef("image");
  m_pWidth = storage.GetFloatRef("width", kDefaultWidth);
  m_pLength = storage.GetFloatRef("length", kDefaultLength);
}

void ObjectGroupInfo::Reset() {
  m_texture = gui::Texture{};
  m_pFilename->clear();
}

void ObjectGroupInfo::LoadImage() {
  if (m_fileOpener && m_fileOpener->ready(0)) {
    auto result = m_fileOpener->result();
    if (!result.empty()) LoadImageImpl(result[0].c_str());
    m_fileOpener.reset();
  }
  if (!m_texture && !m_pFilename->empty()) {
    if (!LoadImageImpl(m_pFilename->c_str())) m_pFilename->clear();
  }
}

bool ObjectGroupInfo::LoadImageImpl(const char* fn) {
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

ObjectFrameData::ObjectFrameData(FieldObjectModel& model,
                                 const FieldFrameData& ffd, float width,
                                 float length)
    : m_model{model},
      m_ffd{ffd},
      m_width2(ffd.scale * width / 2),
      m_length2(ffd.scale * length / 2),
      m_hitRadius((std::min)(m_width2, m_length2) / 2) {
  if (auto xData = model.GetXData()) m_x = xData->GetValue();
  if (auto yData = model.GetYData()) m_y = yData->GetValue();
  if (auto rotationData = model.GetRotationData())
    m_rot = rotationData->GetValue();
  UpdateFrameData();
}

void ObjectFrameData::SetPosition(double x, double y) {
  m_x = x;
  m_y = y;
  m_model.SetPosition(x, y);
}

void ObjectFrameData::SetRotation(double rot) {
  double rotDegrees = units::convert<units::radians, units::degrees>(rot);
  // force to -180 to +180 range
  rotDegrees = rotDegrees + std::ceil((-rotDegrees - 180) / 360) * 360;
  m_rot = rotDegrees;
  m_model.SetRotation(rotDegrees);
}

void ObjectFrameData::UpdateFrameData() {
  // (0,0) origin is bottom left
  ImVec2 center(m_ffd.min.x + m_ffd.scale * m_x,
                m_ffd.max.y - m_ffd.scale * m_y);

  // build rotated points around center
  float length2 = m_length2;
  float width2 = m_width2;
  double rot = GetRotation();
  float cos_a = std::cos(-rot);
  float sin_a = std::sin(-rot);

  m_corners[0] = center + ImRotate(ImVec2(-length2, -width2), cos_a, sin_a);
  m_corners[1] = center + ImRotate(ImVec2(length2, -width2), cos_a, sin_a);
  m_corners[2] = center + ImRotate(ImVec2(length2, width2), cos_a, sin_a);
  m_corners[3] = center + ImRotate(ImVec2(-length2, width2), cos_a, sin_a);
  m_arrow[0] =
      center + ImRotate(ImVec2(-length2 / 2, -width2 / 2), cos_a, sin_a);
  m_arrow[1] = center + ImRotate(ImVec2(length2 / 2, 0), cos_a, sin_a);
  m_arrow[2] =
      center + ImRotate(ImVec2(-length2 / 2, width2 / 2), cos_a, sin_a);

  m_center = center;
}

int ObjectFrameData::IsHovered(const ImVec2& cursor) const {
  // only allow initiation of dragging when invisible button is hovered;
  // this prevents the window resize handles from simultaneously activating
  // the drag functionality
  if (!ImGui::IsItemHovered()) return 0;

  float hitRadiusSquared = m_hitRadius * m_hitRadius;
  // it's within the hit radius of the center?
  if (gui::GetDistSquared(cursor, m_center) < hitRadiusSquared)
    return 1;
  else if (gui::GetDistSquared(cursor, m_corners[0]) < hitRadiusSquared)
    return 2;
  else if (gui::GetDistSquared(cursor, m_corners[1]) < hitRadiusSquared)
    return 3;
  else if (gui::GetDistSquared(cursor, m_corners[2]) < hitRadiusSquared)
    return 4;
  else if (gui::GetDistSquared(cursor, m_corners[3]) < hitRadiusSquared)
    return 5;
  else
    return 0;
}

bool ObjectFrameData::HandleDrag(const ImVec2& cursor, int hitCorner,
                                 ObjectDragState* drag) {
  bool rv = false;
  if (hitCorner > 0 && ImGui::IsMouseClicked(0)) {
    if (hitCorner == 1) {
      drag->corner = hitCorner;
      drag->initialOffset = cursor - m_center;
    } else {
      drag->corner = hitCorner;
      ImVec2 off = cursor - m_center;
      drag->initialAngle = std::atan2(off.y, off.x) + GetRotation();
    }
    rv = true;
  }

  if (drag->corner > 0 && ImGui::IsMouseDown(0)) {
    if (drag->corner == 1) {
      ImVec2 newPos = cursor - drag->initialOffset;
      SetPosition(
          (std::clamp(newPos.x, m_ffd.min.x, m_ffd.max.x) - m_ffd.min.x) /
              m_ffd.scale,
          (m_ffd.max.y - std::clamp(newPos.y, m_ffd.min.y, m_ffd.max.y)) /
              m_ffd.scale);
      UpdateFrameData();
    } else {
      ImVec2 off = cursor - m_center;
      SetRotation(drag->initialAngle - std::atan2(off.y, off.x));
    }
  } else {
    drag->corner = 0;
  }

  return rv;
}

void ObjectFrameData::Draw(ImDrawList* drawList, const gui::Texture& texture,
                           int hitCorner) const {
  if (texture) {
    drawList->AddImageQuad(texture, m_corners[0], m_corners[1], m_corners[2],
                           m_corners[3]);
  } else {
    drawList->AddQuad(m_corners[0], m_corners[1], m_corners[2], m_corners[3],
                      IM_COL32(255, 0, 0, 255), 4.0);
    drawList->AddTriangle(m_arrow[0], m_arrow[1], m_arrow[2],
                          IM_COL32(0, 255, 0, 255), 4.0);
  }

  if (hitCorner > 0) {
    if (hitCorner == 1) {
      drawList->AddCircle(m_center, m_hitRadius, IM_COL32(0, 255, 0, 255));
    } else {
      drawList->AddCircle(m_corners[hitCorner - 2], m_hitRadius,
                          IM_COL32(0, 255, 0, 255));
    }
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

  model->ForEachFieldObjectGroup([&](auto& groupModel, auto name) {
    if (!groupModel.Exists()) return;
    PushID(name);
    auto& objGroupRef = field->m_objectGroups[name];
    if (!objGroupRef) objGroupRef = std::make_unique<ObjectGroupInfo>();
    auto objGroup = objGroupRef.get();

    wpi::SmallString<64> nameBuf = name;
    if (ImGui::CollapsingHeader(nameBuf.c_str())) {
      if (ImGui::Button("Choose image...")) {
        objGroup->m_fileOpener = std::make_unique<pfd::open_file>(
            "Choose object image", "",
            std::vector<std::string>{
                "Image File",
                "*.jpg *.jpeg *.png *.bmp *.psd *.tga *.gif "
                "*.hdr *.pic *.ppm *.pgm"});
      }
      if (ImGui::Button("Reset image")) {
        objGroup->Reset();
      }
      ImGui::InputFloat("Width", objGroup->m_pWidth);
      ImGui::InputFloat("Length", objGroup->m_pLength);
    }
    PopID();
  });
  ImGui::PopItemWidth();
}

void glass::DisplayField2D(Field2DModel* model, const ImVec2& contentSize) {
  auto& storage = GetStorage();
  auto field = storage.GetData<FieldInfo>();
  if (!field) {
    storage.SetData(std::make_shared<FieldInfo>());
    field = storage.GetData<FieldInfo>();
  }

  ImVec2 windowPos = ImGui::GetWindowPos();
  ImVec2 mousePos = ImGui::GetIO().MousePos;

  // for dragging to work, there needs to be a button (otherwise the window is
  // dragged)
  if (contentSize.x <= 0 || contentSize.y <= 0) return;
  ImVec2 cursorPos = windowPos + ImGui::GetCursorPos();  // screen coords
  ImGui::InvisibleButton("field", contentSize);

  // field
  field->LoadImage();
  FieldFrameData ffd = field->GetFrameData(cursorPos, cursorPos + contentSize);
  auto drawList = ImGui::GetWindowDrawList();
  field->Draw(drawList, ffd);

  model->ForEachFieldObjectGroup([&](auto& groupModel, auto name) {
    if (!groupModel.Exists()) return;
    PushID(name);
    auto& objGroupRef = field->m_objectGroups[name];
    if (!objGroupRef) objGroupRef = std::make_unique<ObjectGroupInfo>();
    auto objGroup = objGroupRef.get();
    objGroup->LoadImage();

    int i = 0;
    groupModel.ForEachFieldObject([&](auto& objModel) {
      ++i;
      ObjectFrameData ofd{objModel, ffd, *objGroup->m_pWidth,
                          *objGroup->m_pLength};

      int hitCorner = 0;
      if (objGroup->m_dragState.object == 0 ||
          objGroup->m_dragState.object == i) {
        hitCorner = ofd.IsHovered(mousePos);
        if (ofd.HandleDrag(mousePos, hitCorner, &objGroup->m_dragState))
          objGroup->m_dragState.object = i;
      }

      // draw
      ofd.Draw(drawList, objGroup->GetTexture(), hitCorner);
    });
    PopID();
  });
}

void Field2DView::Display() {
  if (ImGui::BeginPopupContextItem()) {
    DisplayField2DSettings(m_model);
    ImGui::EndPopup();
  }
  DisplayField2D(m_model, ImGui::GetWindowContentRegionMax() -
                              ImGui::GetWindowContentRegionMin());
}
