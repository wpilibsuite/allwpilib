// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/buttons/Trigger.h>

namespace frc {
class GenericHID;
}  // namespace frc

class DoubleButton : public frc::Trigger {
 public:
  DoubleButton(frc::GenericHID* joy, int button1, int button2);

  bool Get() override;

 private:
  frc::GenericHID& m_joy;
  int m_button1;
  int m_button2;
};
