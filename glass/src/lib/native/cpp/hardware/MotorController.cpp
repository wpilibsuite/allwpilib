// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/hardware/MotorController.h"

#include <imgui.h>
#include <imgui_internal.h>

#include "glass/Context.h"
#include "glass/DataSource.h"

using namespace glass;

void glass::DisplayMotorController(MotorControllerModel* m) {
  // Get duty cycle data from the model and do not display anything if the data
  // is null.
  auto dc = m->GetPercentData();
  if (!dc || !m->Exists()) {
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(96, 96, 96, 255));
    ImGui::Text("Unknown MotorController");
    ImGui::PopStyleColor();
    return;
  }

  // Set the buttons and sliders to read-only if the model is read-only.
  if (m->IsReadOnly()) {
    ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(210, 210, 210, 255));
  }

  // Add button to zero output.
  if (ImGui::Button("Zero")) {
    m->SetPercent(0.0);
  }
  ImGui::SameLine();

  // Display a slider for the data.
  float value = dc->GetValue();
  ImGui::SetNextItemWidth(ImGui::GetFontSize() * 8);

  if (dc->SliderFloat("% Output", &value, -1.0f, 1.0f)) {
    m->SetPercent(value);
  }

  if (m->IsReadOnly()) {
    ImGui::PopStyleColor();
    ImGui::PopItemFlag();
  }
}
