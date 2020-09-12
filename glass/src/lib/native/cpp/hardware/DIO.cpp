/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "glass/hardware/DIO.h"

#include <imgui.h>

#include "glass/DataSource.h"
#include "glass/hardware/Encoder.h"
#include "glass/support/IniSaverInfo.h"

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
  auto& info = dioData->GetNameInfo();
  char label[128];
  if (exists && dpwmData) {
    dpwmData->GetNameInfo().GetLabel(label, sizeof(label), "PWM", index);
    if (auto simDevice = dpwm->GetSimDevice()) {
      LabelSimDevice(label, simDevice);
    } else {
      dpwmData->LabelText(label, "%0.3f", dpwmData->GetValue());
    }
  } else if (exists && encoder) {
    info.GetLabel(label, sizeof(label), " In", index);
    if (auto simDevice = encoder->GetSimDevice()) {
      LabelSimDevice(label, simDevice);
    } else {
      ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(96, 96, 96, 255));
      ImGui::LabelText(label, "Encoder[%d,%d]", encoder->GetChannelA(),
                       encoder->GetChannelB());
      ImGui::PopStyleColor();
    }
  } else if (exists && dutyCycleData) {
    dutyCycleData->GetNameInfo().GetLabel(label, sizeof(label), "Dty", index);
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
    if (name[0] != '\0')
      info.GetLabel(label, sizeof(label), name);
    else
      info.GetLabel(label, sizeof(label), model->IsInput() ? " In" : "Out",
                    index);
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
  if (info.PopupEditName(index)) {
    if (dpwmData) dpwmData->SetName(info.GetName());
    if (dutyCycleData) dutyCycleData->SetName(info.GetName());
  }
}

void glass::DisplayDIO(DIOModel* model, int index, bool outputsEnabled) {
  ImGui::PushItemWidth(ImGui::GetFontSize() * 8);
  DisplayDIOImpl(model, index, outputsEnabled);
  ImGui::PopItemWidth();
}

void glass::DisplayDIOs(DIOsModel* model, bool outputsEnabled,
                        wpi::StringRef noneMsg) {
  bool hasAny = false;

  ImGui::PushItemWidth(ImGui::GetFontSize() * 8);
  model->ForEachDIO([&](DIOModel& dio, int i) {
    hasAny = true;
    ImGui::PushID(i);
    DisplayDIOImpl(&dio, i, outputsEnabled);
    ImGui::PopID();
  });
  ImGui::PopItemWidth();
  if (!hasAny && !noneMsg.empty())
    ImGui::TextUnformatted(noneMsg.begin(), noneMsg.end());
}
