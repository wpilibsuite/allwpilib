/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "RoboRioGui.h"

#include <imgui.h>
#include <mockdata/RoboRioData.h>

#include "HALSimGui.h"

using namespace halsimgui;

static void DisplayRoboRio() {
  ImGui::Button("User Button");
  HALSIM_SetRoboRioFPGAButton(0, ImGui::IsItemActive());

  ImGui::PushItemWidth(ImGui::GetFontSize() * 8);

  if (ImGui::CollapsingHeader("RoboRIO Input")) {
    {
      double val = HALSIM_GetRoboRioVInVoltage(0);
      if (ImGui::InputDouble("Voltage (V)", &val))
        HALSIM_SetRoboRioVInVoltage(0, val);
    }

    {
      double val = HALSIM_GetRoboRioVInCurrent(0);
      if (ImGui::InputDouble("Current (A)", &val))
        HALSIM_SetRoboRioVInCurrent(0, val);
    }
  }

  if (ImGui::CollapsingHeader("6V Rail")) {
    {
      double val = HALSIM_GetRoboRioUserVoltage6V(0);
      if (ImGui::InputDouble("Voltage (V)", &val))
        HALSIM_SetRoboRioUserVoltage6V(0, val);
    }

    {
      double val = HALSIM_GetRoboRioUserCurrent6V(0);
      if (ImGui::InputDouble("Current (A)", &val))
        HALSIM_SetRoboRioUserCurrent6V(0, val);
    }

    {
      static const char* options[] = {"inactive", "active"};
      int val = HALSIM_GetRoboRioUserActive6V(0) ? 1 : 0;
      if (ImGui::Combo("Active", &val, options, 2))
        HALSIM_SetRoboRioUserActive6V(0, val);
    }

    {
      int val = HALSIM_GetRoboRioUserFaults6V(0);
      if (ImGui::InputInt("Faults", &val))
        HALSIM_SetRoboRioUserFaults6V(0, val);
    }
  }

  if (ImGui::CollapsingHeader("5V Rail")) {
    {
      double val = HALSIM_GetRoboRioUserVoltage5V(0);
      if (ImGui::InputDouble("Voltage (V)", &val))
        HALSIM_SetRoboRioUserVoltage5V(0, val);
    }

    {
      double val = HALSIM_GetRoboRioUserCurrent5V(0);
      if (ImGui::InputDouble("Current (A)", &val))
        HALSIM_SetRoboRioUserCurrent5V(0, val);
    }

    {
      static const char* options[] = {"inactive", "active"};
      int val = HALSIM_GetRoboRioUserActive5V(0) ? 1 : 0;
      if (ImGui::Combo("Active", &val, options, 2))
        HALSIM_SetRoboRioUserActive5V(0, val);
    }

    {
      int val = HALSIM_GetRoboRioUserFaults5V(0);
      if (ImGui::InputInt("Faults", &val))
        HALSIM_SetRoboRioUserFaults5V(0, val);
    }
  }

  if (ImGui::CollapsingHeader("3.3V Rail")) {
    {
      double val = HALSIM_GetRoboRioUserVoltage3V3(0);
      if (ImGui::InputDouble("Voltage (V)", &val))
        HALSIM_SetRoboRioUserVoltage3V3(0, val);
    }

    {
      double val = HALSIM_GetRoboRioUserCurrent3V3(0);
      if (ImGui::InputDouble("Current (A)", &val))
        HALSIM_SetRoboRioUserCurrent3V3(0, val);
    }

    {
      static const char* options[] = {"inactive", "active"};
      int val = HALSIM_GetRoboRioUserActive3V3(0) ? 1 : 0;
      if (ImGui::Combo("Active", &val, options, 2))
        HALSIM_SetRoboRioUserActive3V3(0, val);
    }

    {
      int val = HALSIM_GetRoboRioUserFaults3V3(0);
      if (ImGui::InputInt("Faults", &val))
        HALSIM_SetRoboRioUserFaults3V3(0, val);
    }
  }

  ImGui::PopItemWidth();
}

void RoboRioGui::Initialize() {
  HALSimGui::AddWindow("RoboRIO", DisplayRoboRio,
                       ImGuiWindowFlags_AlwaysAutoResize);
  // hide it by default
  HALSimGui::SetWindowVisibility("RoboRIO", HALSimGui::kHide);
  HALSimGui::SetDefaultWindowPos("RoboRIO", 5, 125);
}
