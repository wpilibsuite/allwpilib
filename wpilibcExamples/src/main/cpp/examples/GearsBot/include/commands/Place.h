// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc2/command/CommandHelper.h>
#include <frc2/command/SequentialCommandGroup.h>

#include "subsystems/Claw.h"
#include "subsystems/Elevator.h"
#include "subsystems/Wrist.h"

/**
 * Place a held soda can onto the platform.
 */
class Place : public frc2::CommandHelper<frc2::SequentialCommandGroup, Place> {
 public:
  Place(Claw* claw, Wrist* wrist, Elevator* elevator);
};
