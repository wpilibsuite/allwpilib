/*----------------------------------------------------------------------------*/
/* Copyright (c) 2011-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/buttons/ReleasedButtonScheduler.h"

#include "frc/buttons/Button.h"
#include "frc/commands/Command.h"

using namespace frc;

ReleasedButtonScheduler::ReleasedButtonScheduler(bool last, Trigger* button,
                                                 Command* orders)
    : ButtonScheduler(last, button, orders) {}

void ReleasedButtonScheduler::Execute() {
  bool state = m_button->GetSendableState();

  if (m_lastState && !state) {
    // If sendable button was pressed and isn't now, start command
    m_command->Start();
  } else if (m_button->GetReleased()) {
    // Otherwise, check whether the normal button has been released
    m_command->Start();
  }

  m_lastState = state;
}
