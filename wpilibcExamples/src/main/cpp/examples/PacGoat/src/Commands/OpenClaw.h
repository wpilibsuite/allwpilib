/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <Commands/Command.h>

/**
 * Opens the claw
 */
class OpenClaw : public frc::Command {
public:
	OpenClaw();
	void Initialize() override;
	bool IsFinished() override;
};
