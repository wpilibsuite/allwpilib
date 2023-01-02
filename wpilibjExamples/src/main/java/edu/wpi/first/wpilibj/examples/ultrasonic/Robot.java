// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.ultrasonic;

import edu.wpi.first.wpilibj.TimedRobot;
import edu.wpi.first.wpilibj.Ultrasonic;
import edu.wpi.first.wpilibj.shuffleboard.Shuffleboard;
import edu.wpi.first.wpilibj.smartdashboard.SmartDashboard;

/**
 * This is a sample program demonstrating how to read from a ping-response ultrasonic sensor with
 * the {@link Ultrasonic class}.
 */
public class Robot extends TimedRobot {
  // Creates a ping-response Ultrasonic object on DIO 1 and 2.
  Ultrasonic m_ultrasonic = new Ultrasonic(1, 2);

  @Override
  public void robotInit() {
    // Add the ultrasonic on the "Sensors" tab of the dashboard
    // Data will update automatically
    Shuffleboard.getTab("Sensors").add(m_ultrasonic);
  }

  @Override
  public void teleopPeriodic() {
    // We can read the distance in millimeters
    double distanceMillimeters = m_ultrasonic.getRangeMM();
    // ... or in inches
    double distanceInches = m_ultrasonic.getRangeInches();

    // Alternatively, publish the data itself periodically
    SmartDashboard.putNumber("Distance[mm]", distanceMillimeters);
    SmartDashboard.putNumber("Distance[inch]", distanceInches);
  }

  @Override
  public void testInit() {
    // By default, the Ultrasonic class polls all ultrasonic sensors in a round-robin to prevent
    // them from interfering from one another.
    // However, manual polling is also possible -- note that this disables automatic mode!
    m_ultrasonic.ping();
  }

  @Override
  public void testPeriodic() {
    if (m_ultrasonic.isRangeValid()) {
      // Data is valid, publish it
      SmartDashboard.putNumber("Distance[mm]", m_ultrasonic.getRangeMM());
      SmartDashboard.putNumber("Distance[inch]", m_ultrasonic.getRangeInches());

      // Ping for next measurement
      m_ultrasonic.ping();
    }
  }

  @Override
  public void testExit() {
    // Enable automatic mode
    Ultrasonic.setAutomaticMode(true);
  }
}
