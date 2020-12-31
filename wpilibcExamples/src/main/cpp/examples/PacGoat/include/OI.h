// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/Joystick.h>
#include <frc/buttons/JoystickButton.h>

#include "triggers/DoubleButton.h"

class OI {
 public:
  OI();
  frc::Joystick& GetJoystick();

 private:
  frc::Joystick m_joystick{0};

  frc::JoystickButton m_l1{&m_joystick, 11};
  frc::JoystickButton m_l2{&m_joystick, 9};
  frc::JoystickButton m_r1{&m_joystick, 12};
  frc::JoystickButton m_r2{&m_joystick, 10};

  DoubleButton m_sticks{&m_joystick, 2, 3};
};
