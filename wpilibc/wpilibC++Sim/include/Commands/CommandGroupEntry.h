/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#ifndef __COMMAND_GROUP_ENTRY_H__
#define __COMMAND_GROUP_ENTRY_H__

class Command;

class CommandGroupEntry
{
public:
	typedef enum {kSequence_InSequence, kSequence_BranchPeer, kSequence_BranchChild} Sequence;

	CommandGroupEntry();
	CommandGroupEntry(Command *command, Sequence state);
	CommandGroupEntry(Command *command, Sequence state, double timeout);
	bool IsTimedOut();

	double m_timeout;
	Command *m_command;
	Sequence m_state;
};

#endif
