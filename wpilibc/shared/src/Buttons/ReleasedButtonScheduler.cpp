/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Buttons/ReleasedButtonScheduler.h"

#include "Buttons/Button.h"
#include "Commands/Command.h"

using namespace frc;

ReleasedButtonScheduler::ReleasedButtonScheduler(bool last, Trigger* button,
                                                 Command* orders)
    : ButtonScheduler(last, button, orders) {}

void ReleasedButtonScheduler::Execute() {
  if (m_button->Grab()) {
    m_pressedLast = true;
  } else {
    if (m_pressedLast) {
      m_pressedLast = false;
      m_command->Start();
    }
  }
}
