/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "glass/other/StringChooser.h"

#include <imgui.h>

using namespace glass;

void glass::DisplayStringChooser(StringChooserModel* model) {
  auto& defaultValue = model->GetDefault();
  auto& selected = model->GetSelected();
  auto& active = model->GetActive();
  auto& options = model->GetOptions();

  const char* preview =
      selected.empty() ? defaultValue.c_str() : selected.c_str();

  const char* label;
  if (active == preview) {
    label = "GOOD##select";
  } else {
    label = "BAD ##select";
  }

  if (ImGui::BeginCombo(label, preview)) {
    for (auto&& option : options) {
      ImGui::PushID(option.c_str());
      bool isSelected = (option == selected);
      if (ImGui::Selectable(option.c_str(), isSelected)) {
        model->SetSelected(option);
      }
      if (isSelected) ImGui::SetItemDefaultFocus();
      ImGui::PopID();
    }
    ImGui::EndCombo();
  }

  ImGui::SameLine();
}
