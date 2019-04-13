/*----------------------------------------------------------------------------*/
/* Copyright (c) 2011-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/buttons/Button.h"

using namespace frc;

void Button::WhenPressed(Command* command) { WhenActive(command); }

void Button::WhileHeld(Command* command) { WhileActive(command); }

void Button::WhenReleased(Command* command) { WhenInactive(command); }

void Button::CancelWhenPressed(Command* command) { CancelWhenActive(command); }

void Button::ToggleWhenPressed(Command* command) { ToggleWhenActive(command); }
