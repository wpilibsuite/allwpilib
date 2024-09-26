// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "HALSimGui.h"

#include <memory>
#include <utility>

#include <glass/Context.h>
#include <glass/Storage.h>
#include <imgui.h>
#include <wpigui.h>

using namespace halsimgui;

glass::MainMenuBar HALSimGui::mainMenu;
std::unique_ptr<glass::WindowManager> HALSimGui::manager;
std::unique_ptr<HALProvider> HALSimGui::halProvider;
std::unique_ptr<glass::NetworkTablesProvider> HALSimGui::ntProvider;

void HALSimGui::GlobalInit() {
  manager = std::make_unique<glass::WindowManager>(
      glass::GetStorageRoot().GetChild("SimWindow"));
  manager->GlobalInit();
  halProvider = std::make_unique<HALProvider>(
      glass::GetStorageRoot().GetChild("HALProvider"));
  halProvider->GlobalInit();
  ntProvider = std::make_unique<glass::NetworkTablesProvider>(
      glass::GetStorageRoot().GetChild("NTProvider"));
  ntProvider->GlobalInit();

  wpi::gui::AddLateExecute([] { mainMenu.Display(); });

  glass::AddStandardNetworkTablesViews(*ntProvider);
}

namespace halsimgui {

void AddGuiInit(std::function<void()> initialize) {
  wpi::gui::AddInit(std::move(initialize));
}

void AddGuiEarlyExecute(std::function<void()> execute) {
  wpi::gui::AddEarlyExecute(std::move(execute));
}

void AddGuiLateExecute(std::function<void()> execute) {
  wpi::gui::AddLateExecute(std::move(execute));
}

void GuiExit() {
  wpi::gui::Exit();
}

}  // namespace halsimgui
