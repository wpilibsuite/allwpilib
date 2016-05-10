/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "command/MockConditionalCommand.h"

bool MockConditionalCommand::Condition(){
	return m_condition;
}

void MockConditionalCommand::SetCondition(bool condition){
	m_condition = condition;
}

MockConditionalCommand::MockConditionalCommand(MockCommand *onTrue, MockCommand *onFalse) : ConditionalCommand(onTrue,onFalse){

}
