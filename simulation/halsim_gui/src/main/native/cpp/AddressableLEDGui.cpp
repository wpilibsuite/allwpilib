/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "AddressableLEDGui.h"

#include <hal/Ports.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <mockdata/AddressableLEDData.h>
#include <wpi/SmallVector.h>
#include <wpi/StringRef.h>

#include "ExtraGuiWidgets.h"
#include "HALSimGui.h"
#include "IniSaver.h"
#include "IniSaverInfo.h"

using namespace halsimgui;

namespace {
struct LEDDisplayInfo {
  int numColumns = 10;
  LEDConfig config;

  bool ReadIni(wpi::StringRef name, wpi::StringRef value);
  void WriteIni(ImGuiTextBuffer* out);
};
}  // namespace

static IniSaver<LEDDisplayInfo> gDisplaySettings{"AddressableLED"};

bool LEDDisplayInfo::ReadIni(wpi::StringRef name, wpi::StringRef value) {
  if (name == "columns") {
    int num;
    if (value.getAsInteger(10, num)) return true;
    numColumns = num;
  } else if (name == "serpentine") {
    int num;
    if (value.getAsInteger(10, num)) return true;
    config.serpentine = num != 0;
  } else if (name == "order") {
    int num;
    if (value.getAsInteger(10, num)) return true;
    config.order = static_cast<LEDConfig::Order>(num);
  } else if (name == "start") {
    int num;
    if (value.getAsInteger(10, num)) return true;
    config.start = static_cast<LEDConfig::Start>(num);
  } else {
    return false;
  }
  return true;
}

void LEDDisplayInfo::WriteIni(ImGuiTextBuffer* out) {
  out->appendf("columns=%d\nserpentine=%d\norder=%d\nstart=%d\n", numColumns,
               config.serpentine ? 1 : 0, static_cast<int>(config.order),
               static_cast<int>(config.start));
}

static void DisplayAddressableLEDs() {
  bool hasAny = false;
  static const int numLED = HAL_GetNumAddressableLEDs();

  for (int i = 0; i < numLED; ++i) {
    if (!HALSIM_GetAddressableLEDInitialized(i)) continue;
    hasAny = true;

    if (numLED > 1) ImGui::Text("LEDs[%d]", i);

    static HAL_AddressableLEDData data[HAL_kAddressableLEDMaxLength];
    int length = HALSIM_GetAddressableLEDData(i, data);
    bool running = HALSIM_GetAddressableLEDRunning(i);
    auto& info = gDisplaySettings[i];

    ImGui::PushItemWidth(ImGui::GetFontSize() * 6);
    ImGui::LabelText("Length", "%d", length);
    ImGui::LabelText("Running", "%s", running ? "Yes" : "No");
    ImGui::InputInt("Columns", &info.numColumns);
    {
      static const char* options[] = {"Row Major", "Column Major"};
      int val = info.config.order;
      if (ImGui::Combo("Order", &val, options, 2))
        info.config.order = static_cast<LEDConfig::Order>(val);
    }
    {
      static const char* options[] = {"Upper Left", "Lower Left", "Upper Right",
                                      "Lower Right"};
      int val = info.config.start;
      if (ImGui::Combo("Start", &val, options, 4))
        info.config.start = static_cast<LEDConfig::Start>(val);
    }
    ImGui::Checkbox("Serpentine", &info.config.serpentine);
    if (info.numColumns < 1) info.numColumns = 1;
    ImGui::PopItemWidth();

    // show as LED indicators
    static int values[HAL_kAddressableLEDMaxLength];
    static ImU32 colors[HAL_kAddressableLEDMaxLength];

    if (!running) {
      colors[0] = IM_COL32(128, 128, 128, 255);
      for (int j = 0; j < length; ++j) values[j] = -1;
    } else {
      for (int j = 0; j < length; ++j) {
        values[j] = j + 1;
        colors[j] = IM_COL32(data[j].r, data[j].g, data[j].b, 255);
      }
    }

    DrawLEDs(values, length, info.numColumns, colors, 0, 0, info.config);
  }
  if (!hasAny) ImGui::Text("No addressable LEDs");
}

void AddressableLEDGui::Initialize() {
  gDisplaySettings.Initialize();
  HALSimGui::AddWindow("Addressable LEDs", DisplayAddressableLEDs,
                       ImGuiWindowFlags_AlwaysAutoResize);
  HALSimGui::SetWindowVisibility("Addressable LEDs", HALSimGui::kHide);
  HALSimGui::SetDefaultWindowPos("Addressable LEDs", 290, 100);
}
