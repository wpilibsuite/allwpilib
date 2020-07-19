/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "RoboRioGui.h"

#include <hal/simulation/RoboRioData.h>
#include <imgui.h>

#include "HALSimGui.h"

using namespace halsimgui;

static void DisplayRoboRio() {
  ImGui::Button("User Button");
  HALSIM_SetRoboRioFPGAButton(ImGui::IsItemActive());

  ImGui::PushItemWidth(ImGui::GetFontSize() * 8);

  if (ImGui::CollapsingHeader("RoboRIO Input")) {
    {
      double val = HALSIM_GetRoboRioVInVoltage();
      if (ImGui::InputDouble("Voltage (V)", &val))
        HALSIM_SetRoboRioVInVoltage(val);
    }

    {
      double val = HALSIM_GetRoboRioVInCurrent();
      if (ImGui::InputDouble("Current (A)", &val))
        HALSIM_SetRoboRioVInCurrent(val);
    }
  }

  if (ImGui::CollapsingHeader("6V Rail")) {
    {
      double val = HALSIM_GetRoboRioUserVoltage6V();
      if (ImGui::InputDouble("Voltage (V)", &val))
        HALSIM_SetRoboRioUserVoltage6V(val);
    }

    {
      double val = HALSIM_GetRoboRioUserCurrent6V();
      if (ImGui::InputDouble("Current (A)", &val))
        HALSIM_SetRoboRioUserCurrent6V(val);
    }

    {
      static const char* options[] = {"inactive", "active"};
      int val = HALSIM_GetRoboRioUserActive6V() ? 1 : 0;
      if (ImGui::Combo("Active", &val, options, 2))
        HALSIM_SetRoboRioUserActive6V(val);
    }

    {
      int val = HALSIM_GetRoboRioUserFaults6V();
      if (ImGui::InputInt("Faults", &val)) HALSIM_SetRoboRioUserFaults6V(val);
    }
  }

  if (ImGui::CollapsingHeader("5V Rail")) {
    {
      double val = HALSIM_GetRoboRioUserVoltage5V();
      if (ImGui::InputDouble("Voltage (V)", &val))
        HALSIM_SetRoboRioUserVoltage5V(val);
    }

    {
      double val = HALSIM_GetRoboRioUserCurrent5V();
      if (ImGui::InputDouble("Current (A)", &val))
        HALSIM_SetRoboRioUserCurrent5V(val);
    }

    {
      static const char* options[] = {"inactive", "active"};
      int val = HALSIM_GetRoboRioUserActive5V() ? 1 : 0;
      if (ImGui::Combo("Active", &val, options, 2))
        HALSIM_SetRoboRioUserActive5V(val);
    }

    {
      int val = HALSIM_GetRoboRioUserFaults5V();
      if (ImGui::InputInt("Faults", &val)) HALSIM_SetRoboRioUserFaults5V(val);
    }
  }

  if (ImGui::CollapsingHeader("3.3V Rail")) {
    {
      double val = HALSIM_GetRoboRioUserVoltage3V3();
      if (ImGui::InputDouble("Voltage (V)", &val))
        HALSIM_SetRoboRioUserVoltage3V3(val);
    }

    {
      double val = HALSIM_GetRoboRioUserCurrent3V3();
      if (ImGui::InputDouble("Current (A)", &val))
        HALSIM_SetRoboRioUserCurrent3V3(val);
    }

    {
      static const char* options[] = {"inactive", "active"};
      int val = HALSIM_GetRoboRioUserActive3V3() ? 1 : 0;
      if (ImGui::Combo("Active", &val, options, 2))
        HALSIM_SetRoboRioUserActive3V3(val);
    }

    {
      int val = HALSIM_GetRoboRioUserFaults3V3();
      if (ImGui::InputInt("Faults", &val)) HALSIM_SetRoboRioUserFaults3V3(val);
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
