// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <memory>

#include "wpi/glass/MainMenuBar.hpp"
#include "wpi/glass/WindowManager.hpp"
#include "wpi/glass/networktables/NetworkTablesProvider.hpp"

#include "wpi/halsim/gui/HALProvider.hpp"

namespace halsimgui {

class HALSimGui {
 public:
  static void GlobalInit();

  static glass::MainMenuBar mainMenu;
  static std::unique_ptr<glass::WindowManager> manager;

  static std::unique_ptr<HALProvider> halProvider;
  static std::unique_ptr<glass::NetworkTablesProvider> ntProvider;
};

void AddGuiInit(std::function<void()> initialize);
void AddGuiLateExecute(std::function<void()> execute);
void AddGuiEarlyExecute(std::function<void()> execute);
void GuiExit();

}  // namespace halsimgui
