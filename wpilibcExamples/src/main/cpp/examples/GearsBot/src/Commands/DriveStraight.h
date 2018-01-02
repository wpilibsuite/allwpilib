/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <Commands/Command.h>
#include <PIDController.h>
#include <PIDOutput.h>
#include <PIDSource.h>

/**
 * Drive the given distance straight (negative values go backwards).
 * Uses a local PID controller to run a simple PID loop that is only
 * enabled while this command is running. The input is the averaged
 * values of the left and right encoders.
 */
class DriveStraight : public frc::Command {
public:
	explicit DriveStraight(double distance);
	void Initialize() override;
	bool IsFinished() override;
	void End() override;

	class DriveStraightPIDSource : public frc::PIDSource {
	public:
		virtual ~DriveStraightPIDSource() = default;
		double PIDGet() override;
	};

	class DriveStraightPIDOutput : public frc::PIDOutput {
	public:
		virtual ~DriveStraightPIDOutput() = default;
		void PIDWrite(double d) override;
	};

private:
	DriveStraightPIDSource m_source;
	DriveStraightPIDOutput m_output;
	frc::PIDController m_pid{4, 0, 0, &m_source, &m_output};
};
