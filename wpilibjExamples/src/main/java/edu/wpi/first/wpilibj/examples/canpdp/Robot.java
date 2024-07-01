// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.canpdp;

import edu.wpi.first.wpilibj.PowerDistribution;
import edu.wpi.first.wpilibj.TimedRobot;
import edu.wpi.first.wpilibj.smartdashboard.SmartDashboard;

/**
 * This is a sample program showing how to retrieve information from the Power Distribution Panel
 * via CAN. The information will be displayed under variables through the SmartDashboard.
 */
public class Robot extends TimedRobot {
  private final PowerDistribution m_pdp = new PowerDistribution();

  public Robot() {
    // Put the PDP itself to the dashboard
    SmartDashboard.putData("PDP", m_pdp);
  }

  @Override
  public void robotPeriodic() {
    // Get the current going through channel 7, in Amperes.
    // The PDP returns the current in increments of 0.125A.
    // At low currents the current readings tend to be less accurate.
    double current7 = m_pdp.getCurrent(7);
    SmartDashboard.putNumber("Current Channel 7", current7);

    // Get the voltage going into the PDP, in Volts.
    // The PDP returns the voltage in increments of 0.05 Volts.
    double voltage = m_pdp.getVoltage();
    SmartDashboard.putNumber("Voltage", voltage);

    // Retrieves the temperature of the PDP, in degrees Celsius.
    double temperatureCelsius = m_pdp.getTemperature();
    SmartDashboard.putNumber("Temperature", temperatureCelsius);

    // Get the total current of all channels.
    double totalCurrent = m_pdp.getTotalCurrent();
    SmartDashboard.putNumber("Total Current", totalCurrent);

    // Get the total power of all channels.
    // Power is the bus voltage multiplied by the current with the units Watts.
    double totalPower = m_pdp.getTotalPower();
    SmartDashboard.putNumber("Total Power", totalPower);

    // Get the total energy of all channels.
    // Energy is the power summed over time with units Joules.
    double totalEnergy = m_pdp.getTotalEnergy();
    SmartDashboard.putNumber("Total Energy", totalEnergy);
  }
}
