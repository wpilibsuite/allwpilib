// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/command/CommandHelper.h>
#include <frc/command/SequentialCommandGroup.h>

#include "subsystems/Claw.h"
#include "subsystems/Drivetrain.h"
#include "subsystems/Elevator.h"
#include "subsystems/Wrist.h"

/**
 * The main autonomous command to pickup and deliver the soda to the box.
 */
class Autonomous
    : public frc::CommandHelper<frc::SequentialCommandGroup, Autonomous> {
 public:
  Autonomous(Claw& claw, Wrist& wrist, Elevator& elevator,
             Drivetrain& drivetrain);
};
