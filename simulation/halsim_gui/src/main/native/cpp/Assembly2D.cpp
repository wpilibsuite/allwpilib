/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Assembly2D.h"

#include <cmath>

#include <GL/gl3w.h>
#include <hal/SimDevice.h>
#include <imgui.h>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>
#include <mockdata/SimDeviceData.h>
#include <units/units.h>
#include <wpi/Path.h>
#include <wpi/SmallString.h>
#include <wpi/json.h>
#include <wpi/raw_istream.h>
#include <wpi/raw_ostream.h>

#include "GuiUtil.h"
#include "HALSimGui.h"
#include "SimDeviceGui.h"
#include "portable-file-dialogs.h"

using namespace halsimgui;

int counter = 0;
static void DisplayAssembly2D() {
  ImGui::InvisibleButton("field", ImGui::GetContentRegionAvail());
  ImVec2 windowPos = ImGui::GetWindowPos();
  auto drawList = ImGui::GetWindowDrawList();
  drawList->AddCircle(windowPos, counter++, IM_COL32(0, 255, 0, 255));
  if (counter > 1000) {
    counter = 0;
  }
}

struct BodyConfig {
  std::string name;
  std::string type;
};

void static readJson(std::string jFile) {
  // std::ifstream jsonRead(jFile);
  // wpi::json jsonObject;
  // jsonRead >> jsonObject;
  std::string name;
  // open config file
  std::error_code ec;
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

  // // team number
  // try {
  //   name = j.at("name").get<std::string>();
  // } catch (const wpi::json::exception& e) {
  //   wpi::errs() << "could not read team number: " << e.what() << '\n';
  // }

  BodyConfig tmpBodyConfig;
  BodyConfig c;
  try {
    for (auto&& body : j.at("body")) {
      try {
        c.name = body.at("name").get<std::string>();
      } catch (const wpi::json::exception& e) {
        wpi::errs() << "could not read body name: " << e.what() << '\n';
      }

      // path
      try {
        c.type = body.at("type").get<std::string>();
      } catch (const wpi::json::exception& e) {
        wpi::errs() << "camera '" << c.name
                    << "': could not type path: " << e.what() << '\n';
      }
    }
  } catch (const wpi::json::exception& e) {
    wpi::errs() << "could not read body: " << e.what() << '\n';
  }
  wpi::outs() << c.name << " " << c.type << "\n";
}

// BodyConfig static ReadBody(const wpi::json& config) {
//   BodyConfig c;

//   // name
//   try {
//     c.name = config.at("name").get<std::string>();
//   } catch (const wpi::json::exception& e) {
//     wpi::errs() << "could not read body name: " << e.what() << '\n';
//   }

//   // path
//   try {
//     c.type = config.at("type").get<std::string>();
//   } catch (const wpi::json::exception& e) {
//     wpi::errs() << "camera '" << c.name
//                 << "': could not type path: " << e.what() << '\n';
//   }
//   return BodyConfig
// }

void Assembly2D::Initialize() {
  // hook ini handler to save settings
  ImGuiSettingsHandler iniHandler;
  iniHandler.TypeName = "2D Assembly";
  iniHandler.TypeHash = ImHashStr(iniHandler.TypeName);
  ImGui::GetCurrentContext()->SettingsHandlers.push_back(iniHandler);
  HALSimGui::AddWindow("2D Assembly", DisplayAssembly2D);
  HALSimGui::SetDefaultWindowPos("2D Assembly", 200, 200);
  HALSimGui::SetDefaultWindowSize("2D Assembly", 400, 200);
  HALSimGui::SetWindowPadding("2D Assembly", 0, 0);
  readJson("/home/gabe/github/allwpilib/Assembly2D.json");
}