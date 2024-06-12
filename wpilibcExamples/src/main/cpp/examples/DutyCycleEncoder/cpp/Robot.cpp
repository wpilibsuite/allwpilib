// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <frc/DutyCycleEncoder.h>
#include <frc/TimedRobot.h>
#include <frc/smartdashboard/SmartDashboard.h>

class Robot : public frc::TimedRobot {
  // Duty cycle encoder on channel 0
  frc::DutyCycleEncoder m_dutyCycleEncoder{0};

 public:
  void RobotInit() override {
    // Set to 0.5 units per rotation
    m_dutyCycleEncoder.SetDistancePerRotation(0.5);
  }

  void RobotPeriodic() override {
    // Connected can be checked, and uses the frequency of the encoder
    auto connected = m_dutyCycleEncoder.IsConnected();

    // Duty Cycle Frequency in Hz
    auto frequency = m_dutyCycleEncoder.GetFrequency();

    // Output of encoder
    auto output = m_dutyCycleEncoder.Get();

    // Output scaled by DistancePerPulse
    auto distance = m_dutyCycleEncoder.GetDistance();

    frc::SmartDashboard::PutBoolean("Connected", connected);
    frc::SmartDashboard::PutNumber("Frequency", frequency);
    frc::SmartDashboard::PutNumber("Output", output.value());
    frc::SmartDashboard::PutNumber("Distance", distance);
  }
};

#ifndef RUNNING_FRC_TESTS
int main() {
  return frc::StartRobot<Robot>();
}
#endif
