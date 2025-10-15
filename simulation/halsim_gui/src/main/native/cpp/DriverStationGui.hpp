// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <string_view>

#include <glass/WindowManager.h>

namespace halsimgui {

class DSManager : public glass::WindowManager {
 public:
  explicit DSManager(glass::Storage& storage) : WindowManager{storage} {}

  void DisplayMenu() override;
};

class DriverStationGui {
 public:
  static void GlobalInit();
  static void SetDSSocketExtension(void* data);

  static std::unique_ptr<DSManager> dsManager;
};

}  // namespace halsimgui
