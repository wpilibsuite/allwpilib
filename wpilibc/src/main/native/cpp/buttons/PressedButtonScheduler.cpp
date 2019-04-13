/*----------------------------------------------------------------------------*/
/* Copyright (c) 2011-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/buttons/PressedButtonScheduler.h"

#include "frc/buttons/Button.h"
#include "frc/commands/Command.h"

using namespace frc;

PressedButtonScheduler::PressedButtonScheduler(bool last, Trigger* button,
                                               Command* orders)
    : ButtonScheduler(last, button, orders) {}

void PressedButtonScheduler::Execute() {
  bool pressed = m_button->Grab();

  if (!m_pressedLast && pressed) {
    m_command->Start();
  }

  m_pressedLast = pressed;
}
