/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/shuffleboard/Shuffleboard.h"

#include <networktables/NetworkTableInstance.h>

#include "frc/shuffleboard/ShuffleboardTab.h"

using namespace frc;

void Shuffleboard::Update() { GetInstance().Update(); }

ShuffleboardTab& Shuffleboard::GetTab(wpi::StringRef title) {
  return GetInstance().GetTab(title);
}

void Shuffleboard::EnableActuatorWidgets() {
  GetInstance().EnableActuatorWidgets();
}

void Shuffleboard::DisableActuatorWidgets() {
  // Need to update to make sure the sendable builders are initialized
  Update();
  GetInstance().DisableActuatorWidgets();
}

detail::ShuffleboardInstance& Shuffleboard::GetInstance() {
  static detail::ShuffleboardInstance inst(
      nt::NetworkTableInstance::GetDefault());
  return inst;
}
