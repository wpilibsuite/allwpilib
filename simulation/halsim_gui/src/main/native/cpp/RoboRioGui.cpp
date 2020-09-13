/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "RoboRioGui.h"

#include <memory>

#include <hal/simulation/RoboRioData.h>
#include <imgui.h>

#include "GuiDataSource.h"
#include "HALSimGui.h"

using namespace halsimgui;

namespace {
HALSIMGUI_DATASOURCE_DOUBLE(RoboRioVInVoltage, "Rio Input Voltage");
HALSIMGUI_DATASOURCE_DOUBLE(RoboRioVInCurrent, "Rio Input Current");
HALSIMGUI_DATASOURCE_DOUBLE(RoboRioUserVoltage6V, "Rio 6V Voltage");
HALSIMGUI_DATASOURCE_DOUBLE(RoboRioUserCurrent6V, "Rio 6V Current");
HALSIMGUI_DATASOURCE_BOOLEAN(RoboRioUserActive6V, "Rio 6V Active");
HALSIMGUI_DATASOURCE_INT(RoboRioUserFaults6V, "Rio 6V Faults");
HALSIMGUI_DATASOURCE_DOUBLE(RoboRioUserVoltage5V, "Rio 5V Voltage");
HALSIMGUI_DATASOURCE_DOUBLE(RoboRioUserCurrent5V, "Rio 5V Current");
HALSIMGUI_DATASOURCE_BOOLEAN(RoboRioUserActive5V, "Rio 5V Active");
HALSIMGUI_DATASOURCE_INT(RoboRioUserFaults5V, "Rio 5V Faults");
HALSIMGUI_DATASOURCE_DOUBLE(RoboRioUserVoltage3V3, "Rio 3.3V Voltage");
HALSIMGUI_DATASOURCE_DOUBLE(RoboRioUserCurrent3V3, "Rio 3.3V Current");
HALSIMGUI_DATASOURCE_BOOLEAN(RoboRioUserActive3V3, "Rio 3.3V Active");
HALSIMGUI_DATASOURCE_INT(RoboRioUserFaults3V3, "Rio 3.3V Faults");
struct RoboRioSource {
  RoboRioVInVoltageSource vInVoltage;
  RoboRioVInCurrentSource vInCurrent;
  RoboRioUserVoltage6VSource userVoltage6V;
  RoboRioUserCurrent6VSource userCurrent6V;
  RoboRioUserActive6VSource userActive6V;
  RoboRioUserFaults6VSource userFaults6V;
  RoboRioUserVoltage5VSource userVoltage5V;
  RoboRioUserCurrent5VSource userCurrent5V;
  RoboRioUserActive5VSource userActive5V;
  RoboRioUserFaults5VSource userFaults5V;
  RoboRioUserVoltage3V3Source userVoltage3V3;
  RoboRioUserCurrent3V3Source userCurrent3V3;
  RoboRioUserActive3V3Source userActive3V3;
  RoboRioUserFaults3V3Source userFaults3V3;
};
}  // namespace

static std::unique_ptr<RoboRioSource> gRioSource;

static void UpdateRoboRioSources() {
  if (!gRioSource) gRioSource = std::make_unique<RoboRioSource>();
}

static void DisplayRoboRio() {
  ImGui::Button("User Button");
  HALSIM_SetRoboRioFPGAButton(ImGui::IsItemActive());

  ImGui::PushItemWidth(ImGui::GetFontSize() * 8);

  if (ImGui::CollapsingHeader("RoboRIO Input")) {
    {
      double val = gRioSource->vInVoltage.GetValue();
      if (gRioSource->vInVoltage.InputDouble("Voltage (V)", &val))
        HALSIM_SetRoboRioVInVoltage(val);
    }

    {
      double val = gRioSource->vInCurrent.GetValue();
      if (gRioSource->vInCurrent.InputDouble("Current (A)", &val))
        HALSIM_SetRoboRioVInCurrent(val);
    }
  }

  if (ImGui::CollapsingHeader("6V Rail")) {
    {
      double val = gRioSource->userVoltage6V.GetValue();
      if (gRioSource->userVoltage6V.InputDouble("Voltage (V)", &val))
        HALSIM_SetRoboRioUserVoltage6V(val);
    }

    {
      double val = gRioSource->userCurrent6V.GetValue();
      if (gRioSource->userCurrent6V.InputDouble("Current (A)", &val))
        HALSIM_SetRoboRioUserCurrent6V(val);
    }

    {
      static const char* options[] = {"inactive", "active"};
      int val = gRioSource->userActive6V.GetValue() ? 1 : 0;
      if (gRioSource->userActive6V.Combo("Active", &val, options, 2))
        HALSIM_SetRoboRioUserActive6V(val);
    }

    {
      int val = gRioSource->userFaults6V.GetValue();
      if (gRioSource->userFaults6V.InputInt("Faults", &val))
        HALSIM_SetRoboRioUserFaults6V(val);
    }
  }

  if (ImGui::CollapsingHeader("5V Rail")) {
    {
      double val = gRioSource->userVoltage5V.GetValue();
      if (gRioSource->userVoltage5V.InputDouble("Voltage (V)", &val))
        HALSIM_SetRoboRioUserVoltage5V(val);
    }

    {
      double val = gRioSource->userCurrent5V.GetValue();
      if (gRioSource->userCurrent5V.InputDouble("Current (A)", &val))
        HALSIM_SetRoboRioUserCurrent5V(val);
    }

    {
      static const char* options[] = {"inactive", "active"};
      int val = gRioSource->userActive5V.GetValue() ? 1 : 0;
      if (gRioSource->userActive5V.Combo("Active", &val, options, 2))
        HALSIM_SetRoboRioUserActive5V(val);
    }

    {
      int val = gRioSource->userFaults5V.GetValue();
      if (gRioSource->userFaults5V.InputInt("Faults", &val))
        HALSIM_SetRoboRioUserFaults5V(val);
    }
  }

  if (ImGui::CollapsingHeader("3.3V Rail")) {
    {
      double val = gRioSource->userVoltage3V3.GetValue();
      if (gRioSource->userVoltage3V3.InputDouble("Voltage (V)", &val))
        HALSIM_SetRoboRioUserVoltage3V3(val);
    }

    {
      double val = gRioSource->userCurrent3V3.GetValue();
      if (gRioSource->userCurrent3V3.InputDouble("Current (A)", &val))
        HALSIM_SetRoboRioUserCurrent3V3(val);
    }

    {
      static const char* options[] = {"inactive", "active"};
      int val = HALSIM_GetRoboRioUserActive3V3() ? 1 : 0;
      if (gRioSource->userActive3V3.Combo("Active", &val, options, 2))
        HALSIM_SetRoboRioUserActive3V3(val);
    }

    {
      int val = gRioSource->userFaults3V3.GetValue();
      if (gRioSource->userFaults3V3.InputInt("Faults", &val))
        HALSIM_SetRoboRioUserFaults3V3(val);
    }
  }

  ImGui::PopItemWidth();
}

void RoboRioGui::Initialize() {
  HALSimGui::AddExecute(UpdateRoboRioSources);
  HALSimGui::AddWindow("RoboRIO", DisplayRoboRio,
                       ImGuiWindowFlags_AlwaysAutoResize);
  // hide it by default
  HALSimGui::SetWindowVisibility("RoboRIO", HALSimGui::kHide);
  HALSimGui::SetDefaultWindowPos("RoboRIO", 5, 125);
}
