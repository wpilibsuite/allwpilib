/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "AddressableLEDGui.h"

#include <cstdio>
#include <cstring>
#include <vector>

#include <hal/Ports.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <mockdata/AddressableLEDData.h>
#include <wpi/SmallVector.h>
#include <wpi/StringRef.h>

#include "ExtraGuiWidgets.h"
#include "HALSimGui.h"

using namespace halsimgui;

namespace {
struct LEDDisplaySettings {
  int numColumns = 10;
  LEDConfig config;
};
}  // namespace

static std::vector<LEDDisplaySettings> displaySettings;

// read/write columns setting to ini file
static void* AddressableLEDReadOpen(ImGuiContext* ctx,
                                    ImGuiSettingsHandler* handler,
                                    const char* name) {
  int num;
  if (wpi::StringRef{name}.getAsInteger(10, num)) return nullptr;
  if (num < 0) return nullptr;
  if (num >= static_cast<int>(displaySettings.size()))
    displaySettings.resize(num + 1);
  return &displaySettings[num];
}

static void AddressableLEDReadLine(ImGuiContext* ctx,
                                   ImGuiSettingsHandler* handler, void* entry,
                                   const char* lineStr) {
  auto* settings = static_cast<LEDDisplaySettings*>(entry);
  // format: columns=#
  wpi::StringRef line{lineStr};
  auto [name, value] = line.split('=');
  name = name.trim();
  value = value.trim();
  if (name == "columns") {
    int num;
    if (value.getAsInteger(10, num)) return;
    settings->numColumns = num;
  } else if (name == "serpentine") {
    int num;
    if (value.getAsInteger(10, num)) return;
    settings->config.serpentine = num != 0;
  } else if (name == "order") {
    int num;
    if (value.getAsInteger(10, num)) return;
    settings->config.order = static_cast<LEDConfig::Order>(num);
  } else if (name == "start") {
    int num;
    if (value.getAsInteger(10, num)) return;
    settings->config.start = static_cast<LEDConfig::Start>(num);
  }
}

static void AddressableLEDWriteAll(ImGuiContext* ctx,
                                   ImGuiSettingsHandler* handler,
                                   ImGuiTextBuffer* out_buf) {
  for (size_t i = 0; i < displaySettings.size(); ++i) {
    out_buf->appendf(
        "[AddressableLED][%d]\ncolumns=%d\nserpentine=%d\norder=%d\n"
        "start=%d\n\n",
        static_cast<int>(i), displaySettings[i].numColumns,
        displaySettings[i].config.serpentine ? 1 : 0,
        static_cast<int>(displaySettings[i].config.order),
        static_cast<int>(displaySettings[i].config.start));
  }
}

static void DisplayAddressableLEDs() {
  bool hasAny = false;
  static const int numLED = HAL_GetNumAddressableLEDs();
  if (numLED > static_cast<int>(displaySettings.size()))
    displaySettings.resize(numLED);

  for (int i = 0; i < numLED; ++i) {
    if (!HALSIM_GetAddressableLEDInitialized(i)) continue;
    hasAny = true;

    if (numLED > 1) ImGui::Text("LEDs[%d]", i);

    static HAL_AddressableLEDData data[HAL_kAddressableLEDMaxLength];
    int length = HALSIM_GetAddressableLEDData(i, data);
    bool running = HALSIM_GetAddressableLEDRunning(i);

    ImGui::PushItemWidth(ImGui::GetFontSize() * 6);
    ImGui::LabelText("Length", "%d", length);
    ImGui::LabelText("Running", "%s", running ? "Yes" : "No");
    ImGui::InputInt("Columns", &displaySettings[i].numColumns);
    {
      static const char* options[] = {"Row Major", "Column Major"};
      int val = displaySettings[i].config.order;
      if (ImGui::Combo("Order", &val, options, 2))
        displaySettings[i].config.order = static_cast<LEDConfig::Order>(val);
    }
    {
      static const char* options[] = {"Upper Left", "Lower Left", "Upper Right",
                                      "Lower Right"};
      int val = displaySettings[i].config.start;
      if (ImGui::Combo("Start", &val, options, 4))
        displaySettings[i].config.start = static_cast<LEDConfig::Start>(val);
    }
    ImGui::Checkbox("Serpentine", &displaySettings[i].config.serpentine);
    if (displaySettings[i].numColumns < 1) displaySettings[i].numColumns = 1;
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

    DrawLEDs(values, length, displaySettings[i].numColumns, colors, 0, 0,
             displaySettings[i].config);
  }
  if (!hasAny) ImGui::Text("No addressable LEDs");
}

void AddressableLEDGui::Initialize() {
  // hook ini handler to save columns settings
  ImGuiSettingsHandler iniHandler;
  iniHandler.TypeName = "AddressableLED";
  iniHandler.TypeHash = ImHashStr(iniHandler.TypeName);
  iniHandler.ReadOpenFn = AddressableLEDReadOpen;
  iniHandler.ReadLineFn = AddressableLEDReadLine;
  iniHandler.WriteAllFn = AddressableLEDWriteAll;
  ImGui::GetCurrentContext()->SettingsHandlers.push_back(iniHandler);

  HALSimGui::AddWindow("Addressable LEDs", DisplayAddressableLEDs,
                       ImGuiWindowFlags_AlwaysAutoResize);
  HALSimGui::SetWindowVisibility("Addressable LEDs", HALSimGui::kHide);
  HALSimGui::SetDefaultWindowPos("Addressable LEDs", 290, 100);
}
