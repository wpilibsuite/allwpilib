// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/other/Subsystem.h"

#include <string>

#include <imgui.h>

using namespace glass;

void glass::DisplaySubsystem(SubsystemModel* m) {
  if (auto name = m->GetName()) {
    ImGui::Text("%s", name);
    ImGui::Separator();
  }
  if (m->Exists()) {
    std::string defaultCommand = m->GetDefaultCommand();
    std::string currentCommand = m->GetCurrentCommand();
    ImGui::Text("%s", ("Default Command: " + defaultCommand).c_str());
    ImGui::Text("%s", ("Current Command: " + currentCommand).c_str());
  } else {
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(96, 96, 96, 255));
    ImGui::Text("Unknown Subsystem");
    ImGui::PopStyleColor();
  }
}
