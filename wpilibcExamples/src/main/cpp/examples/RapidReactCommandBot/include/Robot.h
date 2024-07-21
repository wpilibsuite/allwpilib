// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc2/command/CommandPtr.h>
#include <frc2/command/CommandRobot.h>
#include <frc2/command/button/CommandXboxController.h>

#include "Constants.h"
#include "subsystems/Drive.h"
#include "subsystems/Intake.h"
#include "subsystems/Pneumatics.h"
#include "subsystems/Shooter.h"
#include "subsystems/Storage.h"

class Robot : public frc2::CommandRobot {
 public:
  Robot();

 private:
  // The robot's subsystems
  Drive m_drive;
  Intake m_intake;
  Shooter m_shooter;
  Storage m_storage;
  Pneumatics m_pneumatics;

  // The driver's controller
  frc2::CommandXboxController m_driverController{
      OIConstants::kDriverControllerPort};

  frc2::CommandPtr m_exampleAuto{
      m_drive
          .DriveDistanceCommand(AutoConstants::kDriveDistance,
                                AutoConstants::kDriveSpeed)
          .WithTimeout(AutoConstants::kTimeout)};
};
