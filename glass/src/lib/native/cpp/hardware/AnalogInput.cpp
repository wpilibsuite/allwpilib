/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "glass/hardware/AnalogInput.h"

#include <imgui.h>

#include "glass/Context.h"
#include "glass/DataSource.h"

using namespace glass;

void glass::DisplayAnalogInput(AnalogInputModel* model, int index) {
  auto voltageData = model->GetVoltageData();
  if (!voltageData) return;

  // build label
  std::string* name = GetStorage().GetStringRef("name");
  char label[128];
  if (!name->empty()) {
    std::snprintf(label, sizeof(label), "%s [%d]###name", name->c_str(), index);
  } else {
    std::snprintf(label, sizeof(label), "In[%d]###name", index);
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
    if (voltageData->SliderFloat(label, &val, 0.0, 5.0)) model->SetVoltage(val);
  }

  // context menu to change name
  if (PopupEditName("name", name)) voltageData->SetName(name->c_str());
}

void glass::DisplayAnalogInputs(AnalogInputsModel* model,
                                wpi::StringRef noneMsg) {
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
  if (!hasAny && !noneMsg.empty())
    ImGui::TextUnformatted(noneMsg.begin(), noneMsg.end());
}
