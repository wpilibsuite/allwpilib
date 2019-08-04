/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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
