// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "frc/buttons/Trigger.h"
#include "frc/commands/Command.h"

namespace frc {

/**
 * This class provides an easy way to link commands to OI inputs.
 *
 * It is very easy to link a button to a command.  For instance, you could link
 * the trigger button of a joystick to a "score" command.
 *
 * This class represents a subclass of Trigger that is specifically aimed at
 * buttons on an operator interface as a common use case of the more generalized
 * Trigger objects. This is a simple wrapper around Trigger with the method
 * names renamed to fit the Button object use.
 */
class Button : public Trigger {
 public:
  Button() = default;
  Button(Button&&) = default;
  Button& operator=(Button&&) = default;

  /**
   * Specifies the command to run when a button is first pressed.
   *
   * @param command The pointer to the command to run
   */
  virtual void WhenPressed(Command* command);

  /**
   * Specifies the command to be scheduled while the button is pressed.
   *
   * The command will be scheduled repeatedly while the button is pressed and
   * will be canceled when the button is released.
   *
   * @param command The pointer to the command to run
   */
  virtual void WhileHeld(Command* command);

  /**
   * Specifies the command to run when the button is released.
   *
   * The command will be scheduled a single time.
   *
   * @param command The pointer to the command to run
   */
  virtual void WhenReleased(Command* command);

  /**
   * Cancels the specificed command when the button is pressed.
   *
   * @param command The command to be canceled
   */
  virtual void CancelWhenPressed(Command* command);

  /**
   * Toggle the specified command when the button is pressed.
   *
   * @param command The command to be toggled
   */
  virtual void ToggleWhenPressed(Command* command);
};

}  // namespace frc
