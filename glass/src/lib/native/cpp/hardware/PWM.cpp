// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/hardware/PWM.h"

#include <string>

#include <imgui.h>
#include <wpi/StringExtras.h>

#include "glass/Context.h"
#include "glass/DataSource.h"
#include "glass/Storage.h"

using namespace glass;

void glass::DisplayPWM(PWMModel* model, int index, bool outputsEnabled) {
  auto data = model->GetSpeedData();
  if (!data) {
    return;
  }

  // build label
  std::string& name = GetStorage().GetString("name");
  char label[128];
  if (!name.empty()) {
    wpi::format_to_n_c_str(label, sizeof(label), "{} [{}]###name", name, index);
  } else {
    wpi::format_to_n_c_str(label, sizeof(label), "PWM[{}]###name", index);
  }

  int led = model->GetAddressableLED();

  ImGui::SetNextItemWidth(ImGui::GetFontSize() * 4);
  if (led >= 0) {
    ImGui::LabelText(label, "LED[%d]", led);
  } else {
    float val = outputsEnabled ? data->GetValue() : 0;
    data->LabelText(label, "%0.3f", val);
  }
  if (PopupEditName("name", &name)) {
    data->SetName(name);
  }
}

void glass::DisplayPWMs(PWMsModel* model, bool outputsEnabled,
                        std::string_view noneMsg) {
  bool hasAny = false;
  bool first = true;
  model->ForEachPWM([&](PWMModel& pwm, int i) {
    hasAny = true;
    PushID(i);

    if (!first) {
      ImGui::Separator();
    } else {
      first = false;
    }

    DisplayPWM(&pwm, i, outputsEnabled);
    PopID();
  });
  if (!hasAny && !noneMsg.empty()) {
    ImGui::TextUnformatted(noneMsg.data(), noneMsg.data() + noneMsg.size());
  }
}
