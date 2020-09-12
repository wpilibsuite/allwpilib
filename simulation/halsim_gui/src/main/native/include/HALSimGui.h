/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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
