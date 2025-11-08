// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hardware/discrete/DigitalInput.hpp"
#include "wpi/hardware/rotation/DutyCycle.hpp"
#include "wpi/opmode/TimedRobot.hpp"
#include "wpi/smartdashboard/SmartDashboard.hpp"

class Robot : public frc::TimedRobot {
  frc::DutyCycle m_dutyCycle{0};  // Duty cycle input

 public:
  Robot() {}

  void RobotPeriodic() override {
    // Duty Cycle Frequency in Hz
    auto frequency = m_dutyCycle.GetFrequency();

    // Output of duty cycle, ranging from 0 to 1
    // 1 is fully on, 0 is fully off
    auto output = m_dutyCycle.GetOutput();

    frc::SmartDashboard::PutNumber("Frequency", frequency.value());
    frc::SmartDashboard::PutNumber("Duty Cycle", output);
  }
};

#ifndef RUNNING_FRC_TESTS
int main() {
  return frc::StartRobot<Robot>();
}
#endif
