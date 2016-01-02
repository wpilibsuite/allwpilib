/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef __JOYSTICK_BUTTON_H__
#define __JOYSTICK_BUTTON_H__

#include "GenericHID.h"
#include "Buttons/Button.h"

class JoystickButton : public Button {
 public:
  JoystickButton(GenericHID *joystick, int buttonNumber);
  virtual ~JoystickButton() = default;

  virtual bool Get();

 private:
  GenericHID *m_joystick;
  int m_buttonNumber;
};

#endif
