// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <wpi/hardware/power/PowerDistribution.hpp>
#include <wpi/opmode/TimedRobot.hpp>
#include <wpi/smartdashboard/SmartDashboard.hpp>

/**
 * This is a sample program showing how to retrieve information from the Power
 * Distribution Panel via CAN. The information will be displayed under variables
 * through the SmartDashboard.
 */
class Robot : public wpi::TimedRobot {
 public:
  Robot() {
    // Put the PDP itself to the dashboard
    wpi::SmartDashboard::PutData("PDP", &m_pdp);
  }

  void RobotPeriodic() override {
    // Get the current going through channel 7, in Amperes.
    // The PDP returns the current in increments of 0.125A.
    // At low currents the current readings tend to be less accurate.
    double current7 = m_pdp.GetCurrent(7);
    wpi::SmartDashboard::PutNumber("Current Channel 7", current7);

    // Get the voltage going into the PDP, in Volts.
    // The PDP returns the voltage in increments of 0.05 Volts.
    double voltage = m_pdp.GetVoltage();
    wpi::SmartDashboard::PutNumber("Voltage", voltage);

    // Retrieves the temperature of the PDP, in degrees Celsius.
    double temperatureCelsius = m_pdp.GetTemperature();
    wpi::SmartDashboard::PutNumber("Temperature", temperatureCelsius);

    // Get the total current of all channels.
    double totalCurrent = m_pdp.GetTotalCurrent();
    wpi::SmartDashboard::PutNumber("Total Current", totalCurrent);

    // Get the total power of all channels.
    // Power is the bus voltage multiplied by the current with the units Watts.
    double totalPower = m_pdp.GetTotalPower();
    wpi::SmartDashboard::PutNumber("Total Power", totalPower);

    // Get the total energy of all channels.
    // Energy is the power summed over time with units Joules.
    double totalEnergy = m_pdp.GetTotalEnergy();
    wpi::SmartDashboard::PutNumber("Total Energy", totalEnergy);
  }

 private:
  // Object for dealing with the Power Distribution Panel (PDP).
  wpi::PowerDistribution m_pdp{0};
};

#ifndef RUNNING_WPILIB_TESTS
int main() {
  return wpi::StartRobot<Robot>();
}
#endif
