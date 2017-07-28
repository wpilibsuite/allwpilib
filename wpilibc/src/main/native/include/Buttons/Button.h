/*----------------------------------------------------------------------------*/
/* Copyright (c) 2011-2017 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "Buttons/Trigger.h"
#include "Commands/Command.h"

namespace frc {

/**
 * This class provides an easy way to link commands to OI inputs.
 *
 * It is very easy to link a button to a command.  For instance, you could
 * link the trigger button of a joystick to a "score" command.
 *
 * This class represents a subclass of Trigger that is specifically aimed at
 * buttons on an operator interface as a common use case of the more generalized
 * Trigger objects. This is a simple wrapper around Trigger with the method
 * names
 * renamed to fit the Button object use.
 */
class Button : public Trigger {
 public:
  virtual void WhenPressed(Command* command);
  virtual void WhileHeld(Command* command);
  virtual void WhenReleased(Command* command);
  virtual void CancelWhenPressed(Command* command);
  virtual void ToggleWhenPressed(Command* command);
};

}  // namespace frc
