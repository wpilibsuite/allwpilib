// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <memory>

#include <glass/MainMenuBar.h>
#include <glass/WindowManager.h>
#include <glass/networktables/NetworkTablesProvider.h>
#include <hal/SimDevice.h>

namespace glass {
class DataSource;
class DeviceTreeModel;
}  // namespace glass

namespace halsimgui {

class HALSimGui {
 public:
  static void GlobalInit();

  static glass::MainMenuBar mainMenu;
  static std::unique_ptr<glass::WindowManager> manager;

  static std::unique_ptr<glass::NetworkTablesProvider> ntProvider;
};

void DisplayDisableMenuItem();

/**
 * Returns true if outputs are disabled.
 *
 * @return true if outputs are disabled, false otherwise.
 */
bool AreOutputsDisabled();

/**
 * Returns true if outputs are enabled.
 *
 * @return true if outputs are enabled, false otherwise.
 */
inline bool AreOutputsEnabled() {
  return !AreOutputsDisabled();
}

glass::DataSource* GetDeviceValueSource(HAL_SimValueHandle handle);
glass::DeviceTreeModel& GetDeviceTree();

void AddGuiInit(std::function<void()> initialize);
void AddGuiLateExecute(std::function<void()> execute);
void AddGuiEarlyExecute(std::function<void()> execute);
void GuiExit();

}  // namespace halsimgui
