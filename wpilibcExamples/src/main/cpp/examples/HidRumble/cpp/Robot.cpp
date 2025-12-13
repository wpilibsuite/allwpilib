// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/driverstation/XboxController.hpp"
#include "wpi/framework/TimedRobot.hpp"

/**
 * This is a demo program showing the use of GenericHID's rumble feature.
 */
class Robot : public wpi::TimedRobot {
 public:
  void AutonomousInit() override {
    // Turn on rumble at the start of auto
    m_hid.SetRumble(wpi::GenericHID::RumbleType::kLeftRumble, 1.0);
    m_hid.SetRumble(wpi::GenericHID::RumbleType::kRightRumble, 1.0);
  }

  void DisabledInit() override {
    // Stop the rumble when entering disabled
    m_hid.SetRumble(wpi::GenericHID::RumbleType::kLeftRumble, 0.0);
    m_hid.SetRumble(wpi::GenericHID::RumbleType::kRightRumble, 0.0);
  }

 private:
  wpi::GenericHID m_hid{0};
};

#ifndef RUNNING_WPILIB_TESTS
int main() {
  return wpi::StartRobot<Robot>();
}
#endif
