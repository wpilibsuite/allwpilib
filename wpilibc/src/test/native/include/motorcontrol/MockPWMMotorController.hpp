// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

namespace wpi {

class MockPWMMotorController {
 public:
  void Set(double speed);
  double Get() const;
  void SetInverted(bool isInverted);
  bool GetInverted() const;
  void Disable();
  void StopMotor();

 private:
  double m_speed = 0.0;
  bool m_isInverted = false;
};

}  // namespace wpi
