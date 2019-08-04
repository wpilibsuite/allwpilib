/*----------------------------------------------------------------------------*/
/* Copyright (c) 2011-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "frc/GenericHID.h"
#include "frc/buttons/Button.h"

namespace frc {

class JoystickButton : public Button {
 public:
  JoystickButton(GenericHID* joystick, int buttonNumber);
  virtual ~JoystickButton() = default;

  JoystickButton(JoystickButton&&) = default;
  JoystickButton& operator=(JoystickButton&&) = default;

  virtual bool Get();

 private:
  GenericHID* m_joystick;
  int m_buttonNumber;
};

}  // namespace frc
