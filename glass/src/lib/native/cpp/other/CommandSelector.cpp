/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "glass/other/CommandSelector.h"

#include <imgui.h>

#include "glass/Context.h"
#include "glass/DataSource.h"

using namespace glass;

void glass::DisplayCommandSelector(CommandSelectorModel* m) {
  if (auto name = m->GetName()) ImGui::Text("%s", name);
  if (m->Exists()) {
    if (auto run = m->GetRunningData()) {
      bool running = run->GetValue();
      if (ImGui::Button(running ? "Cancel" : "Run")) {
        running = !running;
        m->SetRunning(running);
      }
      ImGui::SameLine();
      if (running) ImGui::Text("Running...");
    }
  } else {
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(96, 96, 96, 255));
    ImGui::Text("Unknown Command");
    ImGui::PopStyleColor();
  }
}
