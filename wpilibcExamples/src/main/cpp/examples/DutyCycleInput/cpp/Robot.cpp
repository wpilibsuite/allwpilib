// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <frc/DigitalInput.h>
#include <frc/DutyCycle.h>
#include <frc/TimedRobot.h>
#include <frc/smartdashboard/SmartDashboard.h>

class Robot : public frc::TimedRobot {
  frc::DigitalInput m_input{0};         // Input channel
  frc::DutyCycle m_dutyCycle{m_input};  // Duty cycle input

 public:
  Robot() {}

  void RobotPeriodic() override {
    // Duty Cycle Frequency in Hz
    auto frequency = m_dutyCycle.GetFrequency();

    // Output of duty cycle, ranging from 0 to 1
    // 1 is fully on, 0 is fully off
    auto output = m_dutyCycle.GetOutput();

    frc::SmartDashboard::PutNumber("Frequency", frequency);
    frc::SmartDashboard::PutNumber("Duty Cycle", output);
  }
};

#ifndef RUNNING_FRC_TESTS
int main() {
  return frc::StartRobot<Robot>();
}
#endif
