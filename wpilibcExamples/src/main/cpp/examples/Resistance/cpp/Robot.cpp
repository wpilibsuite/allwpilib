// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <frc/PowerDistribution.h>
#include <frc/ResistanceCalculator.h>
#include <frc/RobotController.h>
#include <frc/TimedRobot.h>
#include <frc/motorcontrol/PWMSparkMax.h>
#include <frc/smartdashboard/SmartDashboard.h>

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
    frc::SmartDashboard::PutData(&m_powerDistribution);
  }

  void TeleopPeriodic() override {
    // Get the current for channel 1
    units::ampere_t chan1Current{m_powerDistribution.GetCurrent(kChannel)};

    // Get the voltage given to the motor plugged into channel 1
    units::volt_t chan1Voltage{m_motor.Get() *
                               frc::RobotController::GetBatteryVoltage()};

    // Calculate the channel's resistance based on that current and voltage
    units::ohm_t resistance =
        m_resistCalc1.Calculate(chan1Current, chan1Voltage).value();

    // Log the resistance
    frc::SmartDashboard::PutNumber("Channel 1 resistance", resistance.value());
    frc::SmartDashboard::PutNumber(
        "Robot resistance", m_powerDistribution.GetTotalResistance().value());
  }

 private:
  /** The channel on the PDP whose resistance will be logged in this example. */
  static const int kChannel = 1;

  /**
   * Object representing the PDP or PDH on the robot. The PowerDistribution
   * class implements Sendable and logs the robot resistance. It can also be
   * used to get the current flowing through a particular channel.
   */
  frc::PowerDistribution m_powerDistribution;

  /** Used to calculate the resistance of channel 1. */
  frc::ResistanceCalculator m_resistCalc1;

  /** The motor plugged into channel 1. */
  frc::PWMSparkMax m_motor{0};
};

#ifndef RUNNING_FRC_TESTS
int main() {
  return frc::StartRobot<Robot>();
}
#endif
