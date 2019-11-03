/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "EncoderGui.h"

#include <cstdio>

#include <hal/Ports.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <mockdata/EncoderData.h>
#include <mockdata/SimDeviceData.h>
#include <wpi/DenseMap.h>
#include <wpi/StringRef.h>

#include "HALSimGui.h"

using namespace halsimgui;

static wpi::DenseMap<int, bool> gEncodersOpen;  // indexed by channel A

// read/write open state to ini file
static void* EncodersReadOpen(ImGuiContext* ctx, ImGuiSettingsHandler* handler,
                              const char* name) {
  int num;
  if (wpi::StringRef{name}.getAsInteger(10, num)) return nullptr;
  return &gEncodersOpen[num];
}

static void EncodersReadLine(ImGuiContext* ctx, ImGuiSettingsHandler* handler,
                             void* entry, const char* lineStr) {
  bool* element = static_cast<bool*>(entry);
  wpi::StringRef line{lineStr};
  auto [name, value] = line.split('=');
  name = name.trim();
  value = value.trim();
  if (name == "open") {
    int num;
    if (value.getAsInteger(10, num)) return;
    *element = num;
  }
}

static void EncodersWriteAll(ImGuiContext* ctx, ImGuiSettingsHandler* handler,
                             ImGuiTextBuffer* out_buf) {
  for (auto it : gEncodersOpen)
    out_buf->appendf("[Encoder][%d]\nopen=%d\n\n", it.first, it.second ? 1 : 0);
}

static void DisplayEncoders() {
  bool hasAny = false;
  static int numEncoder = HAL_GetNumEncoders();
  ImGui::PushItemWidth(ImGui::GetFontSize() * 8);
  for (int i = 0; i < numEncoder; ++i) {
    if (HALSIM_GetEncoderInitialized(i)) {
      hasAny = true;
      char name[32];
      int chA = HALSIM_GetEncoderDigitalChannelA(i);
      int chB = HALSIM_GetEncoderDigitalChannelB(i);
      std::snprintf(name, sizeof(name), "Encoder[%d,%d]", chA, chB);
      if (auto simDevice = HALSIM_GetEncoderSimDevice(i)) {
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(96, 96, 96, 255));
        ImGui::Text("%s", HALSIM_GetSimDeviceName(simDevice));
        ImGui::PopStyleColor();
      } else if (ImGui::CollapsingHeader(
                     name,
                     gEncodersOpen[chA] ? ImGuiTreeNodeFlags_DefaultOpen : 0)) {
        gEncodersOpen[chA] = true;

        ImGui::PushID(i);

        // distance per pulse
        double distancePerPulse = HALSIM_GetEncoderDistancePerPulse(i);
        ImGui::LabelText("Dist/Count", "%.6f", distancePerPulse);

        // count
        int count = HALSIM_GetEncoderCount(i);
        if (ImGui::InputInt("Count", &count)) HALSIM_SetEncoderCount(i, count);
        ImGui::SameLine();
        if (ImGui::Button("Reset")) HALSIM_SetEncoderCount(i, 0);

        // max period
        double maxPeriod = HALSIM_GetEncoderMaxPeriod(i);
        ImGui::LabelText("Max Period", "%.6f", maxPeriod);

        // period
        double period = HALSIM_GetEncoderPeriod(i);
        if (ImGui::InputDouble("Period", &period, 0, 0, "%.6g"))
          HALSIM_SetEncoderPeriod(i, period);

        // reverse direction
        ImGui::LabelText(
            "Reverse Direction", "%s",
            HALSIM_GetEncoderReverseDirection(i) ? "true" : "false");

        // direction
        static const char* options[] = {"reverse", "forward"};
        int direction = HALSIM_GetEncoderDirection(i) ? 1 : 0;
        if (ImGui::Combo("Direction", &direction, options, 2))
          HALSIM_SetEncoderDirection(i, direction);

        ImGui::PopID();
      } else {
        gEncodersOpen[chA] = false;
      }
    }
  }
  ImGui::PopItemWidth();
  if (!hasAny) ImGui::Text("No encoders");
}

void EncoderGui::Initialize() {
  // hook ini handler to save settings
  ImGuiSettingsHandler iniHandler;
  iniHandler.TypeName = "Encoder";
  iniHandler.TypeHash = ImHashStr(iniHandler.TypeName);
  iniHandler.ReadOpenFn = EncodersReadOpen;
  iniHandler.ReadLineFn = EncodersReadLine;
  iniHandler.WriteAllFn = EncodersWriteAll;
  ImGui::GetCurrentContext()->SettingsHandlers.push_back(iniHandler);

  HALSimGui::AddWindow("Encoders", DisplayEncoders,
                       ImGuiWindowFlags_AlwaysAutoResize);
  HALSimGui::SetDefaultWindowPos("Encoders", 640, 215);
}
