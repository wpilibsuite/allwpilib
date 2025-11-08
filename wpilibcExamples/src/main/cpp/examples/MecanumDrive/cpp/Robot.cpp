// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <wpi/drive/MecanumDrive.hpp>
#include <wpi/driverstation/Joystick.hpp>
#include <wpi/framework/TimedRobot.hpp>
#include <wpi/hardware/motor/PWMSparkMax.hpp>

/**
 * This is a demo program showing how to use Mecanum control with the
 * MecanumDrive class.
 */
class Robot : public wpi::TimedRobot {
 public:
  Robot() {
    wpi::util::SendableRegistry::AddChild(&m_robotDrive, &m_frontLeft);
    wpi::util::SendableRegistry::AddChild(&m_robotDrive, &m_rearLeft);
    wpi::util::SendableRegistry::AddChild(&m_robotDrive, &m_frontRight);
    wpi::util::SendableRegistry::AddChild(&m_robotDrive, &m_rearRight);

    // Invert the right side motors. You may need to change or remove this to
    // match your robot.
    m_frontRight.SetInverted(true);
    m_rearRight.SetInverted(true);
  }

  void TeleopPeriodic() override {
    /* Use the joystick Y axis for forward movement, X axis for lateral
     * movement, and Z axis for rotation.
     */
    m_robotDrive.DriveCartesian(-m_stick.GetY(), -m_stick.GetX(),
                                -m_stick.GetZ());
  }

 private:
  static constexpr int kFrontLeftChannel = 0;
  static constexpr int kRearLeftChannel = 1;
  static constexpr int kFrontRightChannel = 2;
  static constexpr int kRearRightChannel = 3;

  static constexpr int kJoystickChannel = 0;

  wpi::PWMSparkMax m_frontLeft{kFrontLeftChannel};
  wpi::PWMSparkMax m_rearLeft{kRearLeftChannel};
  wpi::PWMSparkMax m_frontRight{kFrontRightChannel};
  wpi::PWMSparkMax m_rearRight{kRearRightChannel};
  wpi::MecanumDrive m_robotDrive{
      [&](double output) { m_frontLeft.Set(output); },
      [&](double output) { m_rearLeft.Set(output); },
      [&](double output) { m_frontRight.Set(output); },
      [&](double output) { m_rearRight.Set(output); }};

  wpi::Joystick m_stick{kJoystickChannel};
};

#ifndef RUNNING_WPILIB_TESTS
int main() {
  return wpi::StartRobot<Robot>();
}
#endif
