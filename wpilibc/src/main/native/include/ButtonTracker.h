/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

namespace frc {

/**
 * This class allows one to synchronously check whether a button was pressed,
 * released, or held.
 *
 * This is intended for teams not using the Command based robot framework, since
 * that framework already has the ability to trigger commands on button state
 * changes.
 */
class ButtonTracker {
 public:
  explicit ButtonTracker(int port);

  bool PressedButton(int button) const;
  bool ReleasedButton(int button) const;
  bool HeldButton(int button) const;

  void Update();

 private:
  static bool GetButtonState(int buttonStates, int button);

  int m_port;
  int m_oldStates = 0;
  int m_newStates = 0;
};

}  // namespace frc
