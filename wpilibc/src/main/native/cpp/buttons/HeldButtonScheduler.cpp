/*----------------------------------------------------------------------------*/
/* Copyright (c) 2011-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/buttons/HeldButtonScheduler.h"

#include "frc/buttons/Button.h"
#include "frc/commands/Command.h"

using namespace frc;

HeldButtonScheduler::HeldButtonScheduler(bool last, Trigger* button,
                                         Command* orders)
    : ButtonScheduler(last, button, orders) {}

void HeldButtonScheduler::Execute() {
  bool pressed = m_button->Grab();

  if (pressed) {
    m_command->Start();
  } else if (m_pressedLast && !pressed) {
    m_command->Cancel();
  }

  m_pressedLast = pressed;
}
