/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Buttons/ButtonScheduler.h"

#include "Commands/Scheduler.h"

using namespace frc;

ButtonScheduler::ButtonScheduler(bool last, Trigger* button, Command* orders)
    : m_pressedLast(last), m_button(button), m_command(orders) {}

void ButtonScheduler::Start() { Scheduler::GetInstance()->AddButton(this); }
