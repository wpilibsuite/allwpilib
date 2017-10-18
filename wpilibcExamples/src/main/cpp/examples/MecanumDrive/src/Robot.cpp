/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <IterativeRobot.h>
#include <Joystick.h>
#include <RobotDrive.h>

/**
 * This is a demo program showing how to use Mecanum control with the RobotDrive
 * class.
 */
class Robot : public frc::IterativeRobot {
public:
	void RobotInit() {
		// Invert the left side motors
		// You may need to change or remove this to match your robot
		robotDrive.SetInvertedMotor(RobotDrive::kFrontLeftMotor, true);
		robotDrive.SetInvertedMotor(RobotDrive::kRearLeftMotor, true);
	}

	void TeleopPeriodic() override {
		/* Use the joystick X axis for lateral movement, Y axis for
		 * forward
		 * movement, and Z axis for rotation.
		 */
		robotDrive.MecanumDrive_Cartesian(
				stick.GetX(), stick.GetY(), stick.GetZ());
	}

private:
	static constexpr int kFrontLeftChannel = 2;
	static constexpr int kRearLeftChannel = 3;
	static constexpr int kFrontRightChannel = 1;
	static constexpr int kRearRightChannel = 0;

	static constexpr int kJoystickChannel = 0;

	frc::RobotDrive robotDrive{kFrontLeftChannel, kRearLeftChannel,
			kFrontRightChannel, kRearRightChannel};

	frc::Joystick stick{kJoystickChannel};
};

START_ROBOT_CLASS(Robot)
