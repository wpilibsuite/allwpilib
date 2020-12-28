// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/buttons/ButtonScheduler.h"

#include "frc/commands/Scheduler.h"

using namespace frc;

ButtonScheduler::ButtonScheduler(bool last, Trigger* button, Command* orders)
    : m_pressedLast(last), m_button(button), m_command(orders) {}

void ButtonScheduler::Start() {
  Scheduler::GetInstance()->AddButton(this);
}
