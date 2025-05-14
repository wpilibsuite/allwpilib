// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <frc/AnalogGyro.h>
#include <frc/Joystick.h>
#include <frc/TimedRobot.h>
#include <frc/drive/MecanumDrive.h>
#include <frc/motorcontrol/PWMSparkMax.h>

/**
 * This is a sample program that uses mecanum drive with a gyro sensor to
 * maintain rotation vectors in relation to the starting orientation of the
 * robot (field-oriented controls).
 */
class Robot : public frc::TimedRobot {
 public:
  Robot() {
    wpi::SendableRegistry::AddChild(&m_robotDrive, &m_frontLeft);
    wpi::SendableRegistry::AddChild(&m_robotDrive, &m_rearLeft);
    wpi::SendableRegistry::AddChild(&m_robotDrive, &m_frontRight);
    wpi::SendableRegistry::AddChild(&m_robotDrive, &m_rearRight);

    // Invert the right side motors. You may need to change or remove this to
    // match your robot.
    m_frontRight.SetInverted(true);
    m_rearRight.SetInverted(true);

    m_gyro.SetSensitivity(VOLTS_PER_DEGREE_PER_SECOND);
  }

  /**
   * Mecanum drive is used with the gyro angle as an input.
   */
  void TeleopPeriodic() override {
    m_robotDrive.DriveCartesian(-m_joystick.GetY(), -m_joystick.GetX(),
                                -m_joystick.GetZ(), m_gyro.GetRotation2d());
  }

 private:
  // Gyro calibration constant, may need to be adjusted. Gyro value of 360 is
  // set to correspond to one full revolution.
  static constexpr double VOLTS_PER_DEGREE_PER_SECOND = 0.0128;

  static constexpr int FRONT_LEFT_MOTOR_PORT = 0;
  static constexpr int READ_LEFT_MOTOR_PORT = 1;
  static constexpr int FRONT_RIGHT_MOTOR_PORT = 2;
  static constexpr int REAR_RIGHT_MOTOR_PORT = 3;
  static constexpr int GYRO_PORT = 0;
  static constexpr int JOYSTICK_PORT = 0;

  frc::PWMSparkMax m_frontLeft{FRONT_LEFT_MOTOR_PORT};
  frc::PWMSparkMax m_rearLeft{READ_LEFT_MOTOR_PORT};
  frc::PWMSparkMax m_frontRight{FRONT_RIGHT_MOTOR_PORT};
  frc::PWMSparkMax m_rearRight{REAR_RIGHT_MOTOR_PORT};
  frc::MecanumDrive m_robotDrive{
      [&](double output) { m_frontLeft.Set(output); },
      [&](double output) { m_rearLeft.Set(output); },
      [&](double output) { m_frontRight.Set(output); },
      [&](double output) { m_rearRight.Set(output); }};

  frc::AnalogGyro m_gyro{GYRO_PORT};
  frc::Joystick m_joystick{JOYSTICK_PORT};
};

#ifndef RUNNING_FRC_TESTS
int main() {
  return frc::StartRobot<Robot>();
}
#endif
