// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hardware/discrete/DigitalInput.hpp"
#include "wpi/hardware/rotation/Encoder.hpp"
#include "wpi/opmode/TimedRobot.hpp"
#include "wpi/hardware/motor/Spark.hpp"

/**
 * Encoder mechanism homing snippets for frc-docs.
 * https://docs.wpilib.org/en/stable/docs/software/hardware-apis/sensors/encoders-software.html
 */
class Robot : public frc::TimedRobot {
 public:
  void AutonomousPeriodic() override {
    // Runs the motor backwards at half speed until the limit switch is pressed
    // then turn off the motor and reset the encoder
    if (!m_limit.Get()) {
      m_spark.Set(-0.5);
    } else {
      m_spark.Set(0);
      m_encoder.Reset();
    }
  }

 private:
  frc::Encoder m_encoder{0, 1};
  frc::Spark m_spark{0};
  // Limit switch on DIO 2
  frc::DigitalInput m_limit{2};
};

#ifndef RUNNING_FRC_TESTS
int main() {
  return frc::StartRobot<Robot>();
}
#endif
