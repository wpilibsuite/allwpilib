// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <numbers>

#include "wpi/framework/TimedRobot.hpp"
#include "wpi/hardware/rotation/Encoder.hpp"
#include "wpi/telemetry/Telemetry.hpp"

/**
 * Sample program displaying the value of a quadrature encoder on a
 * dashboard.
 *
 * Quadrature Encoders are digital sensors which can detect the amount the
 * encoder has rotated since starting as well as the direction in which the
 * encoder shaft is rotating. However, encoders can not tell you the absolute
 * position of the encoder shaft (ie, it considers where it starts to be the
 * zero position, no matter where it starts), and so can only tell you how much
 * the encoder has rotated since starting.
 *
 * Depending on the precision of an encoder, it will have fewer or greater ticks
 * per revolution; the number of ticks per revolution will affect the conversion
 * between ticks and distance, as specified by DistancePerPulse.
 *
 * One of the most common uses of encoders is in the drivetrain, so that the
 * distance that the robot drives can be precisely controlled during the
 * autonomous mode.
 */
class Robot : public wpi::TimedRobot {
 public:
  Robot() {
    /* Defines how far the mechanism attached to the encoder moves per pulse. In
     * this case, we assume that a 360 count encoder is directly attached to a 3
     * inch diameter (1.5inch radius) wheel, and that we want to measure
     * distance in inches.
     */
    encoder.SetDistancePerPulse(1.0 / 360.0 * 2.0 * std::numbers::pi * 1.5);
  }

  void TeleopPeriodic() override {
    // Retrieve the net displacement of the Encoder since the last Reset.
    wpi::Telemetry::Log("Encoder Distance", encoder.GetDistance());

    // Retrieve the current rate of the encoder.
    wpi::Telemetry::Log("Encoder Rate", encoder.GetRate());
  }

 private:
  /**
   * The Encoder object is constructed with 4 parameters, the last two being
   * optional.
   *
   * The first two parameters (2, 3 in this case) refer to the ports on the
   * roboRIO which the encoder uses. Because a quadrature encoder has two signal
   * wires, the signal from two DIO ports on the roboRIO are used.
   *
   * The third (optional) parameter is a boolean which defaults to false. If you
   * set this parameter to true, the direction of the encoder will  be reversed,
   * in case it makes more sense mechanically.
   *
   * The final (optional) parameter specifies encoding rate (X1, X2, or X4)
   * and defaults to X4. Faster (X4) encoding gives greater positional
   * precision but more noise in the rate.
   */
  wpi::Encoder encoder{2, 3, false, wpi::Encoder::EncodingType::X4};
};

#ifndef RUNNING_WPILIB_TESTS
int main() {
  return wpi::StartRobot<Robot>();
}
#endif
