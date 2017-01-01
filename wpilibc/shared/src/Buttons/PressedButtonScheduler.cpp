/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Buttons/PressedButtonScheduler.h"

#include "Buttons/Button.h"
#include "Commands/Command.h"

using namespace frc;

PressedButtonScheduler::PressedButtonScheduler(bool last, Trigger* button,
                                               Command* orders)
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
