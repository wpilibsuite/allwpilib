// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/command/CommandHelper.h>
#include <frc/command/SequentialCommandGroup.h>

#include "subsystems/Claw.h"
#include "subsystems/Elevator.h"
#include "subsystems/Wrist.h"

/**
 * Make sure the robot is in a state to pickup soda cans.
 */
class PrepareToPickup
    : public frc::CommandHelper<frc::SequentialCommandGroup, PrepareToPickup> {
 public:
  PrepareToPickup(Claw& claw, Wrist& wrist, Elevator& elevator);
};
