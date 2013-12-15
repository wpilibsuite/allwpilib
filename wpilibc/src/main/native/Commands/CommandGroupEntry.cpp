/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "Commands/CommandGroupEntry.h"

#include "Commands/Command.h"

CommandGroupEntry::CommandGroupEntry() :
	m_timeout(-1.0),
	m_command(NULL),
	m_state(kSequence_InSequence)
{
}

CommandGroupEntry::CommandGroupEntry(Command *command, Sequence state) :
	m_timeout(-1.0),
	m_command(command),
	m_state(state)
{
}

CommandGroupEntry::CommandGroupEntry(Command *command, Sequence state, double timeout) :
	m_timeout(timeout),
	m_command(command),
	m_state(state)
{
}

bool CommandGroupEntry::IsTimedOut()
{
	if (m_timeout < 0.0)
		return false;
	double time = m_command->TimeSinceInitialized();
	if (time == 0.0)
		return false;
	return time >= m_timeout;
}
