/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "EncoderGui.h"

#include <hal/Ports.h>
#include <imgui.h>
#include <mockdata/EncoderData.h>
#include <mockdata/SimDeviceData.h>

#include "HALSimGui.h"
#include "IniSaver.h"
#include "IniSaverInfo.h"

using namespace halsimgui;

namespace {
struct EncoderInfo : public NameInfo, public OpenInfo {
  bool ReadIni(wpi::StringRef name, wpi::StringRef value) {
    if (NameInfo::ReadIni(name, value)) return true;
    if (OpenInfo::ReadIni(name, value)) return true;
    return false;
  }
  void WriteIni(ImGuiTextBuffer* out) {
    NameInfo::WriteIni(out);
    OpenInfo::WriteIni(out);
  }
};
}  // namespace

static IniSaver<EncoderInfo> gEncoders{"Encoder"};  // indexed by channel A

static void DisplayEncoders() {
  bool hasAny = false;
  static int numEncoder = HAL_GetNumEncoders();
  ImGui::PushItemWidth(ImGui::GetFontSize() * 8);
  for (int i = 0; i < numEncoder; ++i) {
    if (HALSIM_GetEncoderInitialized(i)) {
      hasAny = true;
      if (auto simDevice = HALSIM_GetEncoderSimDevice(i)) {
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(96, 96, 96, 255));
        ImGui::Text("%s", HALSIM_GetSimDeviceName(simDevice));
        ImGui::PopStyleColor();
      } else {
        int chA = HALSIM_GetEncoderDigitalChannelA(i);
        int chB = HALSIM_GetEncoderDigitalChannelB(i);

        // build header name
        auto& info = gEncoders[chA];
        char name[128];
        info.GetName(name, sizeof(name), "Encoder", chA, chB);

        // header
        bool open = ImGui::CollapsingHeader(
            name, gEncoders[chA].IsOpen() ? ImGuiTreeNodeFlags_DefaultOpen : 0);
        info.SetOpen(open);

        // context menu to change name
        info.PopupEditName(chA);

        if (open) {
          ImGui::PushID(i);
          // distance per pulse
          double distancePerPulse = HALSIM_GetEncoderDistancePerPulse(i);
          ImGui::LabelText("Dist/Count", "%.6f", distancePerPulse);

          // count
          int count = HALSIM_GetEncoderCount(i);
          if (ImGui::InputInt("Count", &count))
            HALSIM_SetEncoderCount(i, count);
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
        }
      }
    }
  }
  ImGui::PopItemWidth();
  if (!hasAny) ImGui::Text("No encoders");
}

void EncoderGui::Initialize() {
  gEncoders.Initialize();
  HALSimGui::AddWindow("Encoders", DisplayEncoders,
                       ImGuiWindowFlags_AlwaysAutoResize);
  HALSimGui::SetDefaultWindowPos("Encoders", 640, 215);
}
