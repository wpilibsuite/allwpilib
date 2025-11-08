// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <wpi/hardware/discrete/DigitalInput.hpp>
#include <wpi/opmode/TimedRobot.hpp>

/**
 * Digital Input snippets for frc-docs.
 * https://docs.wpilib.org/en/stable/docs/software/hardware-apis/sensors/digital-input-software.html
 */
class Robot : public frc::TimedRobot {
 public:
  void TeleopPeriodic() override {
    // Gets the value of the digital input.  Returns true if the circuit is
    // open.
    m_input.Get();
  }

 private:
  // Initializes a DigitalInput on DIO 0
  frc::DigitalInput m_input{0};
};

#ifndef RUNNING_FRC_TESTS
int main() {
  return frc::StartRobot<Robot>();
}
#endif
