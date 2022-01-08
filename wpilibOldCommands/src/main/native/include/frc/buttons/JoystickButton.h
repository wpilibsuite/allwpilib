// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "frc/GenericHID.h"
#include "frc/buttons/Button.h"

namespace frc {

/**
 * A Button} that gets its state from a GenericHID.
 *
 * This class is provided by the OldCommands VendorDep
 */
class JoystickButton : public Button {
 public:
  JoystickButton(GenericHID* joystick, int buttonNumber);
  ~JoystickButton() override = default;

  JoystickButton(JoystickButton&&) = default;
  JoystickButton& operator=(JoystickButton&&) = default;

  bool Get() override;

 private:
  GenericHID* m_joystick;
  int m_buttonNumber;
};

}  // namespace frc
