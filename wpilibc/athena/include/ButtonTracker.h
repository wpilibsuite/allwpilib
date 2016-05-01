/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

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
  explicit ButtonTracker(uint32_t port);

  bool PressedButton(uint32_t button) const;
  bool ReleasedButton(uint32_t button) const;
  bool HeldButton(uint32_t button) const;

  void Update();

 private:
  static bool GetButtonState(uint32_t buttonStates, uint32_t button);

  uint32_t m_port;
  uint32_t m_oldStates = 0;
  uint32_t m_newStates = 0;
};
