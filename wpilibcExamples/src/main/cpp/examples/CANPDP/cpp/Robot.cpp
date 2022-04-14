// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <frc/PowerDistribution.h>
#include <frc/TimedRobot.h>
#include <frc/smartdashboard/SmartDashboard.h>

/**
 * This is a sample program showing how to retrieve information from the Power
 * Distribution Panel via CAN. The information will be displayed under variables
 * through the SmartDashboard.
 */
class Robot : public frc::TimedRobot {
 public:
  void TeleopPeriodic() override {
    /* Get the current going through channel 7, in Amperes. The PDP returns the
     * current in increments of 0.125A. At low currents the current readings
     * tend to be less accurate.
     */
    frc::SmartDashboard::PutNumber("Current Channel 7", m_pdp.GetCurrent(7));

    /* Get the voltage going into the PDP, in Volts. The PDP returns the voltage
     * in increments of 0.05 Volts.
     */
    frc::SmartDashboard::PutNumber("Voltage", m_pdp.GetVoltage());

    // Retrieves the temperature of the PDP, in degrees Celsius.
    frc::SmartDashboard::PutNumber("Temperature", m_pdp.GetTemperature());
  }

 private:
  // Object for dealing with the Power Distribution Panel (PDP).
  frc::PowerDistribution m_pdp;
};

#ifndef RUNNING_FRC_TESTS
int main() {
  return frc::StartRobot<Robot>();
}
#endif
