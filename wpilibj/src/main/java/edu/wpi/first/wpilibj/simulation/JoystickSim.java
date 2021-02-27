// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.wpilibj.Joystick;

/** Class to control a simulated joystick. */
public class JoystickSim extends GenericHIDSim {
  private Joystick m_joystick;

  /**
   * Constructs from a Joystick object.
   *
   * @param joystick joystick to simulate
   */
  public JoystickSim(Joystick joystick) {
    super(joystick);
    m_joystick = joystick;
    // default to a reasonable joystick configuration
    setAxisCount(5);
    setButtonCount(12);
    setPOVCount(1);
  }

  /**
   * Constructs from a joystick port number.
   *
   * @param port port number
   */
  public JoystickSim(int port) {
    super(port);
    // default to a reasonable joystick configuration
    setAxisCount(5);
    setButtonCount(12);
    setPOVCount(1);
  }

  /**
   * Set the X value of the joystick.
   *
   * @param value the new X value
   */
  public void setX(double value) {
    setRawAxis(m_joystick != null ? m_joystick.getXChannel() : Joystick.kDefaultXChannel, value);
  }

  /**
   * Set the Y value of the joystick.
   *
   * @param value the new Y value
   */
  public void setY(double value) {
    setRawAxis(m_joystick != null ? m_joystick.getYChannel() : Joystick.kDefaultYChannel, value);
  }

  /**
   * Set the Z value of the joystick.
   *
   * @param value the new Z value
   */
  public void setZ(double value) {
    setRawAxis(m_joystick != null ? m_joystick.getZChannel() : Joystick.kDefaultZChannel, value);
  }

  /**
   * Set the twist value of the joystick.
   *
   * @param value the new twist value
   */
  public void setTwist(double value) {
    setRawAxis(
        m_joystick != null ? m_joystick.getTwistChannel() : Joystick.kDefaultTwistChannel, value);
  }

  /**
   * Set the throttle value of the joystick.
   *
   * @param value the new throttle value
   */
  public void setThrottle(double value) {
    setRawAxis(
        m_joystick != null ? m_joystick.getThrottleChannel() : Joystick.kDefaultThrottleChannel,
        value);
  }

  /**
   * Set the trigger value of the joystick.
   *
   * @param state the new value
   */
  public void setTrigger(boolean state) {
    setRawButton(1, state);
  }

  /**
   * Set the top state of the joystick.
   *
   * @param state the new state
   */
  public void setTop(boolean state) {
    setRawButton(2, state);
  }
}
