// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <wpi/drive/MecanumDrive.hpp>
#include <wpi/driverstation/Joystick.hpp>
#include <wpi/hardware/imu/OnboardIMU.hpp>
#include <wpi/hardware/motor/PWMSparkMax.hpp>
#include <wpi/opmode/TimedRobot.hpp>

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
  }

  /**
   * Mecanum drive is used with the gyro angle as an input.
   */
  void TeleopPeriodic() override {
    m_robotDrive.DriveCartesian(-m_joystick.GetY(), -m_joystick.GetX(),
                                -m_joystick.GetZ(), m_imu.GetRotation2d());
  }

 private:
  static constexpr int kFrontLeftMotorPort = 0;
  static constexpr int kRearLeftMotorPort = 1;
  static constexpr int kFrontRightMotorPort = 2;
  static constexpr int kRearRightMotorPort = 3;
  static constexpr frc::OnboardIMU::MountOrientation kIMUMountOrientation =
      frc::OnboardIMU::kFlat;
  static constexpr int kJoystickPort = 0;

  frc::PWMSparkMax m_frontLeft{kFrontLeftMotorPort};
  frc::PWMSparkMax m_rearLeft{kRearLeftMotorPort};
  frc::PWMSparkMax m_frontRight{kFrontRightMotorPort};
  frc::PWMSparkMax m_rearRight{kRearRightMotorPort};
  frc::MecanumDrive m_robotDrive{
      [&](double output) { m_frontLeft.Set(output); },
      [&](double output) { m_rearLeft.Set(output); },
      [&](double output) { m_frontRight.Set(output); },
      [&](double output) { m_rearRight.Set(output); }};

  frc::OnboardIMU m_imu{kIMUMountOrientation};
  frc::Joystick m_joystick{kJoystickPort};
};

#ifndef RUNNING_FRC_TESTS
int main() {
  return frc::StartRobot<Robot>();
}
#endif
