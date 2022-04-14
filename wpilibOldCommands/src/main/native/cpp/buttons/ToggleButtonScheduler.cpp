// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

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
