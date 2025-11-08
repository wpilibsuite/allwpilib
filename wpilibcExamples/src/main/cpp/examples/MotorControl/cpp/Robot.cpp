// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <numbers>

#include "wpi/driverstation/Joystick.hpp"
#include "wpi/framework/TimedRobot.hpp"
#include "wpi/hardware/motor/PWMSparkMax.hpp"
#include "wpi/hardware/rotation/Encoder.hpp"
#include "wpi/smartdashboard/SmartDashboard.hpp"

/**
 * This sample program shows how to control a motor using a joystick. In the
 * operator control part of the program, the joystick is read and the value is
 * written to the motor.
 *
 * Joystick analog values range from -1 to 1 and motor controller inputs as
 * range from -1 to 1 making it easy to work together.
 *
 * In addition, the encoder value of an encoder connected to ports 0 and 1 is
 * consistently sent to the Dashboard.
 */
class Robot : public wpi::TimedRobot {
 public:
  void TeleopPeriodic() override { m_motor.Set(m_stick.GetY()); }

  /*
   * The RobotPeriodic function is called every control packet no matter the
   * robot mode.
   */
  void RobotPeriodic() override {
    wpi::SmartDashboard::PutNumber("Encoder", m_encoder.GetDistance());
  }

  Robot() {
    // Use SetDistancePerPulse to set the multiplier for GetDistance
    // This is set up assuming a 6 inch wheel with a 360 CPR encoder.
    m_encoder.SetDistancePerPulse((std::numbers::pi * 6) / 360.0);
  }

 private:
  wpi::Joystick m_stick{0};
  wpi::PWMSparkMax m_motor{0};
  wpi::Encoder m_encoder{0, 1};
};

#ifndef RUNNING_WPILIB_TESTS
int main() {
  return wpi::StartRobot<Robot>();
}
#endif
