// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/framework/TimedRobot.hpp"
#include "wpi/hardware/discrete/DigitalInput.hpp"
#include "wpi/hardware/rotation/DutyCycle.hpp"
#include "wpi/smartdashboard/SmartDashboard.hpp"

class Robot : public wpi::TimedRobot {
  wpi::DutyCycle dutyCycle{0};  // Duty cycle input

 public:
  Robot() {}

  void RobotPeriodic() override {
    // Duty Cycle Frequency in Hz
    auto frequency = dutyCycle.GetFrequency();

    // Output of duty cycle, ranging from 0 to 1
    // 1 is fully on, 0 is fully off
    auto output = dutyCycle.GetOutput();

    wpi::SmartDashboard::PutNumber("Frequency", frequency.value());
    wpi::SmartDashboard::PutNumber("Duty Cycle", output);
  }
};

#ifndef RUNNING_WPILIB_TESTS
int main() {
  return wpi::StartRobot<Robot>();
}
#endif
