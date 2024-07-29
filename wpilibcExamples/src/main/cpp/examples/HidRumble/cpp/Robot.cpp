// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <frc/TimedRobot.h>
#include <frc/XboxController.h>

/**
 * This is a demo program showing the use of GenericHID's rumble feature.
 */
class Robot : public frc::TimedRobot {
 public:
  void AutonomousInit() override {
    // Turn on rumble at the start of auto
    m_hid.SetRumble(frc::GenericHID::RumbleType::kLeftRumble, 1.0);
    m_hid.SetRumble(frc::GenericHID::RumbleType::kRightRumble, 1.0);
  }

  void DisabledInit() override {
    // Stop the rumble when entering disabled
    m_hid.SetRumble(frc::GenericHID::RumbleType::kLeftRumble, 0.0);
    m_hid.SetRumble(frc::GenericHID::RumbleType::kRightRumble, 0.0);
  }

 private:
  frc::GenericHID m_hid{0};
};

#ifndef RUNNING_FRC_TESTS
int main() {
  return frc::StartRobot<Robot>();
}
#endif
