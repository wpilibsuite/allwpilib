/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011. All Rights Reserved.
 */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "Buttons/Button.h"

/**
 * Specifies the command to run when a button is first pressed
 * @param command The pointer to the command to run
 */
void Button::WhenPressed(Command *command) { WhenActive(command); }

/**
 * Specifies the command to be scheduled while the button is pressed
 * The command will be scheduled repeatedly while the button is pressed and will
 * be canceled when the button is released.
 * @param command The pointer to the command to run
 */
void Button::WhileHeld(Command *command) { WhileActive(command); }

/**
 * Specifies the command to run when the button is released
 * The command will be scheduled a single time.
 * @param The pointer to the command to run
 */
void Button::WhenReleased(Command *command) { WhenInactive(command); }

/**
 * Cancels the specificed command when the button is pressed
 * @param The command to be canceled
 */
void Button::CancelWhenPressed(Command *command) { CancelWhenActive(command); }

/**
 * Toggle the specified command when the button is pressed
 * @param The command to be toggled
 */
void Button::ToggleWhenPressed(Command *command) { ToggleWhenActive(command); }
