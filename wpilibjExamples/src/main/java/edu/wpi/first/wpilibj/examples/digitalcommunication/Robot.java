/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.digitalcommunication;

import edu.wpi.first.wpilibj.DigitalOutput;
import edu.wpi.first.wpilibj.DriverStation;
import edu.wpi.first.wpilibj.TimedRobot;

/**
 * This is a sample program demonstrating how to communicate to a light
 * controller from the robot code using the roboRIO's DIO ports.
 */
public class Robot extends TimedRobot {
  // define ports for digitalcommunication with light controller
  private static final int kAlliancePort = 0;
  private static final int kEnabledPort = 1;
  private static final int kAutonomousPort = 2;
  private static final int kAlertPort = 3;

  private final DigitalOutput m_allianceOutput = new DigitalOutput(kAlliancePort);
  private final DigitalOutput m_enabledOutput = new DigitalOutput(kEnabledPort);
  private final DigitalOutput m_autonomousOutput = new DigitalOutput(kAutonomousPort);
  private final DigitalOutput m_alertOutput = new DigitalOutput(kAlertPort);

  @Override
  public void robotPeriodic() {
    DriverStation driverStation = DriverStation.getInstance();

    // pull alliance port high if on red alliance, pull low if on blue alliance
    m_allianceOutput.set(driverStation.getAlliance() == DriverStation.Alliance.Red);

    // pull enabled port high if enabled, low if disabled
    m_enabledOutput.set(driverStation.isEnabled());

    // pull auto port high if in autonomous, low if in teleop (or disabled)
    m_autonomousOutput.set(driverStation.isAutonomous());

    // pull alert port high if match time remaining is between 30 and 25 seconds
    m_alertOutput.set(driverStation.getMatchTime() <= 30 && driverStation.getMatchTime() >= 25);
  }
}
