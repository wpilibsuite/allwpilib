// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.digitalcommunication;

import java.util.Optional;
import org.wpilib.driverstation.DriverStation;
import org.wpilib.framework.TimedRobot;
import org.wpilib.hardware.discrete.DigitalOutput;

/**
 * This is a sample program demonstrating how to communicate to a light controller from the robot
 * code using the roboRIO's DIO ports.
 */
public class Robot extends TimedRobot {
  // define ports for digitalcommunication with light controller
  static final int kAlliancePort = 0;
  static final int kEnabledPort = 1;
  static final int kAutonomousPort = 2;
  static final int kAlertPort = 3;

  private final DigitalOutput m_allianceOutput = new DigitalOutput(kAlliancePort);
  private final DigitalOutput m_enabledOutput = new DigitalOutput(kEnabledPort);
  private final DigitalOutput m_autonomousOutput = new DigitalOutput(kAutonomousPort);
  private final DigitalOutput m_alertOutput = new DigitalOutput(kAlertPort);

  @Override
  public void robotPeriodic() {
    boolean setAlliance = false;
    Optional<DriverStation.Alliance> alliance = DriverStation.getAlliance();
    if (alliance.isPresent()) {
      setAlliance = alliance.get() == DriverStation.Alliance.Red;
    }

    // pull alliance port high if on red alliance, pull low if on blue alliance
    m_allianceOutput.set(setAlliance);

    // pull enabled port high if enabled, low if disabled
    m_enabledOutput.set(DriverStation.isEnabled());

    // pull auto port high if in autonomous, low if in teleop (or disabled)
    m_autonomousOutput.set(DriverStation.isAutonomous());

    // pull alert port high if match time remaining is between 30 and 25 seconds
    var matchTime = DriverStation.getMatchTime();
    m_alertOutput.set(matchTime <= 30 && matchTime >= 25);
  }

  /** Close all resources. */
  @Override
  public void close() {
    m_allianceOutput.close();
    m_enabledOutput.close();
    m_autonomousOutput.close();
    m_alertOutput.close();
    super.close();
  }
}
