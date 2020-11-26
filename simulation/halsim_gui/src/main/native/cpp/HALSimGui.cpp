/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HALSimGui.h"

#include <algorithm>

#include <hal/simulation/DriverStationData.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <wpi/StringMap.h>
#include <wpi/raw_ostream.h>
#include <wpigui.h>

using namespace halsimgui;

namespace gui = wpi::gui;

namespace {
struct WindowInfo {
  WindowInfo() = default;
  explicit WindowInfo(const char* name_) : name{name_} {}
  WindowInfo(const char* name_, std::function<void()> display_,
             ImGuiWindowFlags flags_)
      : name{name_}, display{std::move(display_)}, flags{flags_} {}

  std::string name;
  std::function<void()> display;
  ImGuiWindowFlags flags = 0;
  bool visible = true;
  bool enabled = true;
  ImGuiCond posCond = 0;
  ImGuiCond sizeCond = 0;
  ImVec2 pos;
  ImVec2 size;
  bool setPadding = false;
  ImVec2 padding;
};
}  // namespace

static std::vector<WindowInfo> gWindows;
static wpi::StringMap<int> gWindowMap;   // index into gWindows
static std::vector<int> gSortedWindows;  // index into gWindows
static std::vector<std::function<void()>> gOptionMenus;
static std::vector<std::function<void()>> gMenus;
static bool gDisableOutputsOnDSDisable = true;

// read/write open state to ini file
static void* SimWindowsReadOpen(ImGuiContext* ctx,
                                ImGuiSettingsHandler* handler,
                                const char* name) {
  if (wpi::StringRef{name} == "GLOBAL") return &gDisableOutputsOnDSDisable;

  int index = gWindowMap.try_emplace(name, gWindows.size()).first->second;
  if (index == static_cast<int>(gWindows.size())) {
    gSortedWindows.push_back(index);
    gWindows.emplace_back(name);
    std::sort(gSortedWindows.begin(), gSortedWindows.end(),
              [](int a, int b) { return gWindows[a].name < gWindows[b].name; });
  }
  return &gWindows[index];
}

static void SimWindowsReadLine(ImGuiContext* ctx, ImGuiSettingsHandler* handler,
                               void* entry, const char* lineStr) {
  wpi::StringRef line{lineStr};
  auto [name, value] = line.split('=');
  name = name.trim();
  value = value.trim();

  if (entry == &gDisableOutputsOnDSDisable) {
    int num;
    if (value.getAsInteger(10, num)) return;
    if (name == "disableOutputsOnDS") {
      gDisableOutputsOnDSDisable = num;
    }
    return;
  }

  auto element = static_cast<WindowInfo*>(entry);
  if (name == "visible") {
    int num;
    if (value.getAsInteger(10, num)) return;
    element->visible = num;
  } else if (name == "enabled") {
    int num;
    if (value.getAsInteger(10, num)) return;
    element->enabled = num;
  }
}

static void SimWindowsWriteAll(ImGuiContext* ctx, ImGuiSettingsHandler* handler,
                               ImGuiTextBuffer* out_buf) {
  out_buf->appendf("[SimWindow][GLOBAL]\ndisableOutputsOnDS=%d\n\n",
                   gDisableOutputsOnDSDisable ? 1 : 0);
  for (auto&& window : gWindows)
    out_buf->appendf("[SimWindow][%s]\nvisible=%d\nenabled=%d\n\n",
                     window.name.c_str(), window.visible ? 1 : 0,
                     window.enabled ? 1 : 0);
}

void HALSimGui::Add(std::function<void()> initialize) {
  gui::AddInit(std::move(initialize));
}

void HALSimGui::AddExecute(std::function<void()> execute) {
  gui::AddEarlyExecute(std::move(execute));
}

void HALSimGui::AddWindow(const char* name, std::function<void()> display,
                          int flags) {
  if (display) {
    int index = gWindowMap.try_emplace(name, gWindows.size()).first->second;
    if (index < static_cast<int>(gWindows.size())) {
      if (gWindows[index].display) {
        wpi::errs() << "halsim_gui: ignoring duplicate window '" << name
                    << "'\n";
      } else {
        gWindows[index].display = display;
        gWindows[index].flags = flags;
      }
      return;
    }
    gSortedWindows.push_back(index);
    gWindows.emplace_back(name, std::move(display),
                          static_cast<ImGuiWindowFlags>(flags));
    std::sort(gSortedWindows.begin(), gSortedWindows.end(),
              [](int a, int b) { return gWindows[a].name < gWindows[b].name; });
  }
}

void HALSimGui::AddMainMenu(std::function<void()> menu) {
  if (menu) gMenus.emplace_back(std::move(menu));
}

void HALSimGui::AddOptionMenu(std::function<void()> menu) {
  if (menu) gOptionMenus.emplace_back(std::move(menu));
}

void HALSimGui::SetWindowVisibility(const char* name,
                                    WindowVisibility visibility) {
  auto it = gWindowMap.find(name);
  if (it == gWindowMap.end()) return;
  auto& window = gWindows[it->second];
  switch (visibility) {
    case kHide:
      window.visible = false;
      window.enabled = true;
      break;
    case kShow:
      window.visible = true;
      window.enabled = true;
      break;
    case kDisabled:
      window.enabled = false;
      break;
  }
}

void HALSimGui::SetDefaultWindowPos(const char* name, float x, float y) {
  auto it = gWindowMap.find(name);
  if (it == gWindowMap.end()) return;
  auto& window = gWindows[it->second];
  window.posCond = ImGuiCond_FirstUseEver;
  window.pos = ImVec2{x, y};
}

void HALSimGui::SetDefaultWindowSize(const char* name, float width,
                                     float height) {
  auto it = gWindowMap.find(name);
  if (it == gWindowMap.end()) return;
  auto& window = gWindows[it->second];
  window.sizeCond = ImGuiCond_FirstUseEver;
  window.size = ImVec2{width, height};
}

void HALSimGui::SetWindowPadding(const char* name, float x, float y) {
  auto it = gWindowMap.find(name);
  if (it == gWindowMap.end()) return;
  auto& window = gWindows[it->second];
  window.setPadding = true;
  window.padding = ImVec2{x, y};
}

bool HALSimGui::AreOutputsDisabled() {
  return gDisableOutputsOnDSDisable && !HALSIM_GetDriverStationEnabled();
}

void HALSimGui::GlobalInit() { gui::CreateContext(); }

bool HALSimGui::Initialize() {
  gui::AddInit([] {
    // Hook ini handler to save settings
    ImGuiSettingsHandler iniHandler;
    iniHandler.TypeName = "SimWindow";
    iniHandler.TypeHash = ImHashStr(iniHandler.TypeName);
    iniHandler.ReadOpenFn = SimWindowsReadOpen;
    iniHandler.ReadLineFn = SimWindowsReadLine;
    iniHandler.WriteAllFn = SimWindowsWriteAll;
    ImGui::GetCurrentContext()->SettingsHandlers.push_back(iniHandler);
  });

  gui::AddWindowScaler([](float windowScale) {
    // scale default window positions
    for (auto&& window : gWindows) {
      if ((window.posCond & ImGuiCond_FirstUseEver) != 0) {
        window.pos.x *= windowScale;
        window.pos.y *= windowScale;
        window.size.x *= windowScale;
        window.size.y *= windowScale;
      }
    }
  });

  gui::AddLateExecute([] {
    {
      ImGui::BeginMainMenuBar();

      if (ImGui::BeginMenu("Options")) {
        ImGui::MenuItem("Disable outputs on DS disable", nullptr,
                        &gDisableOutputsOnDSDisable, true);
        for (auto&& menu : gOptionMenus) {
          if (menu) menu();
        }
        ImGui::EndMenu();
      }

      gui::EmitViewMenu();

      if (ImGui::BeginMenu("Window")) {
        for (auto&& windowIndex : gSortedWindows) {
          auto& window = gWindows[windowIndex];
          ImGui::MenuItem(window.name.c_str(), nullptr, &window.visible,
                          window.enabled);
        }
        ImGui::EndMenu();
      }

      for (auto&& menu : gMenus) {
        if (menu) menu();
      }

#if 0
      char str[64];
      std::snprintf(str, sizeof(str), "%.3f ms/frame (%.1f FPS)",
                    1000.0f / ImGui::GetIO().Framerate,
                    ImGui::GetIO().Framerate);
      ImGui::SameLine(ImGui::GetWindowWidth() - ImGui::CalcTextSize(str).x -
                      10);
      ImGui::Text("%s", str);
#endif
      ImGui::EndMainMenuBar();
    }

    for (auto&& window : gWindows) {
      if (window.display && window.visible && window.enabled) {
        if (window.posCond != 0)
          ImGui::SetNextWindowPos(window.pos, window.posCond);
        if (window.sizeCond != 0)
          ImGui::SetNextWindowSize(window.size, window.sizeCond);
        if (window.setPadding)
          ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, window.padding);
        if (ImGui::Begin(window.name.c_str(), &window.visible, window.flags))
          window.display();
        ImGui::End();
        if (window.setPadding) ImGui::PopStyleVar();
      }
    }
  });

  if (!gui::Initialize("Simulation GUI", 1280, 720)) return false;

  return true;
}

void HALSimGui::Main(void*) {
  gui::Main();
  gui::DestroyContext();
}

void HALSimGui::Exit(void*) { gui::Exit(); }

extern "C" {

void HALSIMGUI_Add(void* param, void (*initialize)(void*)) {
  if (initialize) {
    HALSimGui::Add([=] { initialize(param); });
  }
}

void HALSIMGUI_AddExecute(void* param, void (*execute)(void*)) {
  if (execute) {
    HALSimGui::AddExecute([=] { execute(param); });
  }
}

void HALSIMGUI_AddWindow(const char* name, void* param, void (*display)(void*),
                         int32_t flags) {
  if (display) {
    HALSimGui::AddWindow(
        name, [=] { display(param); }, flags);
  }
}

void HALSIMGUI_AddMainMenu(void* param, void (*menu)(void*)) {
  if (menu) {
    HALSimGui::AddMainMenu([=] { menu(param); });
  }
}

void HALSIMGUI_AddOptionMenu(void* param, void (*menu)(void*)) {
  if (menu) {
    HALSimGui::AddOptionMenu([=] { menu(param); });
  }
}

void HALSIMGUI_SetWindowVisibility(const char* name, int32_t visibility) {
  HALSimGui::SetWindowVisibility(
      name, static_cast<HALSimGui::WindowVisibility>(visibility));
}

void HALSIMGUI_SetDefaultWindowPos(const char* name, float x, float y) {
  HALSimGui::SetDefaultWindowPos(name, x, y);
}

void HALSIMGUI_SetDefaultWindowSize(const char* name, float width,
                                    float height) {
  HALSimGui::SetDefaultWindowSize(name, width, height);
}

void HALSIMGUI_SetWindowPadding(const char* name, float x, float y) {
  HALSimGui::SetDefaultWindowSize(name, x, y);
}

int HALSIMGUI_AreOutputsDisabled(void) {
  return HALSimGui::AreOutputsDisabled();
}

}  // extern "C"
