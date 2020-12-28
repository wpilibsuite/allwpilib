// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/other/CommandSelector.h"

#include <imgui.h>

#include "glass/Context.h"
#include "glass/DataSource.h"

using namespace glass;

void glass::DisplayCommandSelector(CommandSelectorModel* m) {
  if (auto name = m->GetName()) {
    ImGui::Text("%s", name);
  }
  if (m->Exists()) {
    if (auto run = m->GetRunningData()) {
      bool running = run->GetValue();
      if (ImGui::Button(running ? "Cancel" : "Run")) {
        running = !running;
        m->SetRunning(running);
      }
      ImGui::SameLine();
      if (running) {
        ImGui::Text("Running...");
      }
    }
  } else {
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(96, 96, 96, 255));
    ImGui::Text("Unknown Command");
    ImGui::PopStyleColor();
  }
}
