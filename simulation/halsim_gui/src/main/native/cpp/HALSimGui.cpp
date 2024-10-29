// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "HALSimGui.h"

#include <memory>
#include <utility>
#include "glass/MainMenuBar.h"

#include <glass/Context.h>
#include <glass/Storage.h>
#include <hal/simulation/DriverStationData.h>
#include <imgui.h>
#include <wpigui.h>

using namespace halsimgui;

glass::MainMenuBar HALSimGui::mainMenu;
static bool gDisableOutputsOnDSDisable = true;

std::unique_ptr<glass::WindowManager> HALSimGui::manager;
std::unique_ptr<glass::NetworkTablesProvider> HALSimGui::ntProvider;

void HALSimGui::GlobalInit() {
  manager = std::make_unique<glass::WindowManager>(
      glass::GetStorageRoot().GetChild("SimWindow"));
  manager->GlobalInit();
  ntProvider = std::make_unique<glass::NetworkTablesProvider>(
      glass::GetStorageRoot().GetChild("NTProvider"));
  ntProvider->GlobalInit();

  wpi::gui::AddLateExecute([] { mainMenu.Display(); });

  glass::AddStandardNetworkTablesViews(*ntProvider);
}

void halsimgui::DisplayDisableMenuItem() {
  ImGui::MenuItem("Disable outputs on DS disable", nullptr,
                  &gDisableOutputsOnDSDisable, true);
}

bool halsimgui::AreOutputsDisabled() {
  return gDisableOutputsOnDSDisable && !HALSIM_GetDriverStationEnabled();
}

void halsimgui::AddGuiInit(std::function<void()> initialize) {
  wpi::gui::AddInit(std::move(initialize));
}

void halsimgui::AddGuiEarlyExecute(std::function<void()> execute) {
  wpi::gui::AddEarlyExecute(std::move(execute));
}

void halsimgui::AddGuiLateExecute(std::function<void()> execute) {
  wpi::gui::AddLateExecute(std::move(execute));
}

void halsimgui::GuiExit() {
  wpi::gui::Exit();
}
