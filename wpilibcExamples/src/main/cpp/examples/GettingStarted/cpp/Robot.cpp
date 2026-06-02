// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/drive/DifferentialDrive.hpp"
#include "wpi/driverstation/Gamepad.hpp"
#include "wpi/framework/TimedRobot.hpp"
#include "wpi/hardware/motor/PWMSparkMax.hpp"
#include "wpi/system/Timer.hpp"

class Robot : public wpi::TimedRobot {
 public:
  Robot() {
    wpi::util::SendableRegistry::AddChild(&robotDrive, &left);
    wpi::util::SendableRegistry::AddChild(&robotDrive, &right);

    // We need to invert one side of the drivetrain so that positive voltages
    // result in both sides moving forward. Depending on how your robot's
    // gearbox is constructed, you might have to invert the left side instead.
    right.SetInverted(true);
    robotDrive.SetExpiration(100_ms);
    timer.Start();
  }

  void AutonomousInit() override { timer.Restart(); }

  void AutonomousPeriodic() override {
    // Drive for 2 seconds
    if (timer.Get() < 2_s) {
      // Drive forwards half velocity, make sure to turn input squaring off
      robotDrive.ArcadeDrive(0.5, 0.0, false);
    } else {
      // Stop robot
      robotDrive.ArcadeDrive(0.0, 0.0, false);
    }
  }

  void TeleopInit() override {}

  void TeleopPeriodic() override {
    // Drive with arcade style (use right stick to steer)
    robotDrive.ArcadeDrive(-controller.GetLeftY(), controller.GetRightX());
  }

  void UtilityInit() override {}

  void UtilityPeriodic() override {}

 private:
  // Robot drive system
  wpi::PWMSparkMax left{0};
  wpi::PWMSparkMax right{1};
  wpi::DifferentialDrive robotDrive{
      [&](double output) { left.SetThrottle(output); },
      [&](double output) { right.SetThrottle(output); }};

  wpi::Gamepad controller{0};
  wpi::Timer timer;
};

#ifndef RUNNING_WPILIB_TESTS
int main() {
  return wpi::StartRobot<Robot>();
}
#endif
