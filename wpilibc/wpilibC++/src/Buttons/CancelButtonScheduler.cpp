/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011. All Rights Reserved.
 */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "Buttons/CancelButtonScheduler.h"

#include "Buttons/Button.h"
#include "Commands/Command.h"

CancelButtonScheduler::CancelButtonScheduler(bool last, Trigger *button,
                                             Command *orders)
    : ButtonScheduler(last, button, orders) {
  pressedLast = m_button->Grab();
}

void CancelButtonScheduler::Execute() {
  if (m_button->Grab()) {
    if (!pressedLast) {
      pressedLast = true;
      m_command->Cancel();
    }
  } else {
    pressedLast = false;
  }
}
