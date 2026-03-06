// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.driverstation;

/**
 * A default implementation of UserControls that provides Gamepad instances for each of the 6
 * joystick ports provided by the DS.
 */
public class DefaultUserControls implements UserControls {
  private final Gamepad[] m_gamepads;

  /** Constructs a DefaultUserControls instance with Gamepads for each port. */
  public DefaultUserControls() {
    m_gamepads = new Gamepad[DriverStation.kJoystickPorts];
    for (int i = 0; i < m_gamepads.length; i++) {
      m_gamepads[i] = new Gamepad(i);
    }
  }

  /**
   * Returns the Gamepad instance for the specified port.
   *
   * @param port The joystick port number.
   * @return The Gamepad instance for the given port.
   */
  public Gamepad getGamepad(int port) {
    return m_gamepads[port];
  }
}
