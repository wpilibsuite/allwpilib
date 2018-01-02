/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <AnalogPotentiometer.h>
#include <Commands/PIDSubsystem.h>
#include <Spark.h>

/**
 * The wrist subsystem is like the elevator, but with a rotational joint instead
 * of a linear joint.
 */
class Wrist : public PIDSubsystem {
public:
	Wrist();

	void InitDefaultCommand() override;

	/**
	 * The log method puts interesting information to the SmartDashboard.
	 */
	void Log();

	/**
	 * Use the potentiometer as the PID sensor. This method is automatically
	 * called by the subsystem.
	 */
	double ReturnPIDInput() override;

	/**
	 * Use the motor as the PID output. This method is automatically called
	 * by
	 * the subsystem.
	 */
	void UsePIDOutput(double d) override;

private:
	frc::Spark m_motor{6};

// Conversion value of potentiometer varies between the real world and
// simulation
#ifndef SIMULATION
	frc::AnalogPotentiometer m_pot{3, -270.0 / 5};
#else
	frc::AnalogPotentiometer m_pot{3};  // Defaults to degrees
#endif

	static constexpr double kP_real = 1;
	static constexpr double kP_simulation = 0.05;
};
