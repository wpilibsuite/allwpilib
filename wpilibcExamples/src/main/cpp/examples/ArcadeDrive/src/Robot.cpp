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
	frc::Spark m_left{0};
	frc::Spark m_right{1};
	frc::RobotDrive m_robotDrive{m_left, m_right};
	frc::Joystick m_stick{0};

public:
	void TeleopPeriodic() {
		// drive with arcade style
		m_robotDrive.ArcadeDrive(m_stick.GetY(), m_stick.GetX());
	}
};

START_ROBOT_CLASS(Robot)
