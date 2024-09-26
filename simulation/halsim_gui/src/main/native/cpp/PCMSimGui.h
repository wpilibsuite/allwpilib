// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once
#include <memory>

#include <glass/hardware/Pneumatic.h>

namespace halsimgui {

class PCMSimGui {
 public:
  static void Initialize();
  static bool PCMsAnyInitialized();
  static bool PCMsAnySolenoids(glass::PneumaticControlsModel* model);
  static std::unique_ptr<glass::PneumaticControlsModel> GetPCMsModel();
};

}  // namespace halsimgui
