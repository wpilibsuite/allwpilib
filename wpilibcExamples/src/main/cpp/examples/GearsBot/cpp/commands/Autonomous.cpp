// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "commands/Autonomous.h"

#include <frc2/command/ParallelCommandGroup.h>

#include "commands/CloseClaw.h"
#include "commands/DriveStraight.h"
#include "commands/Pickup.h"
#include "commands/Place.h"
#include "commands/PrepareToPickup.h"
#include "commands/SetDistanceToBox.h"
#include "commands/SetWristSetpoint.h"

Autonomous::Autonomous(Claw* claw, Wrist* wrist, Elevator* elevator,
                       DriveTrain* drivetrain) {
  SetName("Autonomous");
  AddCommands(
      // clang-format off
      PrepareToPickup(claw, wrist, elevator),
      Pickup(claw, wrist, elevator),
      SetDistanceToBox(0.10, drivetrain),
      // DriveStraight(4, drivetrain) // Use encoders if ultrasonic is broken
      Place(claw, wrist, elevator),
      SetDistanceToBox(0.6, drivetrain),
      // DriveStraight(-2, drivetrain) // Use encoders if ultrasonic is broken
      frc2::ParallelCommandGroup(SetWristSetpoint(-45, wrist),
                                 CloseClaw(claw)));
  // clang-format on
}
