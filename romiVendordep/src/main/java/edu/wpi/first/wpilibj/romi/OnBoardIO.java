// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.romi;

import edu.wpi.first.wpilibj.DigitalInput;
import edu.wpi.first.wpilibj.DigitalOutput;
import edu.wpi.first.wpilibj.DriverStation;
import edu.wpi.first.wpilibj.Timer;

/**
 * This class represents the onboard IO of the Romi reference robot. This includes the pushbuttons
 * and LEDs.
 *
 * <p>DIO 0 - Button A (input only) DIO 1 - Button B (input) or Green LED (output) DIO 2 - Button C
 * (input) or Red LED (output) DIO 3 - Yellow LED (output only)
 */
public class OnBoardIO {
  private final DigitalInput m_buttonA = new DigitalInput(0);
  private final DigitalOutput m_yellowLed = new DigitalOutput(3);

  // DIO 1
  private final DigitalInput m_buttonB;
  private final DigitalOutput m_greenLed;

  // DIO 2
  private final DigitalInput m_buttonC;
  private final DigitalOutput m_redLed;

  private static final double MESSAGE_INTERVAL = 1.0;
  private double m_nextMessageTime;

  /** Mode for Romi onboard IO channel. */
  public enum ChannelMode {
    /** Input. */
    INPUT,
    /** Output. */
    OUTPUT
  }

  /**
   * Constructor.
   *
   * @param dio1 Mode for DIO 1 (input = Button B, output = green LED)
   * @param dio2 Mode for DIO 2 (input = Button C, output = red LED)
   */
  public OnBoardIO(ChannelMode dio1, ChannelMode dio2) {
    if (dio1 == ChannelMode.INPUT) {
      m_buttonB = new DigitalInput(1);
      m_greenLed = null;
    } else {
      m_greenLed = new DigitalOutput(1);
      m_buttonB = null;
    }

    if (dio2 == ChannelMode.INPUT) {
      m_buttonC = new DigitalInput(2);
      m_redLed = null;
    } else {
      m_redLed = new DigitalOutput(2);
      m_buttonC = null;
    }
  }

  /**
   * Gets if the A button is pressed.
   *
   * @return Whether or not Button A is pressed
   */
  public boolean getButtonAPressed() {
    return m_buttonA.get();
  }

  /**
   * Gets if the B button is pressed.
   *
   * @return Whether or not Button B is pressed
   */
  public boolean getButtonBPressed() {
    if (m_buttonB != null) {
      return m_buttonB.get();
    }

    double currentTime = Timer.getTimestamp();
    if (currentTime > m_nextMessageTime) {
      DriverStation.reportError("Button B was not configured", true);
      m_nextMessageTime = currentTime + MESSAGE_INTERVAL;
    }
    return false;
  }

  /**
   * Gets if the C button is pressed.
   *
   * @return Whether or not Button C is pressed
   */
  public boolean getButtonCPressed() {
    if (m_buttonC != null) {
      return m_buttonC.get();
    }

    double currentTime = Timer.getTimestamp();
    if (currentTime > m_nextMessageTime) {
      DriverStation.reportError("Button C was not configured", true);
      m_nextMessageTime = currentTime + MESSAGE_INTERVAL;
    }
    return false;
  }

  /**
   * Sets the green LED.
   *
   * @param value Set whether or not to turn the Green LED on
   */
  public void setGreenLed(boolean value) {
    if (m_greenLed != null) {
      m_greenLed.set(value);
    } else {
      double currentTime = Timer.getTimestamp();
      if (currentTime > m_nextMessageTime) {
        DriverStation.reportError("Green LED was not configured", true);
        m_nextMessageTime = currentTime + MESSAGE_INTERVAL;
      }
    }
  }

  /**
   * Sets the red LED.
   *
   * @param value Set whether or not to turn the Red LED on
   */
  public void setRedLed(boolean value) {
    if (m_redLed != null) {
      m_redLed.set(value);
    } else {
      double currentTime = Timer.getTimestamp();
      if (currentTime > m_nextMessageTime) {
        DriverStation.reportError("Red LED was not configured", true);
        m_nextMessageTime = currentTime + MESSAGE_INTERVAL;
      }
    }
  }

  /**
   * Sets the yellow LED.
   *
   * @param value Set whether or not to turn the Yellow LED on
   */
  public void setYellowLed(boolean value) {
    m_yellowLed.set(value);
  }
}
