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
 * Pickup a soda can (if one is between the open claws) and
 * get it in a safe state to drive around.
 */
class Pickup
    : public frc2::CommandHelper<frc2::SequentialCommandGroup, Pickup> {
 public:
  Pickup(Claw* claw, Wrist* wrist, Elevator* elevator);
};
