/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <frc/TimedRobot.h>
#include <frc/commands/Command.h>
#include <frc/commands/Scheduler.h>
#include <frc/livewindow/LiveWindow.h>
#include <frc/smartdashboard/SmartDashboard.h>

#include "OI.h"
#include "commands/Autonomous.h"
#include "subsystems/Claw.h"
#include "subsystems/DriveTrain.h"
#include "subsystems/Elevator.h"
#include "subsystems/Wrist.h"

class Robot : public frc::TimedRobot {
 public:
  static DriveTrain drivetrain;
  static Elevator elevator;
  static Wrist wrist;
  static Claw claw;
  static OI oi;

 private:
  Autonomous m_autonomousCommand;
  frc::LiveWindow& m_lw = *frc::LiveWindow::GetInstance();

  void RobotInit() override;
  void AutonomousInit() override;
  void AutonomousPeriodic() override;
  void TeleopInit() override;
  void TeleopPeriodic() override;
  void TestPeriodic() override;
};
