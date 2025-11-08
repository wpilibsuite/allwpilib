// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hardware/rotation/Encoder.hpp"
#include "wpi/opmode/TimedRobot.hpp"
#include "wpi/util/deprecated.hpp"

/**
 * Encoder snippets for frc-docs.
 * https://docs.wpilib.org/en/stable/docs/software/hardware-apis/sensors/encoders-software.html
 */
WPI_IGNORE_DEPRECATED
class Robot : public frc::TimedRobot {
 public:
  Robot() {
    // Configures the encoder to return a distance of 4 for every 256 pulses
    // Also changes the units of getRate
    m_encoder.SetDistancePerPulse(4.0 / 256.0);
    // Configures the encoder to consider itself stopped after .1 seconds
    m_encoder.SetMaxPeriod(0.1_s);
    // Configures the encoder to consider itself stopped when its rate is below
    // 10
    m_encoder.SetMinRate(10);
    // Reverses the direction of the encoder
    m_encoder.SetReverseDirection(true);
    // Configures an encoder to average its period measurement over 5 samples
    // Can be between 1 and 127 samples
    m_encoder.SetSamplesToAverage(5);
  }

  void TeleopPeriodic() override {
    // Gets the distance traveled
    m_encoder.GetDistance();

    // Gets the current rate of the encoder
    m_encoder.GetRate();

    // Gets whether the encoder is stopped
    m_encoder.GetStopped();

    // Gets the last direction in which the encoder moved
    m_encoder.GetDirection();

    // Gets the current period of the encoder
    m_encoder.GetPeriod();

    // Resets the encoder to read a distance of zero
    m_encoder.Reset();
  }

 private:
  // Initializes an encoder on DIO pins 0 and 1
  // Defaults to 4X decoding and non-inverted
  frc::Encoder m_encoder{0, 1};

  // Initializes an encoder on DIO pins 0 and 1
  // 2X encoding and non-inverted
  frc::Encoder m_encoder2x{0, 1, false, frc::Encoder::EncodingType::k2X};
};

#ifndef RUNNING_FRC_TESTS
int main() {
  return frc::StartRobot<Robot>();
}
#endif
