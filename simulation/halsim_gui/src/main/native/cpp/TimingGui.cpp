/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "TimingGui.h"

#include <cstdio>
#include <cstring>
#include <vector>

#include <hal/HALBase.h>
#include <imgui.h>
#include <mockdata/MockHooks.h>
#include <mockdata/NotifierData.h>

#include "HALSimGui.h"

using namespace halsimgui;

static void DisplayTiming() {
  int32_t status = 0;
  uint64_t curTime = HAL_GetFPGATime(&status);

  if (ImGui::Button("Run")) HALSIM_ResumeTiming();
  ImGui::SameLine();
  if (ImGui::Button("Pause")) HALSIM_PauseTiming();
  ImGui::SameLine();
  ImGui::PushButtonRepeat(true);
  if (ImGui::Button("Step")) {
    HALSIM_PauseTiming();
    uint64_t nextTimeout = HALSIM_GetNextNotifierTimeout();
    if (nextTimeout != UINT64_MAX) HALSIM_StepTiming(nextTimeout - curTime);
  }
  ImGui::PopButtonRepeat();
  ImGui::PushItemWidth(ImGui::GetFontSize() * 4);
  ImGui::LabelText("FPGA Time", "%.3f", curTime / 1000000.0);
  ImGui::PopItemWidth();

  static std::vector<HALSIM_NotifierInfo> notifiers;
  int32_t num = HALSIM_GetNotifierInfo(notifiers.data(), notifiers.size());
  if (static_cast<uint32_t>(num) > notifiers.size()) {
    notifiers.resize(num);
    HALSIM_GetNotifierInfo(notifiers.data(), notifiers.size());
  }
  if (num > 0) ImGui::Separator();
  ImGui::PushItemWidth(ImGui::GetFontSize() * 4);
  for (int32_t i = 0; i < num; ++i)
    ImGui::LabelText(notifiers[i].name, "%.3f",
                     notifiers[i].timeout / 1000000.0);
  ImGui::PopItemWidth();
}

void TimingGui::Initialize() {
  HALSimGui::AddWindow("Timing", DisplayTiming,
                       ImGuiWindowFlags_AlwaysAutoResize);
  HALSimGui::SetDefaultWindowPos("Timing", 5, 150);
}
