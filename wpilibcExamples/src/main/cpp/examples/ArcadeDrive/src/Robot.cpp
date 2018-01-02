/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <Drive/DifferentialDrive.h>
#include <IterativeRobot.h>
#include <Joystick.h>
#include <Spark.h>

/**
 * This is a demo program showing the use of the DifferentialDrive class.
 * Runs the motors with arcade steering.
 */
class Robot : public frc::IterativeRobot {
	frc::Spark m_leftMotor{0};
	frc::Spark m_rightMotor{1};
	frc::DifferentialDrive m_robotDrive{m_leftMotor, m_rightMotor};
	frc::Joystick m_stick{0};

public:
	void TeleopPeriodic() {
		// drive with arcade style
		m_robotDrive.ArcadeDrive(m_stick.GetY(), m_stick.GetX());
	}
};

START_ROBOT_CLASS(Robot)
