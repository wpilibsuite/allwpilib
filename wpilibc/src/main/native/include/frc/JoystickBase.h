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
 * Joystick Interface.
 */
class JoystickBase : public GenericHID {
 public:
  WPI_DEPRECATED("Inherit directly from GenericHID instead.")
  explicit JoystickBase(int port);
  virtual ~JoystickBase() = default;

  JoystickBase(JoystickBase&&) = default;
  JoystickBase& operator=(JoystickBase&&) = default;

  virtual double GetZ(JoystickHand hand = kRightHand) const = 0;
  virtual double GetTwist() const = 0;
  virtual double GetThrottle() const = 0;
};

}  // namespace frc
