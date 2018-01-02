/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <Commands/CommandGroup.h>

/**
 * Pickup a soda can (if one is between the open claws) and
 * get it in a safe state to drive around.
 */
class Pickup : public frc::CommandGroup {
public:
	Pickup();
};
