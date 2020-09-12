/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <glass/WindowManager.h>

namespace halsimgui {

class DSManager : public glass::WindowManager {
 public:
  explicit DSManager(const wpi::Twine& iniName) : WindowManager{iniName} {}

  void DisplayMenu() override;
};

class DriverStationGui {
 public:
  static void GlobalInit();
  static void SetDSSocketExtension(void* data);

  static DSManager dsManager;
};

}  // namespace halsimgui
