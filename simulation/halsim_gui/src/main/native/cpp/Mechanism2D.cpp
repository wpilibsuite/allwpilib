/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Mechanism2D.h"

#include <cmath>
#include <string>

#include <hal/SimDevice.h>
#include <hal/simulation/SimDeviceData.h>
#include <imgui.h>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>
#include <portable-file-dialogs.h>
#include <wpi/json.h>
#include <wpi/math>
#include <wpi/raw_istream.h>
#include <wpi/raw_ostream.h>

#include "HALSimGui.h"

using namespace halsimgui;

static HAL_SimDeviceHandle devHandle = 0;
static wpi::StringMap<ImColor> colorLookUpTable;
static std::unique_ptr<pfd::open_file> m_fileOpener;
static std::string previousJsonLocation = "Not empty";
namespace {
struct BodyConfig {
  std::string name;
  std::string type = "line";
  int length = 100;
  std::string color = "green";
  int angle = 0;
  std::vector<BodyConfig> children;
  int lineWidth = 1;
};
}  // namespace
static std::vector<BodyConfig> bodyConfigVector;
namespace {
struct DrawLineStruct {
  float xEnd;
  float yEnd;
  float angle;
};
}  // namespace
static struct NamedColor {
  const char* name;
  ImColor value;
} staticColors[] = {{"white", IM_COL32(255, 255, 255, 255)},
                    {"silver", IM_COL32(192, 192, 192, 255)},
                    {"gray", IM_COL32(128, 128, 128, 255)},
                    {"black", IM_COL32(0, 0, 0, 255)},
                    {"red", IM_COL32(255, 0, 0, 255)},
                    {"maroon", IM_COL32(128, 0, 0, 255)},
                    {"yellow", IM_COL32(255, 255, 0, 255)},
                    {"olive", IM_COL32(128, 128, 0, 255)},
                    {"lime", IM_COL32(0, 255, 0, 255)},
                    {"green", IM_COL32(0, 128, 0, 255)},
                    {"aqua", IM_COL32(0, 255, 255, 255)},
                    {"teal", IM_COL32(0, 128, 128, 255)},
                    {"blue", IM_COL32(0, 0, 255, 255)},
                    {"navy", IM_COL32(0, 0, 128, 255)},
                    {"fuchsia", IM_COL32(255, 0, 255, 255)},
                    {"purple", IM_COL32(128, 0, 128, 255)}};

static void buildColorTable() {
  for (auto&& namedColor : staticColors) {
    colorLookUpTable.try_emplace(namedColor.name, namedColor.value);
  }
}
namespace {
class Mechanism2DInfo {
 public:
  std::string jsonLocation;
};
}  // namespace

static Mechanism2DInfo mechanism2DInfo;

bool ReadIni(wpi::StringRef name, wpi::StringRef value) {
  if (name == "jsonLocation") {
    mechanism2DInfo.jsonLocation = value;
  } else {
    return false;
  }
  return true;
}

void WriteIni(ImGuiTextBuffer* out) {
  out->appendf("[Mechanism2D][Mechanism2D]\njsonLocation=%s\n\n",
               mechanism2DInfo.jsonLocation.c_str());
}

// read/write settings to ini file
static void* Mechanism2DReadOpen(ImGuiContext* ctx,
                                 ImGuiSettingsHandler* handler,
                                 const char* name) {
  if (name == wpi::StringRef{"Mechanism2D"}) return &mechanism2DInfo;
  return nullptr;
}

static void Mechanism2DReadLine(ImGuiContext* ctx,
                                ImGuiSettingsHandler* handler, void* entry,
                                const char* lineStr) {
  wpi::StringRef line{lineStr};
  auto [name, value] = line.split('=');
  name = name.trim();
  value = value.trim();
  if (entry == &mechanism2DInfo) ReadIni(name, value);
}

static void Mechanism2DWriteAll(ImGuiContext* ctx,
                                ImGuiSettingsHandler* handler,
                                ImGuiTextBuffer* out_buf) {
  WriteIni(out_buf);
}

static void GetJsonFileLocation() {
  if (m_fileOpener && m_fileOpener->ready(0)) {
    auto result = m_fileOpener->result();
    if (!result.empty()) {
      mechanism2DInfo.jsonLocation = result[0];
    } else {
      wpi::errs() << "Can not find json file!!!";
    }
  }
}

DrawLineStruct DrawLine(float startXLocation, float startYLocation, int length,
                        float angle, ImDrawList* drawList, ImVec2 windowPos,
                        ImColor color, const BodyConfig& bodyConfig,
                        const std::string& previousPath) {
  DrawLineStruct drawLineStruct;
  drawLineStruct.angle = angle;
  // Find the current path do the ligament
  std::string currentPath = previousPath + bodyConfig.name;
  // Find the angle in radians
  double radAngle = (drawLineStruct.angle - 90) * wpi::math::pi / 180;
  // Get the start X and Y location
  drawLineStruct.xEnd = startXLocation + length * std::cos(radAngle);
  drawLineStruct.yEnd = startYLocation + length * std::sin(radAngle);
  // Add the line to the drawList
  drawList->AddLine(
      windowPos + ImVec2(startXLocation, startYLocation),
      windowPos + ImVec2(drawLineStruct.xEnd, drawLineStruct.yEnd), color,
      bodyConfig.lineWidth);
  // Return the end X, Y, and angle
  return drawLineStruct;
}

static void buildDrawList(float startXLocation, float startYLocation,
                          ImDrawList* drawList, float previousAngle,
                          const std::vector<BodyConfig>& subBodyConfigs,
                          ImVec2 windowPos) {
  for (BodyConfig const& bodyConfig : subBodyConfigs) {
    hal::SimDouble angleHandle;
    hal::SimDouble lengthHandle;
    float angle = 0;
    float length = 0;
    // Get the smallest of width or height
    double minSize;
    // Find the min size of the window
    minSize = ImGui::GetWindowHeight() > ImGui::GetWindowWidth()
                  ? ImGui::GetWindowWidth()
                  : ImGui::GetWindowHeight();
    if (devHandle == 0) devHandle = HALSIM_GetSimDeviceHandle("Mechanism2D");
    // Get the length
    if (!lengthHandle)
      lengthHandle = HALSIM_GetSimValueHandle(
          devHandle, (bodyConfig.name + "/length").c_str());
    if (lengthHandle) length = lengthHandle.Get();
    if (length <= 0) {
      length = bodyConfig.length;
    }
    // Get the angle
    if (!angleHandle)
      angleHandle = HALSIM_GetSimValueHandle(
          devHandle, (bodyConfig.name + "/angle").c_str());
    if (angleHandle) angle = angleHandle.Get();
    // Calculate the next angle to go to
    float angleToGoTo = angle + bodyConfig.angle + previousAngle;
    // Draw the first line and get the ending coordinates

    DrawLineStruct drawLine =
        DrawLine(startXLocation, startYLocation, minSize / 100 * length,
                 angleToGoTo, drawList, windowPos,
                 colorLookUpTable[bodyConfig.color], bodyConfig, "");

    // If the line has children then draw them with the stating points being the
    // end of the parent
    if (!bodyConfig.children.empty()) {
      buildDrawList(drawLine.xEnd, drawLine.yEnd, drawList, drawLine.angle,
                    bodyConfig.children, windowPos);
    }
  }
}

static BodyConfig readSubJson(const std::string& name, wpi::json const& body) {
  BodyConfig c;
  try {
    c.name = name + "/" + body.at("name").get<std::string>();
  } catch (const wpi::json::exception& e) {
    wpi::errs() << "could not read body name: " << e.what() << '\n';
  }
  try {
    c.length = body.at("length").get<int>();
  } catch (const wpi::json::exception& e) {
    wpi::errs() << "length '" << c.name
                << "': could not find length path: " << e.what() << '\n';
  }
  try {
    c.color = body.at("color").get<std::string>();
  } catch (const wpi::json::exception& e) {
    wpi::errs() << "color '" << c.name
                << "': could not find color path: " << e.what() << '\n';
  }
  try {
    c.angle = body.at("angle").get<int>();
  } catch (const wpi::json::exception& e) {
    wpi::errs() << "angle '" << c.name
                << "': could not find angle path: " << e.what() << '\n';
  }
  try {
    c.lineWidth = body.at("lineWidth").get<int>();
  } catch (const wpi::json::exception& e) {
    wpi::errs() << "lineWidth '" << c.name
                << "': could not find lineWidth path: " << e.what() << '\n';
  }
  try {
    for (wpi::json const& child : body.at("children")) {
      c.children.push_back(readSubJson(c.name, child));
      wpi::outs() << "Reading Child with name " << c.name << '\n';
    }
  } catch (const wpi::json::exception& e) {
    wpi::errs() << "could not read body: " << e.what() << '\n';
  }
  return c;
}

static void readJson(std::string jFile) {
  std::error_code ec;
  std::string name;
  wpi::raw_fd_istream is(jFile, ec);
  if (ec) {
    wpi::errs() << "could not open '" << jFile << "': " << ec.message() << '\n';
  }
  // parse file
  wpi::json j;
  try {
    j = wpi::json::parse(is);
  } catch (const wpi::json::parse_error& e) {
    wpi::errs() << "byte " << e.byte << ": " << e.what() << '\n';
  }
  // top level must be an object
  if (!j.is_object()) {
    wpi::errs() << "must be JSON object\n";
  }
  try {
    name = j.at("name").get<std::string>();
  } catch (const wpi::json::exception& e) {
    wpi::errs() << "name '" << name
                << "': could not find name path: " << e.what() << '\n';
  }
  try {
    for (wpi::json const& body : j.at("body")) {
      bodyConfigVector.push_back(readSubJson(name, body));
    }
  } catch (const wpi::json::exception& e) {
    wpi::errs() << "could not read body: " << e.what() << '\n';
  }
}

static void DisplayAssembly2D() {
  if (ImGui::BeginPopupContextItem()) {
    if (ImGui::MenuItem("Load Json")) {
      m_fileOpener = std::make_unique<pfd::open_file>(
          "Choose Mechanism2D json", "", std::vector<std::string>{"*.json"});
    }
    ImGui::EndPopup();
  }

  GetJsonFileLocation();
  if (!mechanism2DInfo.jsonLocation.empty()) {
    // Only read the json file if it changed
    if (mechanism2DInfo.jsonLocation != previousJsonLocation) {
      bodyConfigVector.clear();
      readJson(mechanism2DInfo.jsonLocation);
    }
    previousJsonLocation = mechanism2DInfo.jsonLocation;
    ImVec2 windowPos = ImGui::GetWindowPos();
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    buildDrawList(ImGui::GetWindowWidth() / 2, ImGui::GetWindowHeight(),
                  drawList, 0, bodyConfigVector, windowPos);
  }
}

void Mechanism2D::Initialize() {
  // hook ini handler to save settings
  ImGuiSettingsHandler iniHandler;
  iniHandler.TypeName = "Mechanism2D";
  iniHandler.TypeHash = ImHashStr(iniHandler.TypeName);
  iniHandler.ReadOpenFn = Mechanism2DReadOpen;
  iniHandler.ReadLineFn = Mechanism2DReadLine;
  iniHandler.WriteAllFn = Mechanism2DWriteAll;
  ImGui::GetCurrentContext()->SettingsHandlers.push_back(iniHandler);

  buildColorTable();
  if (auto win =
          HALSimGui::manager.AddWindow("Mechanism 2D", DisplayAssembly2D)) {
    win->SetVisibility(glass::Window::kHide);
    win->SetDefaultPos(200, 200);
    win->SetDefaultSize(600, 600);
    win->SetPadding(0, 0);
  }
}
