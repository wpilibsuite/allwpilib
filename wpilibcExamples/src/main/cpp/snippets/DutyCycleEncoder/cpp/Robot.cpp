// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <wpi/framework/TimedRobot.hpp>
#include <wpi/hardware/rotation/DutyCycleEncoder.hpp>

/**
 * DutyCycleEncoder snippets for frc-docs.
 * https://docs.wpilib.org/en/stable/docs/software/hardware-apis/sensors/encoders-software.html
 */
class Robot : public wpi::TimedRobot {
 public:
  Robot() {}

  void TeleopPeriodic() override {
    // Gets the rotation
    m_encoder.Get();

    // Gets if the encoder is connected
    m_encoder.IsConnected();
  }

 private:
  // Initializes a duty cycle encoder on DIO pins 0
  wpi::DutyCycleEncoder m_encoder{0};

  // Initializes a duty cycle encoder on DIO pins 0 to return a value of 4 for
  // a full rotation, with the encoder reporting 0 half way through rotation (2
  // out of 4)
  wpi::DutyCycleEncoder m_encoderFR{0, 4.0, 2.0};
};

#ifndef RUNNING_WPILIB_TESTS
int main() {
  return wpi::StartRobot<Robot>();
}
#endif
