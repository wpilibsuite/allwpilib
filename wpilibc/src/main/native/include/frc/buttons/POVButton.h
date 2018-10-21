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
  /**
   * Creates a POV button for triggering commands.
   *
   * @param joystick The GenericHID object that has the POV
   * @param angle The desired angle in degrees (e.g. 90, 270)
   * @param povNumber The POV number (@see GenericHID#GetPOV)
   */
  POVButton(GenericHID& joystick, int angle, int povNumber = 0);
  virtual ~POVButton() = default;

  POVButton(POVButton&&) = default;
  POVButton& operator=(POVButton&&) = default;

  bool Get() override;

 private:
  GenericHID* m_joystick;
  int m_angle;
  int m_povNumber;
};
}  // namespace frc
