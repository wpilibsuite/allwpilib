/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "ButtonTracker.h"

#include "DriverStation.h"

using namespace frc;

ButtonTracker::ButtonTracker(int port) { m_port = port; }

/**
 * @param button The button number to be checked (starting at 1).
 * @return true if button wasn't pressed during last loop iteration but is now.
 */
bool ButtonTracker::PressedButton(int button) const {
  return !GetButtonState(m_oldStates, button) &&
         GetButtonState(m_newStates, button);
}

/**
 * @param button The button number to be checked (starting at 1).
 * @return true if button was pressed during last loop iteration but isn't now.
 */
bool ButtonTracker::ReleasedButton(int button) const {
  return GetButtonState(m_oldStates, button) &&
         !GetButtonState(m_newStates, button);
}

/**
 * @param button The button number to be checked (starting at 1).
 * @return true if button was pressed during last loop iteration and is now.
 */
bool ButtonTracker::HeldButton(int button) const {
  return GetButtonState(m_oldStates, button) &&
         GetButtonState(m_newStates, button);
}

/**
 * @param buttonStates button statuses received from Driver Station.
 * @param button The button number to be checked (starting at 1).
 * @return true if button is currently pressed.
 */
bool ButtonTracker::GetButtonState(int buttonStates, int button) {
  return ((1 << (button - 1)) & buttonStates) != 0;
}

/**
 * Gets new button statuses for joystick from Driver Station.
 *
 * This should be called once per loop iteration in OperatorControl().
 */
void ButtonTracker::Update() {
  // "new" values are now "old"
  m_oldStates = m_newStates;

  // Get newer values
  m_newStates = DriverStation::GetInstance().GetStickButtons(m_port);
}
