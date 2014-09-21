/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__

#include "Commands/Command.h"
#include "ErrorBase.h"
#include "SmartDashboard/NamedSendable.h"
#include "networktables/NetworkTable.h"
#include "networktables2/type/NumberArray.h"
#include "networktables2/type/StringArray.h"
#include "SmartDashboard/SmartDashboard.h"
#include "HAL/Semaphore.hpp"
#include <list>
#include <map>
#include <set>
#include <vector>

class ButtonScheduler;
class Subsystem;

class Scheduler : public ErrorBase, public NamedSendable
{
public:
	static Scheduler *GetInstance();

	void AddCommand(Command* command);
	void AddButton(ButtonScheduler* button);
	void RegisterSubsystem(Subsystem *subsystem);
	void Run();	
	void Remove(Command *command);
	void RemoveAll();
	void ResetAll();
	void SetEnabled(bool enabled);
	
	void UpdateTable();
	std::string GetSmartDashboardType();
	void InitTable(ITable *subTable);
	ITable * GetTable();
	std::string GetName();
	std::string GetType();

private:
	Scheduler();
	virtual ~Scheduler();

	void ProcessCommandAddition(Command *command);

	static Scheduler *_instance;
	Command::SubsystemSet m_subsystems;
	MUTEX_ID m_buttonsLock;
	typedef std::vector<ButtonScheduler *> ButtonVector;
	ButtonVector m_buttons;
	typedef std::vector<Command *> CommandVector;
	MUTEX_ID m_additionsLock;
	CommandVector m_additions;
	typedef std::set<Command *> CommandSet;
	CommandSet m_commands;
	bool m_adding;
	bool m_enabled;
	StringArray *commands;
	NumberArray *ids;
	NumberArray *toCancel;
	ITable *m_table;
	bool m_runningCommandsChanged;
};
#endif

