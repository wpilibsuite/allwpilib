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

static constexpr int kDefaultColumns = 10;
static std::vector<int> numColumns;

// read/write columns setting to ini file
static void* AddressableLEDReadOpen(ImGuiContext* ctx,
                                    ImGuiSettingsHandler* handler,
                                    const char* name) {
  int num;
  if (wpi::StringRef{name}.getAsInteger(10, num)) return nullptr;
  if (num < 0) return nullptr;
  if (num >= static_cast<int>(numColumns.size()))
    numColumns.resize(num + 1, kDefaultColumns);
  return &numColumns[num];
}

static void AddressableLEDReadLine(ImGuiContext* ctx,
                                   ImGuiSettingsHandler* handler, void* entry,
                                   const char* lineStr) {
  int* cols = static_cast<int*>(entry);
  // format: columns=#
  wpi::StringRef line{lineStr};
  auto [name, value] = line.split('=');
  name = name.trim();
  value = value.trim();
  if (name == "columns") {
    int num;
    if (value.getAsInteger(10, num)) return;
    *cols = num;
  }
}

static void AddressableLEDWriteAll(ImGuiContext* ctx,
                                   ImGuiSettingsHandler* handler,
                                   ImGuiTextBuffer* out_buf) {
  for (size_t i = 0; i < numColumns.size(); ++i) {
    out_buf->appendf("[AddressableLED][%d]\ncolumns=%d\n\n",
                     static_cast<int>(i), numColumns[i]);
  }
}

static void DisplayAddressableLEDs() {
  bool hasAny = false;
  static const int numLED = HAL_GetNumAddressableLEDs();
  if (numLED > static_cast<int>(numColumns.size()))
    numColumns.resize(numLED, kDefaultColumns);

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
    ImGui::InputInt("Columns", &numColumns[i]);
    if (numColumns[i] < 1) numColumns[i] = 1;
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

    DrawLEDs(values, length, numColumns[i], colors);
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
