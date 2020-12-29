// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/other/CommandScheduler.h"

#include <imgui.h>

#include "glass/Context.h"
#include "glass/DataSource.h"

using namespace glass;

void glass::DisplayCommandScheduler(CommandSchedulerModel* m) {
  ImGui::SetNextItemWidth(ImGui::GetFontSize() * 20);
  ImGui::Text("Scheduled Commands: ");
  ImGui::Separator();
  ImGui::Spacing();

  if (m->Exists()) {
    float pos = ImGui::GetContentRegionAvail().x * 0.97f -
                ImGui::CalcTextSize("Cancel").x;

    const auto& commands = m->GetCurrentCommands();
    for (size_t i = 0; i < commands.size(); ++i) {
      ImGui::Text("%s", commands[i].c_str());
      ImGui::SameLine(pos);

      ImGui::PushID(i);
      if (ImGui::Button("Cancel")) {
        m->CancelCommand(i);
      }
      ImGui::PopID();
    }
  } else {
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(96, 96, 96, 255));
    ImGui::Text("Unknown Scheduler");
    ImGui::PopStyleColor();
  }
}
