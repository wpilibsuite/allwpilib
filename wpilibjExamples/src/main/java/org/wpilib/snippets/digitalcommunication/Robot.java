// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.snippets.digitalcommunication;

import java.util.Optional;
import org.wpilib.driverstation.Alliance;
import org.wpilib.driverstation.MatchState;
import org.wpilib.driverstation.RobotState;
import org.wpilib.framework.TimedRobot;
import org.wpilib.hardware.discrete.DigitalOutput;

/**
 * This is a sample program demonstrating how to communicate to a light controller from the robot
 * code using the roboRIO's DIO ports.
 */
public class Robot extends TimedRobot {
  // define ports for digitalcommunication with light controller
  static final int ALLIANCE_PORT = 0;
  static final int ENABLED_PORT = 1;
  static final int AUTONOMOUS_PORT = 2;
  static final int ALERT_PORT = 3;

  private final DigitalOutput allianceOutput = new DigitalOutput(ALLIANCE_PORT);
  private final DigitalOutput enabledOutput = new DigitalOutput(ENABLED_PORT);
  private final DigitalOutput autonomousOutput = new DigitalOutput(AUTONOMOUS_PORT);
  private final DigitalOutput alertOutput = new DigitalOutput(ALERT_PORT);

  @Override
  public void robotPeriodic() {
    boolean setAlliance = false;
    Optional<Alliance> alliance = MatchState.getAlliance();
    if (alliance.isPresent()) {
      setAlliance = alliance.get() == Alliance.RED;
    }

    // pull alliance port high if on red alliance, pull low if on blue alliance
    allianceOutput.set(setAlliance);

    // pull enabled port high if enabled, low if disabled
    enabledOutput.set(RobotState.isEnabled());

    // pull auto port high if in autonomous, low if in teleop (or disabled)
    autonomousOutput.set(RobotState.isAutonomous());

    // pull alert port high if match time remaining is between 30 and 25 seconds
    var matchTime = MatchState.getMatchTime();
    alertOutput.set(matchTime <= 30 && matchTime >= 25);
  }

  /** Close all resources. */
  @Override
  public void close() {
    allianceOutput.close();
    enabledOutput.close();
    autonomousOutput.close();
    alertOutput.close();
    super.close();
  }
}
