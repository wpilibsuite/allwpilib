/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#ifndef __SUBSYSTEM_H__
#define __SUBSYSTEM_H__

#include "ErrorBase.h"
#include "SmartDashboard/NamedSendable.h"
#include <string>


class Command;

class Subsystem : public ErrorBase, public NamedSendable
{
	friend class Scheduler;
public:
	Subsystem(const char *name);
	virtual ~Subsystem() {}

	void SetDefaultCommand(Command *command);
	Command *GetDefaultCommand();
	void SetCurrentCommand(Command *command);
	Command *GetCurrentCommand();
	virtual void InitDefaultCommand();
	
private:
	void ConfirmCommand();

	Command *m_currentCommand;
	bool m_currentCommandChanged;
	Command *m_defaultCommand;
	std::string m_name;
	bool m_initializedDefaultCommand;

public:
	virtual std::string GetName();
	virtual void InitTable(ITable* table);
	virtual ITable* GetTable();
	virtual std::string GetSmartDashboardType();
protected:
	ITable* m_table;
};

#endif
