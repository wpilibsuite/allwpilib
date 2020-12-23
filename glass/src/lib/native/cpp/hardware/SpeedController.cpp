/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "glass/hardware/SpeedController.h"

#include <imgui.h>

#include "glass/Context.h"
#include "glass/DataSource.h"

using namespace glass;

void glass::DisplaySpeedController(SpeedControllerModel* m) {
  // Get duty cycle data from the model and do not display anything if the data
  // is null.
  auto dc = m->GetPercentData();
  if (!dc || !m->Exists()) {
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(96, 96, 96, 255));
    ImGui::Text("Unknown SpeedController");
    ImGui::PopStyleColor();
    return;
  }

  // Add button to zero output.
  if (ImGui::Button("Zero")) m->SetPercent(0.0);
  ImGui::SameLine();

  // Display a slider for the data.
  float value = dc->GetValue();
  ImGui::SetNextItemWidth(ImGui::GetFontSize() * 8);
  if (dc->SliderFloat("% Output", &value, -1.0f, 1.0f)) m->SetPercent(value);
}
