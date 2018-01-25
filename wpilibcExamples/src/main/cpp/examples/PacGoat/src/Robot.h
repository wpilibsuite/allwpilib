/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
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

class Robot : public IterativeRobot {
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
	SendableChooser<frc::Command*> m_autoChooser;

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
