// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "TimingGui.hpp"

#include <cstdio>
#include <cstring>
#include <memory>
#include <vector>

#include <imgui.h>

#include "wpi/glass/Model.hpp"
#include "wpi/glass/View.hpp"
#include "wpi/hal/HALBase.h"
#include "wpi/hal/simulation/MockHooks.h"
#include "wpi/hal/simulation/NotifierData.h"
#include "wpi/halsim/gui/HALSimGui.hpp"

using namespace halsimgui;

namespace {
class TimingModel : public wpi::glass::Model {
 public:
  void Update() override {}
  bool Exists() override { return true; }
};
}  // namespace

static void DisplayTiming() {
  int32_t status = 0;
  uint64_t curTime = HAL_GetFPGATime(&status);

  if (ImGui::Button("Run")) {
    HALSIM_ResumeTiming();
  }
  ImGui::SameLine();
  if (ImGui::Button("Pause")) {
    HALSIM_PauseTiming();
  }
  ImGui::SameLine();
  ImGui::PushButtonRepeat(true);
  if (ImGui::Button("Step")) {
    HALSIM_PauseTiming();
    uint64_t nextTimeout = HALSIM_GetNextNotifierTimeout();
    if (nextTimeout != UINT64_MAX) {
      HALSIM_StepTimingAsync(nextTimeout - curTime);
    }
  }
  ImGui::PopButtonRepeat();
  ImGui::PushItemWidth(ImGui::GetFontSize() * 4);
  ImGui::LabelText("FPGA Time", "%.3f", curTime / 1000000.0);
  ImGui::PopItemWidth();

  static std::vector<HALSIM_NotifierInfo> notifiers;
  struct NotifierLastValue {
    uint64_t alarmTime = UINT64_MAX;
    double displayTime = 0;
  };
  constexpr double fadeDuration = 0.25;
  double curGuiTime = ImGui::GetTime();

  static std::vector<NotifierLastValue> notifierFades;
  int32_t num = HALSIM_GetNotifierInfo(notifiers.data(), notifiers.size());
  if (static_cast<uint32_t>(num) > notifiers.size()) {
    notifiers.resize(num);
    notifierFades.resize(num);
    num = HALSIM_GetNotifierInfo(notifiers.data(), notifiers.size());
  }
  for (int32_t i = 0; i < num; ++i) {
    if (notifiers[i].alarmTime != UINT64_MAX) {
      notifierFades[i].alarmTime = notifiers[i].alarmTime;
      notifierFades[i].displayTime = curGuiTime;
    } else if (curGuiTime > (notifierFades[i].displayTime + fadeDuration)) {
      notifierFades[i].alarmTime = UINT64_MAX;
      notifierFades[i].displayTime = 0;
    }
  }

  if (num > 0) {
    ImGui::Separator();
  }
  if (ImGui::BeginTable("Notifiers", 4, ImGuiTableFlags_Borders)) {
    ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableSetupColumn("Alarm", ImGuiTableColumnFlags_WidthStretch |
                                         ImGuiTableColumnFlags_DefaultSort);
    ImGui::TableSetupColumn("Interval", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableSetupColumn("Overruns", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableHeadersRow();
    for (int32_t i = 0; i < num; ++i) {
      ImGui::TableNextRow();
      ImGui::TableNextColumn();
      ImGui::TextUnformatted(notifiers[i].name);
      ImGui::TableNextColumn();
      if (notifierFades[i].alarmTime != UINT64_MAX) {
        ImGui::PushStyleVar(
            ImGuiStyleVar_Alpha,
            1.0 - (curGuiTime - notifierFades[i].displayTime) / fadeDuration);
        ImGui::Text("%.3f", notifierFades[i].alarmTime / 1000000.0);
        ImGui::PopStyleVar();
      }
      ImGui::TableNextColumn();
      if (notifiers[i].intervalTime != 0) {
        ImGui::Text("%.3f", notifiers[i].intervalTime / 1000000.0);
      }
      ImGui::TableNextColumn();
      ImGui::Text("%d", notifiers[i].overrunCount);
    }
    ImGui::EndTable();
  }
}

void TimingGui::Initialize() {
  HALSimGui::halProvider->Register(
      "Timing", [] { return true; },
      [] { return std::make_unique<TimingModel>(); },
      [](wpi::glass::Window* win, wpi::glass::Model* model) {
        win->DisableRenamePopup();
        win->SetFlags(ImGuiWindowFlags_AlwaysAutoResize);
        win->SetDefaultPos(5, 150);
        return wpi::glass::MakeFunctionView(DisplayTiming);
      });
  HALSimGui::halProvider->ShowDefault("Timing");
}
