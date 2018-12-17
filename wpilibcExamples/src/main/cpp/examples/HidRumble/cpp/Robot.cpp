/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <frc/GenericHID.h>
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
  frc::XboxController m_hid{0};
};

#ifndef RUNNING_FRC_TESTS
int main() { return frc::StartRobot<Robot>(); }
#endif
