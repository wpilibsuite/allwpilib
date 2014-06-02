/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#ifndef __COMMAND_GROUP_H__
#define __COMMAND_GROUP_H__

#include "Commands/Command.h"
#include "Commands/CommandGroupEntry.h"
#include <list>
#include <vector>

/**
 * A {@link CommandGroup} is a list of commands which are executed in sequence.
 *
 * <p>Commands in a {@link CommandGroup} are added using the {@link CommandGroup#AddSequential(Command) AddSequential(...)} method
 * and are called sequentially.
 * {@link CommandGroup CommandGroups} are themselves {@link Command Commands}
 * and can be given to other {@link CommandGroup CommandGroups}.</p>
 *
 * <p>{@link CommandGroup CommandGroups} will carry all of the requirements of their {@link Command subcommands}.  Additional
 * requirements can be specified by calling {@link CommandGroup#Requires(Subsystem) Requires(...)}
 * normally in the constructor.</P>
 *
 * <p>CommandGroups can also execute commands in parallel, simply by adding them
 * using {@link CommandGroup#AddParallel(Command) AddParallel(...)}.</p>
 * 
 * @see Command
 * @see Subsystem
 */
class CommandGroup : public Command
{
public:
	CommandGroup();
	CommandGroup(const char *name);
	virtual ~CommandGroup();
	
	void AddSequential(Command *command);
	void AddSequential(Command *command, double timeout);
	void AddParallel(Command *command);
	void AddParallel(Command *command, double timeout);
	bool IsInterruptible();
	int GetSize();

protected:
	virtual void Initialize();
	virtual void Execute();
	virtual bool IsFinished();
	virtual void End();
	virtual void Interrupted();
	virtual void _Initialize();
	virtual void _Interrupted();
	virtual void _Execute();
	virtual void _End();

private:
	void CancelConflicts(Command *command);

	typedef std::vector<CommandGroupEntry> CommandVector;
	/** The commands in this group (stored in entries) */
	CommandVector m_commands;
	typedef std::list<CommandGroupEntry> CommandList;
	/** The active children in this group (stored in entries) */
	CommandList m_children;
	/** The current command, -1 signifies that none have been run */
	int m_currentCommandIndex;
};

#endif
