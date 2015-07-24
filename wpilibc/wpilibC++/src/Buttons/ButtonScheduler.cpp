/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011. All Rights Reserved.
 */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "Buttons/ButtonScheduler.h"

#include "Commands/Scheduler.h"

ButtonScheduler::ButtonScheduler(bool last, Trigger *button, Command *orders)
    : m_pressedLast(last), m_button(button), m_command(orders) {}

void ButtonScheduler::Start() { Scheduler::GetInstance()->AddButton(this); }
