// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <frc/DigitalOutput.h>
#include <frc/DriverStation.h>
#include <frc/TimedRobot.h>

/**
 * This is a sample program demonstrating how to communicate to a light
 * controller from the robot code using the roboRIO's DIO ports.
 */
class Robot : public frc::TimedRobot {
 public:
  void RobotPeriodic() override {
    // pull alliance port high if on red alliance, pull low if on blue alliance
    m_allianceOutput.Set(frc::DriverStation::GetAlliance() ==
                         frc::DriverStation::kRed);

    // pull enabled port high if enabled, low if disabled
    m_enabledOutput.Set(frc::DriverStation::IsEnabled());

    // pull auto port high if in autonomous, low if in teleop (or disabled)
    m_autonomousOutput.Set(frc::DriverStation::IsAutonomous());

    // pull alert port high if match time remaining is between 30 and 25 seconds
    auto matchTime = frc::DriverStation::GetMatchTime();
    m_alertOutput.Set(matchTime <= 30 && matchTime >= 25);
  }

 private:
  // define ports for communication with light controller
  static constexpr int kAlliancePort = 0;
  static constexpr int kEnabledPort = 1;
  static constexpr int kAutonomousPort = 2;
  static constexpr int kAlertPort = 3;

  frc::DigitalOutput m_allianceOutput{kAlliancePort};
  frc::DigitalOutput m_enabledOutput{kEnabledPort};
  frc::DigitalOutput m_autonomousOutput{kAutonomousPort};
  frc::DigitalOutput m_alertOutput{kAlertPort};
};

int main() {
  return frc::StartRobot<Robot>();
}
