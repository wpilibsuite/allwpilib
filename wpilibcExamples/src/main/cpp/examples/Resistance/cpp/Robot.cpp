// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <frc/PowerDistribution.h>
#include <frc/ResistanceCalculator.h>
#include <frc/TimedRobot.h>
#include <frc/smartdashboard/SmartDashboard.h>
#include <wpi/numbers>

/**
 * Sample program to demonstrate logging the resistance of a particular PDP/PDH
 * channel and the robot's resistance. The resistance can be calculated by
 * recording the current and voltage of a particular channel and doing a linear
 * regression on that data.
 */
class Robot : public frc::TimedRobot {
 public:
  Robot() {
    // Display the PowerDistribution on the dashboard so that the robot
    // resistance can be seen.
    frc::SmartDashboard::PutData(m_powerDistribution);
  }

  void TeleopPeriodic() override {
    auto chan1Current = m_powerDistribution.getCurrent(kChannel);
    // Get the voltage given to the motor plugged into channel 1.
    auto chan1Voltage = m_motor.get() * RobotController.getBatteryVoltage();

    // Calculate and log channel 1's resistance
    frc::SmartDashboard::PutNumber(
        "Channel 1 resistance",
        m_resistCalc.Calculate(chan1Current, chan1Voltage));
  }

 private:
  /** The channel on the PDP whose resistance will be logged in this example. */
  static const int kChannel = 1;

  /**
   * Object representing the PDP or PDH on the robot. The PowerDistribution
   * class implements Sendable and logs the robot resistance. It can also be
   * used to get the current flowing through a particular channel.
   */
  PowerDistribution m_powerDistribution;

  /** Used to calculate the resistance of channel 1. */
  ResistanceCalculator m_resistCalc;

  /** The motor plugged into channel 1. */
  PWMSparkMax m_motor(0);
};

#ifndef RUNNING_FRC_TESTS
int main() {
  return frc::StartRobot<Robot>();
}
#endif
