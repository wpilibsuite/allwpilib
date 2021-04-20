// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

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
      if (isSelected) {
        ImGui::SetItemDefaultFocus();
      }
      ImGui::PopID();
    }
    ImGui::EndCombo();
  }
}
