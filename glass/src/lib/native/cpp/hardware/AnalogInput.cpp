// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/hardware/AnalogInput.h"

#include <string>

#include <imgui.h>
#include <wpi/StringExtras.h>

#include "glass/Context.h"
#include "glass/DataSource.h"
#include "glass/Storage.h"

using namespace glass;

void glass::DisplayAnalogInput(AnalogInputModel* model, int index) {
  auto voltageData = model->GetVoltageData();
  if (!voltageData) {
    return;
  }

  // build label
  std::string& name = GetStorage().GetString("name");
  char label[128];
  if (!name.empty()) {
    wpi::format_to_n_c_str(label, sizeof(label), "{} [{}]###name", name, index);
  } else {
    wpi::format_to_n_c_str(label, sizeof(label), "In[{}]###name", index);
  }

  if (model->IsGyro()) {
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(96, 96, 96, 255));
    ImGui::LabelText(label, "AnalogGyro[%d]", index);
    ImGui::PopStyleColor();
  } else if (auto simDevice = model->GetSimDevice()) {
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(96, 96, 96, 255));
    ImGui::LabelText(label, "%s", simDevice);
    ImGui::PopStyleColor();
  } else {
    float val = voltageData->GetValue();
    if (voltageData->SliderFloat(label, &val, 0.0, 5.0)) {
      model->SetVoltage(val);
    }
  }

  // context menu to change name
  if (PopupEditName("name", &name)) {
    voltageData->SetName(name);
  }
}

void glass::DisplayAnalogInputs(AnalogInputsModel* model,
                                std::string_view noneMsg) {
  ImGui::Text("(Use Ctrl+Click to edit value)");
  bool hasAny = false;
  bool first = true;
  model->ForEachAnalogInput([&](AnalogInputModel& input, int i) {
    if (!first) {
      ImGui::Spacing();
      ImGui::Spacing();
    } else {
      first = false;
    }
    PushID(i);
    DisplayAnalogInput(&input, i);
    PopID();
    hasAny = true;
  });
  if (!hasAny && !noneMsg.empty()) {
    ImGui::TextUnformatted(noneMsg.data(), noneMsg.data() + noneMsg.size());
  }
}
