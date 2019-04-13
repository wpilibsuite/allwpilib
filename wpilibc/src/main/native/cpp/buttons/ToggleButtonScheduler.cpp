/*----------------------------------------------------------------------------*/
/* Copyright (c) 2011-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/buttons/ToggleButtonScheduler.h"

#include "frc/buttons/Button.h"
#include "frc/commands/Command.h"

using namespace frc;

ToggleButtonScheduler::ToggleButtonScheduler(bool last, Trigger* button,
                                             Command* orders)
    : ButtonScheduler(last, button, orders) {}

void ToggleButtonScheduler::Execute() {
  bool pressed = m_button->Grab();

  if (!m_pressedLast && pressed) {
    if (m_command->IsRunning()) {
      m_command->Cancel();
    } else {
      m_command->Start();
    }
  }

  m_pressedLast = pressed;
}
