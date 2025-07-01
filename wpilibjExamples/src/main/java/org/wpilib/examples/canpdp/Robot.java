// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.canpdp;

import org.wpilib.framework.TimedRobot;
import org.wpilib.hardware.power.PowerDistribution;
import org.wpilib.telemetry.Telemetry;

/**
 * This is a sample program showing how to retrieve information from the Power Distribution Panel
 * via CAN. The information will be displayed under variables through the SmartDashboard.
 */
public class Robot extends TimedRobot {
  private final PowerDistribution m_pdp = new PowerDistribution(0);

  public Robot() {}

  @Override
  public void robotPeriodic() {
    // Get the current going through channel 7, in Amperes.
    // The PDP returns the current in increments of 0.125A.
    // At low currents the current readings tend to be less accurate.
    double current7 = m_pdp.getCurrent(7);
    Telemetry.log("Current Channel 7", current7);

    // Get the voltage going into the PDP, in Volts.
    // The PDP returns the voltage in increments of 0.05 Volts.
    double voltage = m_pdp.getVoltage();
    Telemetry.log("Voltage", voltage);

    // Retrieves the temperature of the PDP, in degrees Celsius.
    double temperatureCelsius = m_pdp.getTemperature();
    Telemetry.log("Temperature", temperatureCelsius);

    // Get the total current of all channels.
    double totalCurrent = m_pdp.getTotalCurrent();
    Telemetry.log("Total Current", totalCurrent);

    // Get the total power of all channels.
    // Power is the bus voltage multiplied by the current with the units Watts.
    double totalPower = m_pdp.getTotalPower();
    Telemetry.log("Total Power", totalPower);

    // Get the total energy of all channels.
    // Energy is the power summed over time with units Joules.
    double totalEnergy = m_pdp.getTotalEnergy();
    Telemetry.log("Total Energy", totalEnergy);

    // Put the PDP itself to telemetry
    Telemetry.log("PDP", m_pdp);
  }
}
