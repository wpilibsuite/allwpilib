/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <wpi/deprecated.h>

#include "frc/GenericHID.h"

namespace frc {

/**
 * Gamepad Interface.
 */
class GamepadBase : public GenericHID {
 public:
  WPI_DEPRECATED("Inherit directly from GenericHID instead.")
  explicit GamepadBase(int port);
  virtual ~GamepadBase() = default;

  GamepadBase(GamepadBase&&) = default;
  GamepadBase& operator=(GamepadBase&&) = default;

  virtual bool GetBumper(JoystickHand hand = kRightHand) const = 0;
  virtual bool GetStickButton(JoystickHand hand) const = 0;
};

}  // namespace frc
