/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import edu.wpi.first.wpilibj.DriverStation;

/**
 * This class allows one to synchronously check whether a button was pressed, released, or held.
 *
 * <p>This is intended for teams not using the Command based robot framework, since that framework
 * already has the ability to trigger commands on button state changes.
 */
public class ButtonTracker {
  private int m_port;
  private int m_oldStates = 0;
  private int m_newStates = 0;

  public ButtonTracker(int port) {
    m_port = port;
  }

  /**
   * @param button The button number to be checked (starting at 1).
   * @return true if button wasn't pressed during last loop iteration but is now.
   */
  public boolean pressedButton(int button) {
    return getButtonState(m_oldStates, button) == false
        && getButtonState(m_newStates, button) == true;
  }

  /**
   * @param button The button number to be checked (starting at 1).
   * @return true if button was pressed during last loop iteration but isn't now.
   */
  public boolean releasedButton(int button) {
    return getButtonState(m_oldStates, button) == true
        && getButtonState(m_newStates, button) == false;
  }

  /**
   * @param button The button number to be checked (starting at 1).
   * @return true if button was pressed during last loop iteration and is now.
   */
  public boolean heldButton(int button) {
    return getButtonState(m_oldStates, button) == true
        && getButtonState(m_newStates, button) == true;
  }

  /**
   * Gets new button statuses for joystick from Driver Station.
   *
   * <p>This should be called once per loop iteration in OperatorControl().
   */
  public void update() {
    // "new" values are now "old"
    m_oldStates = m_newStates;

    // Get newer values
    m_newStates = DriverStation.getInstance().getStickButtons(m_port);
  }

  /**
   * @param buttonStates button statuses received from Driver Station.
   * @param button The button number to be checked (starting at 1).
   * @return true if button is currently pressed.
   */
  private boolean getButtonState(int buttonStates, int button) {
    return ((1 << (button - 1)) & buttonStates) != 0;
  }
}
