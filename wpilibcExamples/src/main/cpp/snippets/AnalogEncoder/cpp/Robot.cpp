// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <frc/AnalogEncoder.h>
#include <frc/TimedRobot.h>

/**
 * AnalogEncoder snippets for frc-docs.
 * https://docs.wpilib.org/en/stable/docs/software/hardware-apis/sensors/encoders-software.html
 */
class Robot : public frc::TimedRobot {
 public:
  Robot() {}

  void TeleopPeriodic() override {
    // Gets the rotation
    m_encoder.Get();
  }

 private:
  // Initializes an analog encoder on Analog Input pin 0
  frc::AnalogEncoder m_encoder{0};

  // Initializes an analog encoder on DIO pins 0 to return a value of 4 for
  // a full rotation, with the encoder reporting 0 half way through rotation (2
  // out of 4)
  frc::AnalogEncoder m_encoderFR{0, 4.0, 2.0};
};

#ifndef RUNNING_FRC_TESTS
int main() {
  return frc::StartRobot<Robot>();
}
#endif
