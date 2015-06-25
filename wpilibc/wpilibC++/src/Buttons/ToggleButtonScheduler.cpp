/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011. All Rights Reserved.
 */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "Buttons/ToggleButtonScheduler.h"

#include "Buttons/Button.h"
#include "Commands/Command.h"

ToggleButtonScheduler::ToggleButtonScheduler(bool last, Trigger *button,
                                             Command *orders)
    : ButtonScheduler(last, button, orders) {
  pressedLast = m_button->Grab();
}

void ToggleButtonScheduler::Execute() {
  if (m_button->Grab()) {
    if (!pressedLast) {
      pressedLast = true;
      if (m_command->IsRunning()) {
        m_command->Cancel();
      } else {
        m_command->Start();
      }
    }
  } else {
    pressedLast = false;
  }
}
