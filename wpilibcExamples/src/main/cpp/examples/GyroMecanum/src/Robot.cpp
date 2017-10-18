/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <AnalogGyro.h>
#include <IterativeRobot.h>
#include <Joystick.h>
#include <RobotDrive.h>

/**
 * This is a sample program that uses mecanum drive with a gyro sensor to
 * maintian rotation vectorsin relation to the starting orientation of the robot
 * (field-oriented controls).
 */
class Robot : public frc::IterativeRobot {
public:
	void RobotInit() override {
		// invert the left side motors
		// you may need to change or remove this to match your robot
		myRobot.SetInvertedMotor(RobotDrive::kFrontLeftMotor, true);
		myRobot.SetInvertedMotor(RobotDrive::kRearLeftMotor, true);

		gyro.SetSensitivity(kVoltsPerDegreePerSecond);
	}

	/**
	 * Mecanum drive is used with the gyro angle as an input.
	 */
	void TeleopPeriodic() override {
		myRobot.MecanumDrive_Cartesian(joystick.GetX(), joystick.GetY(),
				joystick.GetZ(), gyro.GetAngle());
	}

private:
	// Gyro calibration constant, may need to be adjusted
	// Gyro value of 360 is set to correspond to one full revolution
	static constexpr double kVoltsPerDegreePerSecond = 0.0128;

	static constexpr int kFrontLeftMotorPort = 0;
	static constexpr int kFrontRightMotorPort = 1;
	static constexpr int kRearLeftMotorPort = 2;
	static constexpr int kRearRightMotorPort = 3;
	static constexpr int kGyroPort = 0;
	static constexpr int kJoystickPort = 0;

	frc::RobotDrive myRobot{kFrontLeftMotorPort, kFrontRightMotorPort,
			kRearLeftMotorPort, kRearRightMotorPort};
	frc::AnalogGyro gyro{kGyroPort};
	frc::Joystick joystick{kJoystickPort};
};

START_ROBOT_CLASS(Robot)
