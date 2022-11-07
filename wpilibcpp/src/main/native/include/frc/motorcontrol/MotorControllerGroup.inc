// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <vector>

#include "frc/motorcontrol/MotorControllerGroup.h"

namespace frc {

template <class... MotorControllers>
MotorControllerGroup::MotorControllerGroup(
    MotorController& motorController, MotorControllers&... motorControllers)
    : m_motorControllers(std::vector<std::reference_wrapper<MotorController>>{
          motorController, motorControllers...}) {
  Initialize();
}

}  // namespace frc
