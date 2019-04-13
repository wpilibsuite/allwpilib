/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <frc/Encoder.h>
#include <frc/Joystick.h>
#include <frc/PWMVictorSPX.h>
#include <frc/TimedRobot.h>
#include <frc/smartdashboard/SmartDashboard.h>

constexpr double kPi = 3.14159265358979;

/**
 * This sample program shows how to control a motor using a joystick. In the
 * operator control part of the program, the joystick is read and the value is
 * written to the motor.
 *
 * Joystick analog values range from -1 to 1 and speed controller inputs as
 * range from -1 to 1 making it easy to work together.
 *
 * In addition, the encoder value of an encoder connected to ports 0 and 1 is
 * consistently sent to the Dashboard.
 */
class Robot : public frc::TimedRobot {
 public:
  void TeleopPeriodic() override { m_motor.Set(m_stick.GetY()); }

  /*
   * The RobotPeriodic function is called every control packet no matter the
   * robot mode.
   */
  void RobotPeriodic() override {
    frc::SmartDashboard::PutNumber("Encoder", m_encoder.GetDistance());
  }

  void RobotInit() override {
    // Use SetDistancePerPulse to set the multiplier for GetDistance
    // This is set up assuming a 6 inch wheel with a 360 CPR encoder.
    m_encoder.SetDistancePerPulse((kPi * 6) / 360.0);
  }

 private:
  frc::Joystick m_stick{0};
  frc::PWMVictorSPX m_motor{0};
  frc::Encoder m_encoder{0, 1};
};

#ifndef RUNNING_FRC_TESTS
int main() { return frc::StartRobot<Robot>(); }
#endif
