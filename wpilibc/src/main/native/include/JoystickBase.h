/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2017 FIRST. All Rights Reserved.                        */
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

  virtual double GetZ(JoystickHand hand = kRightHand) const = 0;
  virtual double GetTwist() const = 0;
  virtual double GetThrottle() const = 0;

  virtual bool GetTrigger(JoystickHand hand = kRightHand) const = 0;
  virtual bool GetTop(JoystickHand hand = kRightHand) const = 0;
};

}  // namespace frc
