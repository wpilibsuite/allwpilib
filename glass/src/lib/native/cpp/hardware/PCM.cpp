/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "glass/hardware/PCM.h"

#include <cstdio>
#include <cstring>

#include <imgui.h>
#include <wpi/SmallVector.h>

#include "glass/Context.h"
#include "glass/DataSource.h"
#include "glass/other/DeviceTree.h"
#include "glass/support/ExtraGuiWidgets.h"
#include "glass/support/IniSaverInfo.h"

using namespace glass;

bool glass::DisplayPCMSolenoids(PCMModel* model, int index,
                                bool outputsEnabled) {
  wpi::SmallVector<int, 16> channels;
  model->ForEachSolenoid([&](SolenoidModel& solenoid, int j) {
    if (auto data = solenoid.GetOutputData()) {
      if (j >= static_cast<int>(channels.size())) channels.resize(j + 1);
      channels[j] = (outputsEnabled && data->GetValue()) ? 1 : -1;
    }
  });

  if (channels.empty()) return false;

  // show nonexistent channels as empty
  for (auto&& ch : channels) {
    if (ch == 0) ch = -2;
  }

  // build header label
  std::string* name = GetStorage().GetStringRef("name");
  char label[128];
  if (!name->empty()) {
    std::snprintf(label, sizeof(label), "%s [%d]###name", name->c_str(), index);
  } else {
    std::snprintf(label, sizeof(label), "PCM[%d]###name", index);
  }

  // header
  bool open = CollapsingHeader(label);

  PopupEditName("name", name);

  ImGui::SetItemAllowOverlap();
  ImGui::SameLine();

  // show channels as LED indicators
  static const ImU32 colors[] = {IM_COL32(255, 255, 102, 255),
                                 IM_COL32(128, 128, 128, 255)};
  DrawLEDs(channels.data(), channels.size(), channels.size(), colors);

  if (open) {
    ImGui::PushItemWidth(ImGui::GetFontSize() * 4);
    model->ForEachSolenoid([&](SolenoidModel& solenoid, int j) {
      if (auto data = solenoid.GetOutputData()) {
        PushID(j);
        char solenoidName[64];
        auto& info = data->GetNameInfo();
        info.GetLabel(solenoidName, sizeof(solenoidName), "Solenoid", j);
        data->LabelText(solenoidName, "%s", channels[j] == 1 ? "On" : "Off");
        info.PopupEditName(j);
        PopID();
      }
    });
    ImGui::PopItemWidth();
  }

  return true;
}

void glass::DisplayPCMsSolenoids(PCMsModel* model, bool outputsEnabled,
                                 wpi::StringRef noneMsg) {
  bool hasAny = false;
  model->ForEachPCM([&](PCMModel& pcm, int i) {
    PushID(i);
    if (DisplayPCMSolenoids(&pcm, i, outputsEnabled)) hasAny = true;
    PopID();
  });
  if (!hasAny && !noneMsg.empty())
    ImGui::TextUnformatted(noneMsg.begin(), noneMsg.end());
}

void glass::DisplayCompressorDevice(PCMModel* model, int index,
                                    bool outputsEnabled) {
  auto compressor = model->GetCompressor();
  if (!compressor || !compressor->Exists()) return;
  DisplayCompressorDevice(compressor, index, outputsEnabled);
}

void glass::DisplayCompressorDevice(CompressorModel* model, int index,
                                    bool outputsEnabled) {
  char name[32];
  std::snprintf(name, sizeof(name), "Compressor[%d]", index);
  if (BeginDevice(name)) {
    // output enabled
    if (auto runningData = model->GetRunningData()) {
      bool value = outputsEnabled && runningData->GetValue();
      if (DeviceBoolean("Running", false, &value, runningData)) {
        model->SetRunning(value);
      }
    }

    // closed loop enabled
    if (auto enabledData = model->GetEnabledData()) {
      int value = enabledData->GetValue() ? 1 : 0;
      static const char* enabledOptions[] = {"disabled", "enabled"};
      if (DeviceEnum("Closed Loop", true, &value, enabledOptions, 2,
                     enabledData)) {
        model->SetEnabled(value != 0);
      }
    }

    // pressure switch
    if (auto pressureSwitchData = model->GetPressureSwitchData()) {
      int value = pressureSwitchData->GetValue() ? 1 : 0;
      static const char* switchOptions[] = {"full", "low"};
      if (DeviceEnum("Pressure", false, &value, switchOptions, 2,
                     pressureSwitchData)) {
        model->SetPressureSwitch(value != 0);
      }
    }

    // compressor current
    if (auto currentData = model->GetCurrentData()) {
      double value = currentData->GetValue();
      if (DeviceDouble("Current (A)", false, &value, currentData)) {
        model->SetCurrent(value);
      }
    }

    EndDevice();
  }
}

void glass::DisplayCompressorsDevice(PCMsModel* model, bool outputsEnabled) {
  model->ForEachPCM([&](PCMModel& pcm, int i) {
    DisplayCompressorDevice(&pcm, i, outputsEnabled);
  });
}
