/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "glass/hardware/PWM.h"

#include <imgui.h>

#include "glass/Context.h"
#include "glass/DataSource.h"

using namespace glass;

void glass::DisplayPWM(PWMModel* model, int index, bool outputsEnabled) {
  auto data = model->GetSpeedData();
  if (!data) return;

  // build label
  std::string* name = GetStorage().GetStringRef("name");
  char label[128];
  if (!name->empty()) {
    std::snprintf(label, sizeof(label), "%s [%d]###name", name->c_str(), index);
  } else {
    std::snprintf(label, sizeof(label), "PWM[%d]###name", index);
  }

  int led = model->GetAddressableLED();

  ImGui::SetNextItemWidth(ImGui::GetFontSize() * 4);
  if (led >= 0) {
    ImGui::LabelText(label, "LED[%d]", led);
  } else {
    float val = outputsEnabled ? data->GetValue() : 0;
    data->LabelText(label, "%0.3f", val);
  }
  if (PopupEditName("name", name)) {
    data->SetName(name->c_str());
  }
}

void glass::DisplayPWMs(PWMsModel* model, bool outputsEnabled,
                        wpi::StringRef noneMsg) {
  bool hasAny = false;
  bool first = true;
  model->ForEachPWM([&](PWMModel& pwm, int i) {
    hasAny = true;
    PushID(i);

    if (!first)
      ImGui::Separator();
    else
      first = false;

    DisplayPWM(&pwm, i, outputsEnabled);
    PopID();
  });
  if (!hasAny && !noneMsg.empty())
    ImGui::TextUnformatted(noneMsg.begin(), noneMsg.end());
}
