// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <wpi/drive/DifferentialDrive.hpp>
#include <wpi/driverstation/XboxController.hpp>
#include <wpi/hardware/motor/PWMSparkMax.hpp>
#include <wpi/opmode/TimedRobot.hpp>

/**
 * This is a demo program showing the use of the DifferentialDrive class.
 * Runs the motors with split arcade steering and an Xbox controller.
 */
class Robot : public wpi::TimedRobot {
  wpi::PWMSparkMax m_leftMotor{0};
  wpi::PWMSparkMax m_rightMotor{1};
  wpi::DifferentialDrive m_robotDrive{
      [&](double output) { m_leftMotor.Set(output); },
      [&](double output) { m_rightMotor.Set(output); }};
  wpi::XboxController m_driverController{0};

 public:
  Robot() {
    wpi::util::SendableRegistry::AddChild(&m_robotDrive, &m_leftMotor);
    wpi::util::SendableRegistry::AddChild(&m_robotDrive, &m_rightMotor);

    // We need to invert one side of the drivetrain so that positive voltages
    // result in both sides moving forward. Depending on how your robot's
    // gearbox is constructed, you might have to invert the left side instead.
    m_rightMotor.SetInverted(true);
  }

  void TeleopPeriodic() override {
    // Drive with split arcade style
    // That means that the Y axis of the left stick moves forward
    // and backward, and the X of the right stick turns left and right.
    m_robotDrive.ArcadeDrive(-m_driverController.GetLeftY(),
                             -m_driverController.GetRightX());
  }
};

#ifndef RUNNING_WPILIB_TESTS
int main() {
  return wpi::StartRobot<Robot>();
}
#endif
