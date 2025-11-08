// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once
#include <memory>

#include <glass/hardware/Pneumatic.h>

namespace halsimgui {

class PHSimGui {
 public:
  static void Initialize();
  static bool PHsAnyInitialized();
  static bool PHsAnySolenoids(glass::PneumaticControlsModel* model);
  static std::unique_ptr<glass::PneumaticControlsModel> GetPHsModel();
};

}  // namespace halsimgui
