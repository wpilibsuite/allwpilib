/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "Commands/Scheduler.h"

#include "Buttons/ButtonScheduler.h"
#include "Commands/Subsystem.h"
#include "HLUsageReporting.h"
#include "HAL/cpp/Synchronized.hpp"
#include "WPIErrors.h"
#include <iostream>
#include <set>
#include <algorithm>

Scheduler *Scheduler::_instance = NULL;

Scheduler::Scheduler() :
	m_buttonsLock(NULL), m_additionsLock(NULL), m_adding(false) {
	m_buttonsLock = initializeMutexNormal();
	m_additionsLock = initializeMutexNormal();

    HLUsageReporting::ReportScheduler();

	m_table = NULL;
	m_enabled = true;
	m_runningCommandsChanged = false;
	toCancel = NULL;
	commands = NULL;
	ids = NULL;
}

Scheduler::~Scheduler() {
	takeMutex(m_additionsLock);
	deleteMutex(m_additionsLock);

	takeMutex(m_buttonsLock);
	deleteMutex(m_buttonsLock);
}

/**
 * Returns the {@link Scheduler}, creating it if one does not exist.
 * @return the {@link Scheduler}
 */
Scheduler *Scheduler::GetInstance() {
	if (_instance == NULL)
		_instance = new Scheduler();
	return _instance;
}

void Scheduler::SetEnabled(bool enabled) {
	m_enabled = enabled;
}

/**
 * Add a command to be scheduled later.
 * In any pass through the scheduler, all commands are added to the additions list, then
 * at the end of the pass, they are all scheduled.
 * @param command The command to be scheduled
 */
void Scheduler::AddCommand(Command *command) {
	Synchronized sync(m_additionsLock);
	if (std::find(m_additions.begin(), m_additions.end(), command)
			!= m_additions.end())
		return;
	m_additions.push_back(command);
}

void Scheduler::AddButton(ButtonScheduler *button) {
	Synchronized sync(m_buttonsLock);
	m_buttons.push_back(button);
}

void Scheduler::ProcessCommandAddition(Command *command) {
	if (command == NULL)
		return;

	// Check to make sure no adding during adding
	if (m_adding) {
		wpi_setWPIErrorWithContext(IncompatibleState, "Can not start command from cancel method");
		return;
	}

	// Only add if not already in
	CommandSet::iterator found = m_commands.find(command);
	if (found == m_commands.end()) {
		// Check that the requirements can be had
		Command::SubsystemSet requirements = command->GetRequirements();
		Command::SubsystemSet::iterator iter;
		for (iter = requirements.begin(); iter != requirements.end(); iter++) {
			Subsystem *lock = *iter;
			if (lock->GetCurrentCommand() != NULL
					&& !lock->GetCurrentCommand()->IsInterruptible())
				return;
		}

		// Give it the requirements
		m_adding = true;
		for (iter = requirements.begin(); iter != requirements.end(); iter++) {
			Subsystem *lock = *iter;
			if (lock->GetCurrentCommand() != NULL) {
				lock->GetCurrentCommand()->Cancel();
				Remove(lock->GetCurrentCommand());
			}
			lock->SetCurrentCommand(command);
		}
		m_adding = false;

		m_commands.insert(command);

		command->StartRunning();
		m_runningCommandsChanged = true;
	}
}

/**
 * Runs a single iteration of the loop.  This method should be called often in order to have a functioning
 * {@link Command} system.  The loop has five stages:
 *
 * <ol>
 * <li> Poll the Buttons </li>
 * <li> Execute/Remove the Commands </li>
 * <li> Send values to SmartDashboard </li>
 * <li> Add Commands </li>
 * <li> Add Defaults </li>
 * </ol>
 */
void Scheduler::Run() {
	// Get button input (going backwards preserves button priority)
	{
		if (!m_enabled)
			return;

		Synchronized sync(m_buttonsLock);
		ButtonVector::reverse_iterator rButtonIter = m_buttons.rbegin();
		for (; rButtonIter != m_buttons.rend(); rButtonIter++) {
			(*rButtonIter)->Execute();
		}
	}
	
	m_runningCommandsChanged = false;

	// Loop through the commands
	CommandSet::iterator commandIter = m_commands.begin();
	for (; commandIter != m_commands.end();) {
		Command *command = *commandIter;
		// Increment before potentially removing to keep the iterator valid
		commandIter++;
		if (!command->Run()) {
			Remove(command);
			m_runningCommandsChanged = true;
		}
	}

	// Add the new things
	{
		Synchronized sync(m_additionsLock);
		CommandVector::iterator additionsIter = m_additions.begin();
		for (; additionsIter != m_additions.end(); additionsIter++) {
			ProcessCommandAddition(*additionsIter);
		}
		m_additions.clear();
	}

	// Add in the defaults
	Command::SubsystemSet::iterator subsystemIter = m_subsystems.begin();
	for (; subsystemIter != m_subsystems.end(); subsystemIter++) {
		Subsystem *lock = *subsystemIter;
		if (lock->GetCurrentCommand() == NULL) {
			ProcessCommandAddition(lock->GetDefaultCommand());
		}
		lock->ConfirmCommand();
	}

	UpdateTable();
}

/**
 * Registers a {@link Subsystem} to this {@link Scheduler}, so that the {@link Scheduler} might know
 * if a default {@link Command} needs to be run.  All {@link Subsystem Subsystems} should call this.
 * @param system the system
 */
void Scheduler::RegisterSubsystem(Subsystem *subsystem) {
	if (subsystem == NULL) {
		wpi_setWPIErrorWithContext(NullParameter, "subsystem");
		return;
	}
	m_subsystems.insert(subsystem);
}

/**
 * Removes the {@link Command} from the {@link Scheduler}.
 * @param command the command to remove
 */
void Scheduler::Remove(Command *command) {
	if (command == NULL) {
		wpi_setWPIErrorWithContext(NullParameter, "command");
		return;
	}

	if (!m_commands.erase(command))
		return;

	Command::SubsystemSet requirements = command->GetRequirements();
	Command::SubsystemSet::iterator iter = requirements.begin();
	for (; iter != requirements.end(); iter++) {
		Subsystem *lock = *iter;
		lock->SetCurrentCommand(NULL);
	}

	command->Removed();
}

void Scheduler::RemoveAll() {
	while (m_commands.size() > 0) {
		Remove(*m_commands.begin());
	}
}

/**
 * Completely resets the scheduler. Undefined behavior if running.
 */
void Scheduler::ResetAll()
{
	RemoveAll();
	m_subsystems.clear();
	m_buttons.clear();
	m_additions.clear();
	m_commands.clear();
	m_table = NULL;
}

/**
 * Update the network tables associated with the Scheduler object on the SmartDashboard
 */
void Scheduler::UpdateTable() {
	CommandSet::iterator commandIter;
	if (m_table != NULL) {
		// Get the list of possible commands to cancel
		m_table->RetrieveValue("Cancel", *toCancel);
//		m_table->RetrieveValue("Ids", *ids);

		// cancel commands that have had the cancel buttons pressed
		// on the SmartDashboad
		if (toCancel->size() > 0) {
			for (commandIter = m_commands.begin(); commandIter
					!= m_commands.end(); ++commandIter) {
				for (unsigned i = 0; i < toCancel->size(); i++) {
					Command *c = *commandIter;
					if (c->GetID() == toCancel->get(i)) {
						c->Cancel();
					}
				}
			}
			toCancel->setSize(0);
			m_table->PutValue("Cancel", *toCancel);
		}
		
		// Set the running commands
		if (m_runningCommandsChanged) {
			commands->setSize(0);
			ids->setSize(0);
			for (commandIter = m_commands.begin(); commandIter != m_commands.end(); ++commandIter) {
				Command *c = *commandIter;
				commands->add(c->GetName());
				ids->add(c->GetID());
			}
			m_table->PutValue("Names", *commands);
			m_table->PutValue("Ids", *ids);
		}
	}
}

std::string Scheduler::GetName() {
	return "Scheduler";
}

std::string Scheduler::GetType() {
	return "Scheduler";
}

std::string Scheduler::GetSmartDashboardType() {
	return "Scheduler";
}

void Scheduler::InitTable(ITable *subTable) {
	m_table = subTable;
	commands = new StringArray();
	ids = new NumberArray();
	toCancel = new NumberArray();

	m_table->PutValue("Names", *commands);
	m_table->PutValue("Ids", *ids);
	m_table->PutValue("Cancel", *toCancel);
}

ITable * Scheduler::GetTable() {
	return m_table;
}
