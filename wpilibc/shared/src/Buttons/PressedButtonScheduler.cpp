/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011. All Rights Reserved.
 */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "Buttons/PressedButtonScheduler.h"

#include "Buttons/Button.h"
#include "Commands/Command.h"

PressedButtonScheduler::PressedButtonScheduler(bool last, Trigger *button,
                                               Command *orders)
    : ButtonScheduler(last, button, orders) {}

void PressedButtonScheduler::Execute() {
  if (m_button->Grab()) {
    if (!m_pressedLast) {
      m_pressedLast = true;
      m_command->Start();
    }
  } else {
    m_pressedLast = false;
  }
}
