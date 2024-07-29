// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.i2ccommunication;

import edu.wpi.first.wpilibj.DriverStation;
import edu.wpi.first.wpilibj.I2C;
import edu.wpi.first.wpilibj.I2C.Port;
import edu.wpi.first.wpilibj.TimedRobot;
import java.util.Optional;

/**
 * This is a sample program demonstrating how to communicate to a light controller from the robot
 * code using the roboRIO's I2C port.
 */
public class Robot extends TimedRobot {
  static final Port kPort = Port.kOnboard;
  private static final int kDeviceAddress = 4;

  private final I2C m_arduino = new I2C(kPort, kDeviceAddress);

  private void writeString(String input) {
    // Creates a char array from the input string
    char[] chars = input.toCharArray();

    // Creates a byte array from the char array
    byte[] data = new byte[chars.length];

    // Adds each character
    for (int i = 0; i < chars.length; i++) {
      data[i] = (byte) chars[i];
    }

    // Writes bytes over I2C
    m_arduino.transaction(data, data.length, new byte[] {}, 0);
  }

  @Override
  @SuppressWarnings("PMD.ConsecutiveLiteralAppends")
  public void robotPeriodic() {
    // Creates a string to hold current robot state information, including
    // alliance, enabled state, operation mode, and match time. The message
    // is sent in format "AEM###" where A is the alliance color, (R)ed or
    // (B)lue, E is the enabled state, (E)nabled or (D)isabled, M is the
    // operation mode, (A)utonomous or (T)eleop, and ### is the zero-padded
    // time remaining in the match.
    //
    // For example, "RET043" would indicate that the robot is on the red
    // alliance, enabled in teleop mode, with 43 seconds left in the match.
    StringBuilder stateMessage = new StringBuilder(6);

    String allianceString = "U";
    Optional<DriverStation.Alliance> alliance = DriverStation.getAlliance();
    if (alliance.isPresent()) {
      allianceString = alliance.get() == DriverStation.Alliance.Red ? "R" : "B";
    }

    stateMessage
        .append(allianceString)
        .append(DriverStation.isEnabled() ? "E" : "D")
        .append(DriverStation.isAutonomous() ? "A" : "T")
        .append(String.format("%03d", (int) DriverStation.getMatchTime()));

    writeString(stateMessage.toString());
  }

  /** Close all resources. */
  @Override
  public void close() {
    m_arduino.close();
    super.close();
  }
}
