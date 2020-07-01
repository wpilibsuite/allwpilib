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

namespace {

}
int counter = 0;
static void DisplayField2D() {
  ImGui::InvisibleButton("field", ImGui::GetContentRegionAvail());
  ImVec2 windowPos = ImGui::GetWindowPos();
  auto drawList = ImGui::GetWindowDrawList();
  drawList->AddCircle(windowPos, counter++, IM_COL32(0, 255, 0, 255));
  if(counter > 1000){
    counter = 0;
  }
}


void Assembly2D::Initialize() {
  // hook ini handler to save settings
  ImGuiSettingsHandler iniHandler;
  iniHandler.TypeName = "2D Assembly";
  iniHandler.TypeHash = ImHashStr(iniHandler.TypeName);
  ImGui::GetCurrentContext()->SettingsHandlers.push_back(iniHandler);
  HALSimGui::AddWindow("2D Assembly", DisplayField2D);
  // HALSimGui::SetWindowVisibility("2D Assembly", HALSimGui::kHide);
  HALSimGui::SetDefaultWindowPos("2D Assembly", 200, 200);
  HALSimGui::SetDefaultWindowSize("2D Assembly", 400, 200);
  HALSimGui::SetWindowPadding("2D Assembly", 0, 0);
}
