/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "GenericHID.h"

namespace frc {

/**
 * Joystick Interface.
 */
class JoystickBase : public GenericHID {
 public:
  explicit JoystickBase(int port);
  virtual ~JoystickBase() = default;

  virtual float GetZ(JoystickHand hand = kRightHand) const = 0;
  virtual float GetTwist() const = 0;
  virtual float GetThrottle() const = 0;

  virtual bool GetTrigger(JoystickHand hand = kRightHand) const = 0;
  virtual bool GetTop(JoystickHand hand = kRightHand) const = 0;
};

}  // namespace frc
