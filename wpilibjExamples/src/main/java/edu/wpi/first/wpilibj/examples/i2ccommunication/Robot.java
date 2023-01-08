// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.i2ccommunication;

import edu.wpi.first.wpilibj.DriverStation;
import edu.wpi.first.wpilibj.I2C;
import edu.wpi.first.wpilibj.TimedRobot;

/**
 * This is a sample program demonstrating how to communicate to a light controller from the robot
 * code using the roboRIO's I2C port.
 */
public class Robot extends TimedRobot {
  private static int kDeviceAddress = 4;

  private static I2C m_arduino = new I2C(I2C.Port.kOnboard, kDeviceAddress);

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
    m_arduino.transaction(data, data.length, null, 0);
  }

  @Override
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

    stateMessage
        .append(DriverStation.getAlliance() == DriverStation.Alliance.Red ? "R" : "B")
        .append(DriverStation.isEnabled() ? "E" : "D")
        .append(DriverStation.isAutonomous() ? "A" : "T")
        .append(String.format("%03d", (int) DriverStation.getMatchTime()));

    writeString(stateMessage.toString());
  }
}
