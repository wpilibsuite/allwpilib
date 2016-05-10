/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "Commands/ConditionalCommand.h"
#include "command/MockCommand.h"

class MockConditionalCommand : public ConditionalCommand{
private:
	bool m_condition = false;
protected:
	virtual bool Condition();

public:
	void SetCondition(bool condition);
	MockConditionalCommand(MockCommand *onTrue, MockCommand *onFalse);
};
