/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <WPILib.h>

/**
 * This is a demo program showing the use of the RobotDrive class.
 * Runs the motors with arcade steering.
 */
class Robot : public frc::IterativeRobot {
	frc::RobotDrive myRobot{0, 1};
	frc::Joystick stick{0};

public:
	void TeleopPeriodic() {
		myRobot.ArcadeDrive(stick);  // drive with arcade style
	}
};

START_ROBOT_CLASS(Robot)
