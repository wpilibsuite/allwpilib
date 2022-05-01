// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/hardware/DIO.h"

#include <imgui.h>

#include "glass/DataSource.h"
#include "glass/hardware/Encoder.h"
#include "glass/support/NameSetting.h"

using namespace glass;

static void LabelSimDevice(const char* name, const char* simDeviceName) {
  ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(96, 96, 96, 255));
  ImGui::LabelText(name, "%s", simDeviceName);
  ImGui::PopStyleColor();
}

void DisplayDIOImpl(DIOModel* model, int index, bool outputsEnabled) {
  auto dpwm = model->GetDPWM();
  auto dutyCycle = model->GetDutyCycle();
  auto encoder = model->GetEncoder();

  auto dioData = model->GetValueData();
  auto dpwmData = dpwm ? dpwm->GetValueData() : nullptr;
  auto dutyCycleData = dutyCycle ? dutyCycle->GetValueData() : nullptr;

  bool exists = model->Exists();
  NameSetting dioName{dioData->GetName()};
  char label[128];
  if (exists && dpwmData) {
    NameSetting{dpwmData->GetName()}.GetLabel(label, sizeof(label), "PWM",
                                              index);
    if (auto simDevice = dpwm->GetSimDevice()) {
      LabelSimDevice(label, simDevice);
    } else {
      dpwmData->LabelText(label, "%0.3f", dpwmData->GetValue());
    }
  } else if (exists && encoder) {
    dioName.GetLabel(label, sizeof(label), " In", index);
    if (auto simDevice = encoder->GetSimDevice()) {
      LabelSimDevice(label, simDevice);
    } else {
      ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(96, 96, 96, 255));
      ImGui::LabelText(label, "Encoder[%d,%d]", encoder->GetChannelA(),
                       encoder->GetChannelB());
      ImGui::PopStyleColor();
    }
  } else if (exists && dutyCycleData) {
    NameSetting{dutyCycleData->GetName()}.GetLabel(label, sizeof(label), "Dty",
                                                   index);
    if (auto simDevice = dutyCycle->GetSimDevice()) {
      LabelSimDevice(label, simDevice);
    } else {
      double val = dutyCycleData->GetValue();
      if (dutyCycleData->InputDouble(label, &val)) {
        dutyCycle->SetValue(val);
      }
    }
  } else {
    const char* name = model->GetName();
    if (name[0] != '\0') {
      dioName.GetLabel(label, sizeof(label), name);
    } else {
      dioName.GetLabel(label, sizeof(label), model->IsInput() ? " In" : "Out",
                       index);
    }
    if (auto simDevice = model->GetSimDevice()) {
      LabelSimDevice(label, simDevice);
    } else {
      if (!exists) {
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(96, 96, 96, 255));
        dioData->LabelText(label, "unknown");
        ImGui::PopStyleColor();
      } else if (model->IsReadOnly()) {
        dioData->LabelText(
            label, "%s",
            outputsEnabled ? (dioData->GetValue() != 0 ? "1 (high)" : "0 (low)")
                           : "1 (disabled)");

      } else {
        static const char* options[] = {"0 (low)", "1 (high)"};
        int val = dioData->GetValue() != 0 ? 1 : 0;
        if (dioData->Combo(label, &val, options, 2)) {
          model->SetValue(val);
        }
      }
    }
  }
  if (dioName.PopupEditName(index)) {
    if (dpwmData) {
      dpwmData->SetName(dioName.GetName());
    }
    if (dutyCycleData) {
      dutyCycleData->SetName(dioName.GetName());
    }
  }
}

void glass::DisplayDIO(DIOModel* model, int index, bool outputsEnabled) {
  ImGui::PushItemWidth(ImGui::GetFontSize() * 8);
  DisplayDIOImpl(model, index, outputsEnabled);
  ImGui::PopItemWidth();
}

void glass::DisplayDIOs(DIOsModel* model, bool outputsEnabled,
                        std::string_view noneMsg) {
  bool hasAny = false;

  ImGui::PushItemWidth(ImGui::GetFontSize() * 8);
  model->ForEachDIO([&](DIOModel& dio, int i) {
    hasAny = true;
    ImGui::PushID(i);
    DisplayDIOImpl(&dio, i, outputsEnabled);
    ImGui::PopID();
  });
  ImGui::PopItemWidth();
  if (!hasAny && !noneMsg.empty()) {
    ImGui::TextUnformatted(noneMsg.data(), noneMsg.data() + noneMsg.size());
  }
}
