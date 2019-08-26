/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <frc2/command/CommandHelper.h>
#include <frc2/command/SequentialCommandGroup.h>

#include "subsystems/Claw.h"
#include "subsystems/DriveTrain.h"
#include "subsystems/Elevator.h"
#include "subsystems/Wrist.h"

/**
 * The main autonomous command to pickup and deliver the soda to the box.
 */
class Autonomous
    : public frc2::CommandHelper<frc2::SequentialCommandGroup, Autonomous> {
 public:
  Autonomous(Claw* claw, Wrist* wrist, Elevator* elevator,
             DriveTrain* drivetrain);
};
