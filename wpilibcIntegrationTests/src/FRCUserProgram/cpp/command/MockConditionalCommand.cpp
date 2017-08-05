/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2017. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "command/MockConditionalCommand.h"

using namespace frc;

MockConditionalCommand::MockConditionalCommand(MockCommand* onTrue,
                                               MockCommand* onFalse)
    : ConditionalCommand(onTrue, onFalse) {}

void MockConditionalCommand::SetCondition(bool condition) {
  m_condition = condition;
}

bool MockConditionalCommand::Condition() { return m_condition; }
