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
  Ultrasonic m_rangeFinder = new Ultrasonic(1, 2);

  /** Called once at the beginning of the robot program. */
  public Robot() {
    // Add the ultrasonic on the "Sensors" tab of the dashboard
    // Data will update automatically
    Shuffleboard.getTab("Sensors").add(m_rangeFinder);
  }

  @Override
  public void teleopPeriodic() {
    // We can read the distance in millimeters
    double distanceMillimeters = m_rangeFinder.getRangeMM();
    // ... or in inches
    double distanceInches = m_rangeFinder.getRangeInches();

    // We can also publish the data itself periodically
    SmartDashboard.putNumber("Distance[mm]", distanceMillimeters);
    SmartDashboard.putNumber("Distance[inch]", distanceInches);
  }

  @Override
  public void testInit() {
    // By default, the Ultrasonic class polls all ultrasonic sensors in a round-robin to prevent
    // them from interfering from one another.
    // However, manual polling is also possible -- note that this disables automatic mode!
    m_rangeFinder.ping();
  }

  @Override
  public void testPeriodic() {
    if (m_rangeFinder.isRangeValid()) {
      // Data is valid, publish it
      SmartDashboard.putNumber("Distance[mm]", m_rangeFinder.getRangeMM());
      SmartDashboard.putNumber("Distance[inch]", m_rangeFinder.getRangeInches());

      // Ping for next measurement
      m_rangeFinder.ping();
    }
  }

  @Override
  public void testExit() {
    // Enable automatic mode
    Ultrasonic.setAutomaticMode(true);
  }
}
