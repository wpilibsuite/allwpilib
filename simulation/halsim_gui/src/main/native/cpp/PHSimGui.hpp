// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once
#include <memory>

#include <wpi/glass/hardware/Pneumatic.hpp>

namespace halsimgui {

class PHSimGui {
 public:
  static void Initialize();
  static bool PHsAnyInitialized();
  static bool PHsAnySolenoids(wpi::glass::PneumaticControlsModel* model);
  static std::unique_ptr<wpi::glass::PneumaticControlsModel> GetPHsModel();
};

}  // namespace halsimgui
