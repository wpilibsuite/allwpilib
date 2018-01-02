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
 * Drive until the robot is the given distance away from the box. Uses a local
 * PID controller to run a simple PID loop that is only enabled while this
 * command is running. The input is the averaged values of the left and right
 * encoders.
 */
class SetDistanceToBox : public frc::Command {
public:
	explicit SetDistanceToBox(double distance);
	void Initialize() override;
	bool IsFinished() override;
	void End() override;

	class SetDistanceToBoxPIDSource : public frc::PIDSource {
	public:
		virtual ~SetDistanceToBoxPIDSource() = default;
		double PIDGet() override;
	};

	class SetDistanceToBoxPIDOutput : public frc::PIDOutput {
	public:
		virtual ~SetDistanceToBoxPIDOutput() = default;
		void PIDWrite(double d) override;
	};

private:
	SetDistanceToBoxPIDSource m_source;
	SetDistanceToBoxPIDOutput m_output;
	frc::PIDController m_pid{-2, 0, 0, &m_source, &m_output};
};
