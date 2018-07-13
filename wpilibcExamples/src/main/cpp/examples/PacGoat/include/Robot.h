/*----------------------------------------------------------------------------*/
/* Use and modify this template how you wish, it's public domain.             */
/*----------------------------------------------------------------------------*/

#pragma once

#include <Commands/Command.h>
#include <IterativeRobot.h>
#include <SmartDashboard/SendableChooser.h>

#include "Commands/DriveAndShootAutonomous.h"
#include "Commands/DriveForward.h"
#include "OI.h"
#include "Subsystems/Collector.h"
#include "Subsystems/DriveTrain.h"
#include "Subsystems/Pivot.h"
#include "Subsystems/Pneumatics.h"
#include "Subsystems/Shooter.h"

class Robot : public frc::IterativeRobot {
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
