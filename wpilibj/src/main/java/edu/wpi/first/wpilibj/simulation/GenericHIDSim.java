/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.wpilibj.GenericHID;

/**
 * Class to control a simulated generic joystick.
 */
public class GenericHIDSim {
  protected final int m_port;

  /**
   * Constructs from a GenericHID object.
   *
   * @param joystick joystick to simulate
   */
  public GenericHIDSim(GenericHID joystick) {
    m_port = joystick.getPort();
  }

  /**
   * Constructs from a joystick port number.
   *
   * @param port port number
   */
  public GenericHIDSim(int port) {
    m_port = port;
  }

  /**
   * Updates joystick data so that new values are visible to the user program.
   */
  public void notifyNewData() {
    DriverStationSim.notifyNewData();
  }

  public void setRawButton(int button, boolean value) {
    DriverStationSim.setJoystickButton(m_port, button, value);
  }

  public void setRawAxis(int axis, double value) {
    DriverStationSim.setJoystickAxis(m_port, axis, value);
  }

  public void setPOV(int pov, int value) {
    DriverStationSim.setJoystickPOV(m_port, pov, value);
  }

  public void setPOV(int value) {
    setPOV(0, value);
  }

  public void setAxisCount(int count) {
    DriverStationSim.setJoystickAxisCount(m_port, count);
  }

  public void setPOVCount(int count) {
    DriverStationSim.setJoystickPOVCount(m_port, count);
  }

  public void setButtonCount(int count) {
    DriverStationSim.setJoystickButtonCount(m_port, count);
  }

  public void setType(GenericHID.HIDType type) {
    DriverStationSim.setJoystickType(m_port, type.value);
  }

  public void setName(String name) {
    DriverStationSim.setJoystickName(m_port, name);
  }

  public void setAxisType(int axis, int type) {
    DriverStationSim.setJoystickAxisType(m_port, axis, type);
  }

  public boolean getOutput(int outputNumber) {
    long outputs = getOutputs();
    return (outputs & (1 << (outputNumber - 1))) != 0;
  }

  public long getOutputs() {
    return DriverStationSim.getJoystickOutputs(m_port);
  }

  public double getRumble(GenericHID.RumbleType type) {
    int value = DriverStationSim.getJoystickRumble(
        m_port, type == GenericHID.RumbleType.kLeftRumble ? 0 : 1);
    return value / 65535.0;
  }
}
