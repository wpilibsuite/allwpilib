// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cmath>

#include <frc/AnalogGyro.h>
#include <frc/Joystick.h>
#include <frc/TimedRobot.h>
#include <frc/drive/DifferentialDrive.h>
#include <frc/motorcontrol/PWMSparkMax.h>

/**
 * This is a sample program to demonstrate how to use a gyro sensor to make a
 * robot drive straight. This program uses a joystick to drive forwards and
 * backwards while the gyro is used for direction keeping.
 */
class Robot : public frc::TimedRobot {
 public:
  Robot() {
    m_gyro.SetSensitivity(kVoltsPerDegreePerSecond);
    // We need to invert one side of the drivetrain so that positive voltages
    // result in both sides moving forward. Depending on how your robot's
    // gearbox is constructed, you might have to invert the left side instead.
    m_right.SetInverted(true);

    wpi::SendableRegistry::AddChild(&m_drive, &m_left);
    wpi::SendableRegistry::AddChild(&m_drive, &m_right);
  }

  /**
   * The motor speed is set from the joystick while the DifferentialDrive
   * turning value is assigned from the error between the setpoint and the gyro
   * angle.
   */
  void TeleopPeriodic() override {
    double turningValue = (kAngleSetpoint - m_gyro.GetAngle()) * kP;
    m_drive.ArcadeDrive(-m_joystick.GetY(), -turningValue);
  }

 private:
  static constexpr double kAngleSetpoint = 0.0;
  static constexpr double kP = 0.005;  // Proportional turning constant

  // Gyro calibration constant, may need to be adjusted. Gyro value of 360 is
  // set to correspond to one full revolution.
  static constexpr double kVoltsPerDegreePerSecond = 0.0128;

  static constexpr int kLeftMotorPort = 0;
  static constexpr int kRightMotorPort = 1;
  static constexpr int kGyroPort = 0;
  static constexpr int kJoystickPort = 0;

  frc::PWMSparkMax m_left{kLeftMotorPort};
  frc::PWMSparkMax m_right{kRightMotorPort};
  frc::DifferentialDrive m_drive{[&](double output) { m_left.Set(output); },
                                 [&](double output) { m_right.Set(output); }};

  frc::AnalogGyro m_gyro{kGyroPort};
  frc::Joystick m_joystick{kJoystickPort};
};

#ifndef RUNNING_FRC_TESTS
int main() {
  return frc::StartRobot<Robot>();
}
#endif
