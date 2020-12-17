/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "glass/other/Subsystem.h"

#include <imgui.h>

#include "glass/Context.h"
#include "glass/DataSource.h"

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
