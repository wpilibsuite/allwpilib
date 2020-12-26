// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/buttons/Button.h"

using namespace frc;

void Button::WhenPressed(Command* command) { WhenActive(command); }

void Button::WhileHeld(Command* command) { WhileActive(command); }

void Button::WhenReleased(Command* command) { WhenInactive(command); }

void Button::CancelWhenPressed(Command* command) { CancelWhenActive(command); }

void Button::ToggleWhenPressed(Command* command) { ToggleWhenActive(command); }
