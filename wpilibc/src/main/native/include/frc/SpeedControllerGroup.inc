/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

namespace frc {

template <class... SpeedControllers>
SpeedControllerGroup::SpeedControllerGroup(
    SpeedController& speedController, SpeedControllers&... speedControllers)
    : m_speedControllers{speedController, speedControllers...} {
  for (auto& speedController : m_speedControllers)
    AddChild(&speedController.get());
  static int instances = 0;
  ++instances;
  SetName("SpeedControllerGroup", instances);
}

}  // namespace frc
