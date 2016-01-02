/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

/** GenericHID Interface
 */
class GenericHID {
 public:
  enum JoystickHand { kLeftHand = 0, kRightHand = 1 };

  virtual ~GenericHID() = default;

  virtual float GetX(JoystickHand hand = kRightHand) const = 0;
  virtual float GetY(JoystickHand hand = kRightHand) const = 0;
  virtual float GetZ() const = 0;
  virtual float GetTwist() const = 0;
  virtual float GetThrottle() const = 0;
  virtual float GetRawAxis(uint32_t axis) const = 0;

  virtual bool GetTrigger(JoystickHand hand = kRightHand) const = 0;
  virtual bool GetTop(JoystickHand hand = kRightHand) const = 0;
  virtual bool GetBumper(JoystickHand hand = kRightHand) const = 0;
  virtual bool GetRawButton(uint32_t button) const = 0;

  virtual int GetPOV(uint32_t pov = 0) const = 0;
};
