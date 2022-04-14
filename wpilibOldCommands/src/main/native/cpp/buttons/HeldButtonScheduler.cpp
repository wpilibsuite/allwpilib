// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

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
