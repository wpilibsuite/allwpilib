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
  private static final int kPDPId = 0;

  private final PowerDistribution m_pdp = new PowerDistribution(kPDPId);

  @Override
  public void robotPeriodic() {
    /*
     * Get the current going through channel 7, in Amperes. The PDP returns the
     * current in increments of 0.125A. At low currents
     * the current readings tend to be less accurate.
     */
    SmartDashboard.putNumber("Current Channel 7", m_pdp.getCurrent(7));

    /*
     * Get the voltage going into the PDP, in Volts.
     * The PDP returns the voltage in increments of 0.05 Volts.
     */
    SmartDashboard.putNumber("Voltage", m_pdp.getVoltage());

    /*
     * Retrieves the temperature of the PDP, in degrees Celsius.
     */
    SmartDashboard.putNumber("Temperature", m_pdp.getTemperature());
  }
}
