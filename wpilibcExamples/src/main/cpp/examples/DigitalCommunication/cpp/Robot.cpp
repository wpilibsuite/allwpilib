/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <frc/DigitalOutput.h>
#include <frc/TimedRobot.h>
#include <frc/DriverStation.h>

/**
 * This is a sample program demonstrating how to communicate to a light
 * controller from the robot code using the roboRIO's DIO ports.
 */
class Robot : public frc::TimedRobot {
 public:
  void RobotPeriodic() override {
    frc::DriverStation& ds = frc::DriverStation::GetInstance();

    // pull alliance port high if on red alliance, pull low if on blue alliance
    m_allianceOutput.Set(ds.GetAlliance() == frc::DriverStation::kRed);

    // pull enabled port high if enabled, low if disabled
    m_enabledOutput.Set(ds.IsEnabled());

    // pull auto port high if in autonomous, low if in teleop (or disabled)
    m_autonomousOutput.Set(ds.IsAutonomous());

    // pull alert port high if match time remaining is between 30 and 25 seconds
    m_alertOutput.Set(ds.GetMatchTime() <= 30 && ds.GetMatchTime() >= 25);
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

int main() { return frc::StartRobot<Robot>(); }
