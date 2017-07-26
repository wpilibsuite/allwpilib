/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Buttons/ButtonScheduler.h"

#include "Commands/Scheduler.h"

frc::ButtonScheduler::ButtonScheduler(bool last, frc::Trigger* button,
                                      frc::Command* orders)
    : m_pressedLast(last), m_button(button), m_command(orders) {}

void frc::ButtonScheduler::Start() {
  Scheduler::GetInstance()->AddButton(this);
}
