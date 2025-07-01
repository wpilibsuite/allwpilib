// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/framework/TimedRobot.hpp"
#include "wpi/hardware/power/PowerDistribution.hpp"
#include "wpi/telemetry/Telemetry.hpp"

/**
 * This is a sample program showing how to retrieve information from the Power
 * Distribution Panel via CAN. The information will be displayed under variables
 * through the SmartDashboard.
 */
class Robot : public wpi::TimedRobot {
 public:
  Robot() {}

  void RobotPeriodic() override {
    // Get the current going through channel 7, in Amperes.
    // The PDP returns the current in increments of 0.125A.
    // At low currents the current readings tend to be less accurate.
    double current7 = m_pdp.GetCurrent(7);
    wpi::Telemetry::Log("Current Channel 7", current7);

    // Get the voltage going into the PDP, in Volts.
    // The PDP returns the voltage in increments of 0.05 Volts.
    double voltage = m_pdp.GetVoltage();
    wpi::Telemetry::Log("Voltage", voltage);

    // Retrieves the temperature of the PDP, in degrees Celsius.
    double temperatureCelsius = m_pdp.GetTemperature();
    wpi::Telemetry::Log("Temperature", temperatureCelsius);

    // Get the total current of all channels.
    double totalCurrent = m_pdp.GetTotalCurrent();
    wpi::Telemetry::Log("Total Current", totalCurrent);

    // Get the total power of all channels.
    // Power is the bus voltage multiplied by the current with the units Watts.
    double totalPower = m_pdp.GetTotalPower();
    wpi::Telemetry::Log("Total Power", totalPower);

    // Get the total energy of all channels.
    // Energy is the power summed over time with units Joules.
    double totalEnergy = m_pdp.GetTotalEnergy();
    wpi::Telemetry::Log("Total Energy", totalEnergy);

    // Put the PDP itself to the dashboard
    wpi::Telemetry::Log("PDP", m_pdp);
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
