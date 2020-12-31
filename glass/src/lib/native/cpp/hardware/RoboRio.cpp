// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/hardware/RoboRio.h"

#include <imgui.h>

#include "glass/Context.h"
#include "glass/DataSource.h"

using namespace glass;

static void DisplayRail(RoboRioRailModel& rail, const char* name) {
  if (CollapsingHeader(name)) {
    ImGui::PushID(name);
    if (auto data = rail.GetVoltageData()) {
      double val = data->GetValue();
      if (data->InputDouble("Voltage (V)", &val)) {
        rail.SetVoltage(val);
      }
    }

    if (auto data = rail.GetCurrentData()) {
      double val = data->GetValue();
      if (data->InputDouble("Current (A)", &val)) {
        rail.SetCurrent(val);
      }
    }

    if (auto data = rail.GetActiveData()) {
      static const char* options[] = {"inactive", "active"};
      int val = data->GetValue() ? 1 : 0;
      if (data->Combo("Active", &val, options, 2)) {
        rail.SetActive(val);
      }
    }

    if (auto data = rail.GetFaultsData()) {
      int val = data->GetValue();
      if (data->InputInt("Faults", &val)) {
        rail.SetFaults(val);
      }
    }
    ImGui::PopID();
  }
}

void glass::DisplayRoboRio(RoboRioModel* model) {
  ImGui::Button("User Button");
  model->SetUserButton(ImGui::IsItemActive());

  ImGui::PushItemWidth(ImGui::GetFontSize() * 8);

  if (CollapsingHeader("RoboRIO Input")) {
    ImGui::PushID("RoboRIO Input");
    if (auto data = model->GetVInVoltageData()) {
      double val = data->GetValue();
      if (data->InputDouble("Voltage (V)", &val)) {
        model->SetVInVoltage(val);
      }
    }

    if (auto data = model->GetVInCurrentData()) {
      double val = data->GetValue();
      if (data->InputDouble("Current (A)", &val)) {
        model->SetVInCurrent(val);
      }
    }
    ImGui::PopID();
  }

  if (auto rail = model->GetUser6VRail()) {
    DisplayRail(*rail, "6V Rail");
  }
  if (auto rail = model->GetUser5VRail()) {
    DisplayRail(*rail, "5V Rail");
  }
  if (auto rail = model->GetUser3V3Rail()) {
    DisplayRail(*rail, "3.3V Rail");
  }

  ImGui::PopItemWidth();
}
