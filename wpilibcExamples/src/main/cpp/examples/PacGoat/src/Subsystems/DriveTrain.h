/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>

#include <AnalogGyro.h>
#include <Commands/Subsystem.h>
#include <Encoder.h>
#include <RobotDrive.h>
#include <Victor.h>

namespace frc {
class Joystick;
}

/**
 * The DriveTrain subsystem controls the robot's chassis and reads in
 * information about it's speed and position.
 */
class DriveTrain : public frc::Subsystem {
public:
	DriveTrain();

	/**
	 * When other commands aren't using the drivetrain, allow tank drive
	 * with
	 * the joystick.
	 */
	void InitDefaultCommand();

	/**
	 * @param joy PS3 style joystick to use as the input for tank drive.
	 */
	void TankDrive(frc::Joystick* joy);

	/**
	 * @param leftAxis Left sides value
	 * @param rightAxis Right sides value
	 */
	void TankDrive(double leftAxis, double rightAxis);

	/**
	 * Stop the drivetrain from moving.
	 */
	void Stop();

	/**
	 * @return The encoder getting the distance and speed of left side of
	 * the drivetrain.
	 */
	std::shared_ptr<Encoder> GetLeftEncoder();

	/**
	 * @return The encoder getting the distance and speed of right side of
	 * the drivetrain.
	 */
	std::shared_ptr<Encoder> GetRightEncoder();

	/**
	 * @return The current angle of the drivetrain.
	 */
	double GetAngle();

private:
	// Subsystem devices
	frc::Victor frontLeftCIM{1};
	frc::Victor rearLeftCIM{2};
	frc::Victor frontRightCIM{3};
	frc::Victor rearRightCIM{4};
	frc::RobotDrive drive{frontRightCIM, rearLeftCIM, frontRightCIM,
			rearRightCIM};
	std::shared_ptr<frc::Encoder> rightEncoder =
			std::make_shared<frc::Encoder>(
					1, 2, true, Encoder::k4X);
	std::shared_ptr<frc::Encoder> leftEncoder =
			std::make_shared<frc::Encoder>(
					3, 4, false, Encoder::k4X);
	frc::AnalogGyro gyro{0};
};
