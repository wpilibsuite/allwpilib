/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <frc/DigitalInput.h>
#include <frc/DutyCycle.h>
#include <frc/DutyCycleEncoder.h>
#include <frc/TimedRobot.h>
#include <frc/smartdashboard/SmartDashboard.h>

class Robot : public frc::TimedRobot {
  frc::DigitalInput m_input{0};  // Input channel

  // Duty cycle encoder
  frc::DutyCycleEncoder m_dutyCycleEncoder{m_input};

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
    frc::SmartDashboard::PutNumber("Output", output.to<double>());
    frc::SmartDashboard::PutNumber("Distance", distance);
  }
};

#ifndef RUNNING_FRC_TESTS
int main() { return frc::StartRobot<Robot>(); }
#endif
