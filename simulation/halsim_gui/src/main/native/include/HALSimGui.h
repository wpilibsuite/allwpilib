// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <glass/MainMenuBar.h>
#include <glass/WindowManager.h>
#include <glass/networktables/NetworkTablesProvider.h>

#include "HALProvider.h"

namespace halsimgui {

class HALSimGui {
 public:
  static void GlobalInit();

  static glass::MainMenuBar mainMenu;
  static glass::WindowManager manager;

  static HALProvider halProvider;
  static glass::NetworkTablesProvider ntProvider;
};

}  // namespace halsimgui
