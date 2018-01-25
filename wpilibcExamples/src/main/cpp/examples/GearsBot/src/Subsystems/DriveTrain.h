/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <AnalogGyro.h>
#include <AnalogInput.h>
#include <Commands/Subsystem.h>
#include <Drive/DifferentialDrive.h>
#include <Encoder.h>
#include <Spark.h>
#include <SpeedControllerGroup.h>

namespace frc {
class Joystick;
}  // namespace frc

/**
 * The DriveTrain subsystem incorporates the sensors and actuators attached to
 * the robots chassis. These include four drive motors, a left and right encoder
 * and a gyro.
 */
class DriveTrain : public frc::Subsystem {
public:
	DriveTrain();

	/**
	 * When no other command is running let the operator drive around
	 * using the PS3 joystick.
	 */
	void InitDefaultCommand() override;

	/**
	 * The log method puts interesting information to the SmartDashboard.
	 */
	void Log();

	/**
	 * Tank style driving for the DriveTrain.
	 * @param left Speed in range [-1,1]
	 * @param right Speed in range [-1,1]
	 */
	void Drive(double left, double right);

	/**
	 * @return The robots heading in degrees.
	 */
	double GetHeading();

	/**
	 * Reset the robots sensors to the zero states.
	 */
	void Reset();

	/**
	 * @return The distance driven (average of left and right encoders).
	 */
	double GetDistance();

	/**
	 * @return The distance to the obstacle detected by the rangefinder.
	 */
	double GetDistanceToObstacle();

private:
	frc::Spark m_frontLeft{1};
	frc::Spark m_rearLeft{2};
	frc::SpeedControllerGroup m_left{m_frontLeft, m_rearLeft};

	frc::Spark m_frontRight{3};
	frc::Spark m_rearRight{4};
	frc::SpeedControllerGroup m_right{m_frontRight, m_rearRight};

	frc::DifferentialDrive m_robotDrive{m_left, m_right};

	frc::Encoder m_leftEncoder{1, 2};
	frc::Encoder m_rightEncoder{3, 4};
	frc::AnalogInput m_rangefinder{6};
	frc::AnalogGyro m_gyro{1};
};
