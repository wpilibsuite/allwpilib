// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.wpilibj.DriverStation;
import edu.wpi.first.wpilibj.GenericHID;

/** Class to control a simulated generic joystick. */
public class GenericHIDSim {
  /** GenericHID port. */
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

  /** Updates joystick data so that new values are visible to the user program. */
  public void notifyNewData() {
    DriverStationSim.notifyNewData();
  }

  /**
   * Set the value of a given button.
   *
   * @param button the button to set
   * @param value the new value
   */
  public void setRawButton(int button, boolean value) {
    DriverStationSim.setJoystickButton(m_port, button, value);
  }

  /**
   * Set the value of a given axis.
   *
   * @param axis the axis to set
   * @param value the new value
   */
  public void setRawAxis(int axis, double value) {
    DriverStationSim.setJoystickAxis(m_port, axis, value);
  }

  /**
   * Set the value of a given POV.
   *
   * @param pov the POV to set
   * @param value the new value
   */
  public void setPOV(int pov, DriverStation.POVDirection value) {
    DriverStationSim.setJoystickPOV(m_port, pov, value);
  }

  /**
   * Set the value of the default POV (port 0).
   *
   * @param value the new value
   */
  public void setPOV(DriverStation.POVDirection value) {
    setPOV(0, value);
  }

  /**
   * Set the maximum axis index for this device.
   *
   * @param maximumIndex the new maximum axis index
   */
  public void setAxesMaximumIndex(int maximumIndex) {
    DriverStationSim.setJoystickAxesMaximumIndex(m_port, maximumIndex);
  }

  /**
   * Set the axis count of this device.
   *
   * @param count the new axis count
   */
  public void setAxesAvailable(int count) {
    DriverStationSim.setJoystickAxesAvailable(m_port, count);
  }

  /**
   * Set the maximum POV index for this device.
   *
   * @param maximumIndex the new maximum POV index
   */
  public void setPOVsMaximumIndex(int maximumIndex) {
    DriverStationSim.setJoystickPOVsMaximumIndex(m_port, maximumIndex);
  }

  /**
   * Set the POV count of this device.
   *
   * @param count the new POV count
   */
  public void setPOVsAvailable(int count) {
    DriverStationSim.setJoystickPOVsAvailable(m_port, count);
  }

  /**
   * Set the maximum button index for this device.
   *
   * @param maximumIndex the new maximum button index
   */
  public void setButtonsMaximumIndex(int maximumIndex) {
    DriverStationSim.setJoystickButtonsMaximumIndex(m_port, maximumIndex);
  }

  /**
   * Set the button count of this device.
   *
   * @param count the new button count
   */
  public void setButtonsAvailable(long count) {
    DriverStationSim.setJoystickButtonsAvailable(m_port, count);
  }

  /**
   * Set the type of this device.
   *
   * @param type the new device type
   */
  public void setType(GenericHID.HIDType type) {
    DriverStationSim.setJoystickType(m_port, type.value);
  }

  /**
   * Set the name of this device.
   *
   * @param name the new device name
   */
  public void setName(String name) {
    DriverStationSim.setJoystickName(m_port, name);
  }

  /**
   * Read the output of a button.
   *
   * @param outputNumber the button number
   * @return the value of the button (true = pressed)
   */
  public boolean getOutput(int outputNumber) {
    long outputs = getOutputs();
    return (outputs & (1L << (outputNumber - 1))) != 0;
  }

  /**
   * Get the encoded 16-bit integer that passes button values.
   *
   * @return the button values
   */
  public long getOutputs() {
    return DriverStationSim.getJoystickOutputs(m_port);
  }

  /**
   * Get the joystick rumble.
   *
   * @param type the rumble to read
   * @return the rumble value
   */
  public double getRumble(GenericHID.RumbleType type) {
    int value =
        DriverStationSim.getJoystickRumble(
            m_port, type == GenericHID.RumbleType.kLeftRumble ? 0 : 1);
    return value / 65535.0;
  }
}
