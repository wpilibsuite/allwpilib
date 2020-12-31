// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <vector>

#include "frc/SpeedControllerGroup.h"

namespace frc {

template <class... SpeedControllers>
SpeedControllerGroup::SpeedControllerGroup(
    SpeedController& speedController, SpeedControllers&... speedControllers)
    : m_speedControllers(std::vector<std::reference_wrapper<SpeedController>>{
          speedController, speedControllers...}) {
  Initialize();
}

}  // namespace frc
