/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.wpilibj.Joystick;

/**
 * Class to control a simulated joystick.
 */
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

  public void setX(double value) {
    setRawAxis(m_joystick != null ? m_joystick.getXChannel() : Joystick.kDefaultXChannel, value);
  }

  public void setY(double value) {
    setRawAxis(m_joystick != null ? m_joystick.getYChannel() : Joystick.kDefaultYChannel, value);
  }

  public void setZ(double value) {
    setRawAxis(m_joystick != null ? m_joystick.getZChannel() : Joystick.kDefaultZChannel, value);
  }

  public void setTwist(double value) {
    setRawAxis(
        m_joystick != null ? m_joystick.getTwistChannel() : Joystick.kDefaultTwistChannel,
        value);
  }

  public void setThrottle(double value) {
    setRawAxis(
        m_joystick != null ? m_joystick.getThrottleChannel() : Joystick.kDefaultThrottleChannel,
        value);
  }

  public void setTrigger(boolean state) {
    setRawButton(1, state);
  }

  public void setTop(boolean state) {
    setRawButton(2, state);
  }
}
