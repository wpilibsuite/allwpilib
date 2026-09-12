// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/glass/other/Subsystem.hpp"

#include <string>

#include <imgui.h>

using namespace wpi::glass;

void wpi::glass::DisplaySubsystem(SubsystemModel* m) {
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
    ImGui::PushStyleColor(ImGuiCol_Text,
                          ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));
    ImGui::Text("Unknown Subsystem");
    ImGui::PopStyleColor();
  }
}
