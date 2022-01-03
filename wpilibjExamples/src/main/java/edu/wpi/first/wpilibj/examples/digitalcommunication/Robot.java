// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.digitalcommunication;

import edu.wpi.first.wpilibj.DigitalOutput;
import edu.wpi.first.wpilibj.DriverStation;
import edu.wpi.first.wpilibj.TimedRobot;

/**
 * This is a sample program demonstrating how to communicate to a light controller from the robot
 * code using the roboRIO's DIO ports.
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
    // pull alliance port high if on red alliance, pull low if on blue alliance
    m_allianceOutput.set(DriverStation.getAlliance() == DriverStation.Alliance.Red);

    // pull enabled port high if enabled, low if disabled
    m_enabledOutput.set(DriverStation.isEnabled());

    // pull auto port high if in autonomous, low if in teleop (or disabled)
    m_autonomousOutput.set(DriverStation.isAutonomous());

    // pull alert port high if match time remaining is between 30 and 25 seconds
    var matchTime = DriverStation.getMatchTime();
    m_alertOutput.set(matchTime <= 30 && matchTime >= 25);
  }
}
