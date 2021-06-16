// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <glass/WindowManager.h>

#include <string_view>

namespace halsimgui {

class DSManager : public glass::WindowManager {
 public:
  explicit DSManager(std::string_view iniName) : WindowManager{iniName} {}

  void DisplayMenu() override;
};

class DriverStationGui {
 public:
  static void GlobalInit();
  static void SetDSSocketExtension(void* data);

  static DSManager dsManager;
};

}  // namespace halsimgui
