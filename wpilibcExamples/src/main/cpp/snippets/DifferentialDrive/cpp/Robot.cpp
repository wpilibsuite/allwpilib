// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/drive/DifferentialDrive.hpp"
#include "wpi/driverstation/Gamepad.hpp"
#include "wpi/framework/TimedRobot.hpp"
#include "wpi/hardware/motor/PWMSparkMax.hpp"

/**
 * DifferentialDrive snippets for wpilib-docs. Runs the motors with tank
 * drive, arcade drive, and curvature drive.
 * https://docs.wpilib.org/en/stable/docs/software/hardware-apis/motors/wpi-drive-classes.html
 */
class Robot : public wpi::TimedRobot {
  wpi::PWMSparkMax leftMotor{0};
  wpi::PWMSparkMax rightMotor{1};
  wpi::DifferentialDrive robotDrive{
      [&](double output) { leftMotor.SetThrottle(output); },
      [&](double output) { rightMotor.SetThrottle(output); }};
  wpi::Gamepad driverController{0};

 public:
  Robot() {
    // We need to invert one side of the drivetrain so that positive voltages
    // result in both sides moving forward. Depending on how your robot's
    // gearbox is constructed, you might have to invert the left side instead.
    rightMotor.SetInverted(true);
  }

  void TeleopPeriodic() override {
    // Drive with split arcade style
    // That means that the Y axis of the left stick moves forward
    // and backward, and the X of the right stick turns left and right.
    robotDrive.ArcadeDrive(-driverController.GetLeftY(),
                           -driverController.GetRightX());
    // Tank drive with a given left and right rates
    robotDrive.TankDrive(-driverController.GetLeftY(),
                         -driverController.GetRightY());
    // Arcade drive with a given forward and turn rate
    robotDrive.ArcadeDrive(-driverController.GetLeftY(),
                           -driverController.GetLeftX());
    // Curvature drive with a given forward and turn rate, as well as a
    // quick-turn button
    robotDrive.CurvatureDrive(
        -driverController.GetLeftY(), -driverController.GetLeftX(),
        driverController.GetButton(wpi::Gamepad::Button::NORTH_FACE));
  }
};

#ifndef RUNNING_WPILIB_TESTS
int main() {
  return wpi::StartRobot<Robot>();
}
#endif
