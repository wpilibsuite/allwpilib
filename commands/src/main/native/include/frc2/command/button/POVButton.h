// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once
#include <frc/GenericHID.h>
#include <wpi/deprecated.h>

#include "Button.h"

namespace frc2 {
/**
 * A class used to bind command scheduling to joystick POV presses.  Can be
 * composed with other buttons with the operators in Trigger.
 *
 * This class is provided by the NewCommands VendorDep
 *
 * @see Trigger
 */
class POVButton : public Button {
 public:
  /**
   * Creates a POVButton that commands can be bound to.
   *
   * @param joystick The joystick on which the button is located.
   * @param angle The angle of the POV corresponding to a button press.
   * @param povNumber The number of the POV on the joystick.
   */
  WPI_IGNORE_DEPRECATED
  POVButton(frc::GenericHID* joystick, int angle, int povNumber = 0)
      : Button([joystick, angle, povNumber] {
          return joystick->GetPOV(povNumber) == angle;
        }) {}
  WPI_UNIGNORE_DEPRECATED
};
}  // namespace frc2
