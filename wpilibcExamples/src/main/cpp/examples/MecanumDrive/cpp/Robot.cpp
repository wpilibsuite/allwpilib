// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <frc/Joystick.h>
#include <frc/TimedRobot.h>
#include <frc/drive/MecanumDrive.h>
#include <frc/motorcontrol/PWMSparkMax.h>

/**
 * This is a demo program showing how to use Mecanum control with the
 * MecanumDrive class.
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

  void TeleopPeriodic() override {
    /* Use the joystick Y axis for forward movement, X axis for lateral
     * movement, and Z axis for rotation.
     */
    m_robotDrive.DriveCartesian(-m_stick.GetY(), -m_stick.GetX(),
                                -m_stick.GetZ());
  }

 private:
  static constexpr int FRONT_LEFT_CHANNEL = 0;
  static constexpr int REAR_LEFT_CHANNEL = 1;
  static constexpr int FRONT_RIGHT_CHANNEL = 2;
  static constexpr int REAR_RIGHT_CHANNEL = 3;

  static constexpr int JOYSTICK_CHANNEL = 0;

  frc::PWMSparkMax m_frontLeft{FRONT_LEFT_CHANNEL};
  frc::PWMSparkMax m_rearLeft{REAR_LEFT_CHANNEL};
  frc::PWMSparkMax m_frontRight{FRONT_RIGHT_CHANNEL};
  frc::PWMSparkMax m_rearRight{REAR_RIGHT_CHANNEL};
  frc::MecanumDrive m_robotDrive{
      [&](double output) { m_frontLeft.Set(output); },
      [&](double output) { m_rearLeft.Set(output); },
      [&](double output) { m_frontRight.Set(output); },
      [&](double output) { m_rearRight.Set(output); }};

  frc::Joystick m_stick{JOYSTICK_CHANNEL};
};

#ifndef RUNNING_FRC_TESTS
int main() {
  return frc::StartRobot<Robot>();
}
#endif
