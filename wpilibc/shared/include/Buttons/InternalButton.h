/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef __INTERNAL_BUTTON_H__
#define __INTERNAL_BUTTON_H__

#include "Buttons/Button.h"

class InternalButton : public Button {
 public:
  InternalButton() = default;
  InternalButton(bool inverted);
  virtual ~InternalButton() = default;

  void SetInverted(bool inverted);
  void SetPressed(bool pressed);

  virtual bool Get();

 private:
  bool m_pressed = false;
  bool m_inverted = false;
};

#endif
