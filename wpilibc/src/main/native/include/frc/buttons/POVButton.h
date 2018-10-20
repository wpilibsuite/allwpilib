/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "frc/GenericHID.h"
#include "frc/buttons/Button.h"

namespace frc {
class POVButton : public Button {

 public:
  POVButton(GenericHID* joystick, int angle, int povNumber);
  virtual ~POVButton() = default;

  POVButton(POVButton&&) = default;
  POVButton& operator=(POVButton&&) = default;

  virtual bool Get();
 private:
  int m_angle;
  int m_povNumber;
  GenericHID* m_joystick;
};
}
