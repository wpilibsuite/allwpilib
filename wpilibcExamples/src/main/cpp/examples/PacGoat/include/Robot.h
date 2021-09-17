// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/TimedRobot.h>
#include <frc/commands/Command.h>
#include <frc/smartdashboard/SendableChooser.h>

#include "OI.h"
#include "commands/DriveAndShootAutonomous.h"
#include "commands/DriveForward.h"
#include "subsystems/Collector.h"
#include "subsystems/DriveTrain.h"
#include "subsystems/Pivot.h"
#include "subsystems/Pneumatics.h"
#include "subsystems/Shooter.h"

class Robot : public frc::TimedRobot {
 public:
  static DriveTrain drivetrain;
  static Pivot pivot;
  static Collector collector;
  static Shooter shooter;
  static Pneumatics pneumatics;
  static OI oi;

 private:
  frc::Command* m_autonomousCommand = nullptr;
  DriveAndShootAutonomous m_driveAndShootAuto;
  DriveForward m_driveForwardAuto;
  frc::SendableChooser<frc::Command*> m_autoChooser;

  void RobotInit() override;
  void AutonomousInit() override;
  void AutonomousPeriodic() override;
  void TeleopInit() override;
  void TeleopPeriodic() override;
  void TestPeriodic() override;
  void DisabledInit() override;
  void DisabledPeriodic() override;

  void Log();
};
